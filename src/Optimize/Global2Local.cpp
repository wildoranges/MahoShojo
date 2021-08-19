#include <Global2Local.h>
#include <queue>

void Global2Local::execute(){
    auto sea = std::make_unique<SideEffectAnalysis>(module);
    sea->execute();
    analyse();
    for (auto pair : localize_info){
        if (pair.second.size()>0){
            localize(pair.first.first, pair.first.second);
        }
    }
}

void Global2Local::analyse(){
    for (auto global : module->get_global_variable()){
        if (global->get_type()->get_pointer_element_type()->is_array_type()){
            continue;
        }
        for (auto use_pair : global->get_use_list()){
            auto use_inst = dynamic_cast<Instruction *>(use_pair.val_);
            if (use_inst != nullptr){
                auto use_func = use_inst->get_parent()->get_parent();
                localize_info[{global, use_func}].push_back(use_inst);
            }
        }
    }
}

void Global2Local::localize(GlobalVariable *global, Function *func){
    //load the global variable in entry for use in the function
    std::map<BasicBlock *, Value *> bb2local_global;
    auto func_entry = func->get_entry_block();
    auto entry_insts = &func_entry->get_instructions();    
    auto entry_load_inst = LoadInst::create_load(global->get_type()->get_pointer_element_type(),global,func_entry);
    entry_insts->pop_back();
    entry_insts->push_front(entry_load_inst);
    bb2local_global[func_entry] = entry_load_inst;

    //search all the instructions of func and replace the loads and stores of global
    std::map<BasicBlock *, int> color;
    std::queue<BasicBlock *> BB_queue;
    std::vector<Instruction *> need_delete;
    std::map<BasicBlock *, PhiInst *> local_global_phis;
    std::queue<PhiInst *> can_delete_phis;
    BB_queue.push(func_entry);
    while (!BB_queue.empty()){
        auto cur_BB = BB_queue.front();
        BB_queue.pop();
        color[cur_BB] = 1;
        //instructions handle
        for (auto iter_inst = cur_BB->get_instructions().begin(); iter_inst != cur_BB->get_instructions().end(); iter_inst++){
            auto inst = *iter_inst;
            if(inst == entry_load_inst){
                continue;
            }
            if (inst->is_load()){
                auto load_addr = inst->get_operand(0);
                if (load_addr == global){
                    need_delete.push_back(inst);
                    inst->replace_all_use_with(bb2local_global[cur_BB]);
                }
            }
            if (inst->is_store()){
                auto store_val = inst->get_operand(0);
                auto store_addr = inst->get_operand(1);
                if (store_addr == global){
                    need_delete.push_back(inst);
                    bb2local_global[cur_BB] = store_val;
                }
            }
            if (inst->is_call()){
                auto call_func = dynamic_cast<Function *>(inst->get_operand(0));
                //analyse of side_effect
                auto call_load = call_func->get_global_var_side_effect_load();
                auto call_store = call_func->get_global_var_side_effect_store();
                //if all have side effect
                if (bb2local_global[cur_BB] != entry_load_inst){
                    //store is need when local global is changed
                    if (call_load.find(global) != call_load.end()){
                        //when the call func need real value of global
                        auto tmp_store = StoreInst::create_store(bb2local_global[cur_BB], global, cur_BB);
                        cur_BB->get_instructions().pop_back();
                        cur_BB->add_instruction(iter_inst, tmp_store);
                    }
                }
                //load is after call
                if (call_store.find(global) != call_store.end()){
                    //when call func changed global
                    iter_inst++;
                    auto tmp_load = LoadInst::create_load(global->get_type()->get_pointer_element_type(), global, cur_BB);
                    cur_BB->get_instructions().pop_back();
                    cur_BB->add_instruction(iter_inst, tmp_load);
                    iter_inst--;
                    bb2local_global[cur_BB] = tmp_load;
                }
                
            }
        }
        //delete load and store
        for (auto delete_inst : need_delete){
            delete_inst->get_parent()->delete_instr(delete_inst);
        }
        //search succ BBs, move current local global
        for (auto succ_BB : cur_BB->get_succ_basic_blocks()){
            if (succ_BB->get_pre_basic_blocks().size() == 1){
                //succ BB only have one pre BB
                bb2local_global[succ_BB] = bb2local_global[cur_BB];
                BB_queue.push(succ_BB);
            }
            else {
                //analyse when all pre BB are analysed
                bool ready = true;
                for (auto succ_pre : succ_BB->get_pre_basic_blocks()){
                    if (color[succ_pre] != 1){
                        ready = false;
                        break;
                    }
                }
                if (ready){
                    //all pre BB are analysed
                    //get the local global of succ BB
                    //fill the no-operand phi
                    //check if phi is need
                    auto phi_global = local_global_phis[succ_BB];
                    Value * first_local_global = nullptr;
                    bool all_same = true;
                    for (auto succ_pre : succ_BB->get_pre_basic_blocks()){
                        if (bb2local_global[succ_pre] == phi_global){
                            continue;
                        }
                        if (first_local_global == nullptr){
                            first_local_global = bb2local_global[succ_pre];
                        }
                        else {
                            auto const_first_local_global = dynamic_cast<ConstantInt *>(first_local_global);
                            auto local_global = bb2local_global[succ_pre];
                            auto const_local_global = dynamic_cast<ConstantInt *>(local_global);
                            if (const_first_local_global != nullptr && const_local_global != nullptr){
                                if (const_first_local_global->get_value() != const_local_global->get_value()){
                                    all_same = false;
                                    break;
                                }
                            }
                            else if (first_local_global != local_global){
                                all_same = false;
                                break;
                            }
                        }
                    }
                    if (all_same){
                       //record the phis which can be deleted
                       can_delete_phis.push(phi_global);
                    }
                    //fill the phi
                    for (auto succ_pre : succ_BB->get_pre_basic_blocks()){
                        phi_global->add_phi_pair_operand(bb2local_global[succ_pre], succ_pre);
                    }
                }
                else if (color[succ_BB] != 1) {
                    //if there is a loop
                    //a bb has to be searched before it's pre BB
                    //so we have to search a BB before it's pre BB
                    //create a phi without operands
                    if (local_global_phis[succ_BB] == nullptr){
                        //I cannot create a lot of phis
                        auto phi_global = PhiInst::create_phi(global->get_type()->get_pointer_element_type(), succ_BB);
                        succ_BB->add_instr_begin(phi_global);
                        bb2local_global[succ_BB] = phi_global;
                        local_global_phis[succ_BB] = phi_global;
                        BB_queue.push(succ_BB);
                    }
                }
            }
        }
        //when function returns, store the global
        auto terminator = cur_BB->get_terminator();
        if (terminator->is_ret()){
            if (bb2local_global[cur_BB] != entry_load_inst){
                //store is need when local global is changed
                cur_BB->get_instructions().pop_back();
                StoreInst::create_store(bb2local_global[cur_BB], global, cur_BB);
                cur_BB->add_instruction(terminator);
            }
        }
    }
    //delete useless phis
    std::set<PhiInst *> wait_for_delete;
    while (false && !can_delete_phis.empty()){
        auto can_delete_phi = can_delete_phis.front();
        can_delete_phis.pop();
        Value *replace_val = nullptr;
        for (auto i = 0; i < can_delete_phi->get_num_operand(); i+=2){
            if (can_delete_phi->get_operand(i) != can_delete_phi){
                replace_val = can_delete_phi->get_operand(i);
            }
        }
        for (auto use_pair : can_delete_phi->get_use_list()){
            auto use_inst = dynamic_cast<Instruction *>(use_pair.val_);
            if (use_inst!=nullptr){
                //replace all use with first_local_global
                use_inst->set_operand(use_pair.arg_no_, replace_val);
                if (use_inst->is_phi() || local_global_phis[use_inst->get_parent()]==use_inst ){
                    //when use_inst is a phi created by G2L
                    //check if it should be deleted
                    bool all_same = true;
                    Value *first_local_global = nullptr;
                    for (auto i = 0; i < use_inst->get_num_operand(); i+=2){
                        if (use_inst->get_operand(i) == use_inst){
                            continue;
                        }
                        if (first_local_global == nullptr){
                            first_local_global = use_inst->get_operand(i);
                        }
                        else{
                            auto const_first_local_global = dynamic_cast<ConstantInt *>(first_local_global);
                            auto local_global = use_inst->get_operand(i);
                            auto const_local_global = dynamic_cast<ConstantInt *>(local_global);
                            if (const_first_local_global != nullptr && const_local_global != nullptr){
                                if (const_first_local_global->get_value() != const_local_global->get_value()){
                                    all_same = false;
                                    break;
                                }
                            }
                            else if (first_local_global != local_global){
                                all_same = false;
                                break;
                            }
                        }
                    }
                    if (all_same){
                        can_delete_phis.push(dynamic_cast<PhiInst *>(use_inst));
                    }
                }
            }
        }
        can_delete_phi->get_use_list().clear();
        wait_for_delete.insert(can_delete_phi);
    }
    for (auto local_global_phi : wait_for_delete){
        local_global_phi->get_parent()->delete_instr(local_global_phi);
    }
}