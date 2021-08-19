#include "LoopInvariant.h"

void LoopInvariant::execute(){
    CFG_analyser = std::make_unique<CFG_analyse>(module);
    CFG_analyser->execute();


    auto loops = CFG_analyser->get_loops();
    for (auto loop : *loops){
        bool inner = true;
        for (auto BB : *loop){
            if (CFG_analyser->find_bb_loop(BB) != loop){
                inner = false;
                break;
            }
        }
        if (inner == false){
            continue;
        }

        while (loop != nullptr){
            invariants.clear();
            invariants_find(loop);
            if (!invariants.empty()){
                auto first_BB = CFG_analyser->find_loop_entry(loop);
                BasicBlock* new_BB = BasicBlock::create(module, "", first_BB->get_parent());

                //find pre_BBs
                std::vector<BasicBlock*> pre_BBs;
                for (auto pre_BB : first_BB->get_pre_basic_blocks()){
                    if (CFG_analyser->find_bb_loop(pre_BB) != loop){
                        pre_BBs.push_back(pre_BB);
                    }
                }

                //phi in loop entry
                for (auto iter_inst = first_BB->get_instructions().begin(); iter_inst != first_BB->get_instructions().end(); iter_inst++){
                    auto inst = *iter_inst;
                    if (inst->is_phi() == false){
                        break;
                    }
                    auto inst_phi = dynamic_cast<PhiInst *>(inst);
                    std::vector<std::pair<Value*,BasicBlock*>> ops_in_loop;
                    std::vector<std::pair<Value*,BasicBlock*>> ops_out_loop;
                    for (auto i = 0; i < inst->get_num_operand(); i=i+2){
                        auto val = inst->get_operand(i);
                        auto from_BB = dynamic_cast<BasicBlock*>(inst->get_operand(i+1));
                        val->remove_use(inst);
                        from_BB->remove_use(inst);

                        if (CFG_analyser->find_bb_loop(from_BB) == loop){
                            ops_in_loop.push_back({val,from_BB});
                        }
                        else{
                            ops_out_loop.push_back({val,from_BB});
                        }
                    }

                    if (ops_out_loop.size()>1){
                        //more than 1 value out of loop
                        //need phi in new_BB
                        auto new_phi = PhiInst::create_phi((*ops_out_loop.begin()).first->get_type(),new_BB);
                        new_BB->add_instruction(new_phi);
                        for (auto pair : ops_out_loop){
                            new_phi->add_phi_pair_operand(pair.first,pair.second);
                        }
                        
                        inst_phi->remove_operands(0, inst->get_num_operand()-1);
                        for (auto pair : ops_in_loop){
                            inst_phi->add_phi_pair_operand(pair.first,pair.second);
                        }
                        inst_phi->add_phi_pair_operand(new_phi, new_BB);
                    }
                    else{
                        //not need a new phi
                        inst_phi->remove_operands(0, inst->get_num_operand()-1);
                        for (auto pair : ops_out_loop){
                            inst_phi->add_phi_pair_operand(pair.first,new_BB);
                        }
                        for (auto pair : ops_in_loop){
                            inst_phi->add_phi_pair_operand(pair.first,pair.second);
                        }
                    }
                }

                //invariant move
                for (auto pair : invariants){
                    for (auto inst : pair.second){
                        pair.first->get_instructions().remove(inst);
                        new_BB->add_instruction(inst);
                        inst->set_parent(new_BB);
                    }
                }
                //BB br change
                BranchInst::create_br(first_BB, new_BB);
                for (auto pre_BB : pre_BBs){
                    auto terminator = pre_BB->get_terminator();
                    if (terminator->get_num_operand() == 1){
                        terminator->set_operand(0, new_BB);
                    }
                    else{
                        if (dynamic_cast<BasicBlock*>(terminator->get_operand(1)) == first_BB){
                            terminator->set_operand(1, new_BB);
                        }
                        else{
                            terminator->set_operand(2, new_BB);
                        }
                    }
                    first_BB->remove_use(terminator);
                    first_BB->remove_pre_basic_block(pre_BB);
                    pre_BB->remove_succ_basic_block(first_BB);
                    new_BB->add_pre_basic_block(pre_BB);
                    pre_BB->add_succ_basic_block(new_BB);
                }
            }
            loop = CFG_analyser->find_outer_loop(loop);
        }
    }
}

void LoopInvariant::invariants_find(std::vector<BasicBlock *>* loop){
    std::set<Value *> defined_in_loop;
    std::set<Instruction *> invariant;
    for (auto BB : *loop){
        for (auto inst : BB->get_instructions()){
            if (inst->is_store()){
                defined_in_loop.insert(inst->get_operand(1));
            }
            else{
                defined_in_loop.insert(inst);
            }
        }
    }
    bool change = false;
    do {
        change = false;
        for (auto BB : *loop){
            invariant.clear();
            for (auto inst : BB->get_instructions()){
                bool invariant_check = true;
                if (inst->is_call()||inst->is_alloca()||inst->is_ret()||inst->is_br()||inst->is_cmp()||inst->is_phi()||inst->is_load()){
                    continue;
                }
                if (defined_in_loop.find(inst) == defined_in_loop.end()){
                    continue;
                }
                for (auto operand : inst->get_operands()){
                    if (defined_in_loop.find(operand) != defined_in_loop.end()){
                        invariant_check = false;
                    }
                }
                if (invariant_check){
                    defined_in_loop.erase(inst);
                    invariant.insert(inst);
                    change = true;
                }
            }
            if (!invariant.empty()){
                invariants.push_back({BB,invariant});
            }
        }
    }while (change);
}