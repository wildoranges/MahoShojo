#include<CodeGen.h>
#include<RegAlloc.h>
#include<queue>
#include<stack>
#include<algorithm>
#include<sstream>
#include<string>

// namespace CodeGen{

    std::string CodeGen::global(std::string name){
        return IR2asm::space + ".globl " + ".." + name + IR2asm::endl;
    }

    std::string CodeGen::tmp_reg_restore(Instruction *inst) {
        std::string code;
        for(auto pair:tmp_regs_loc){
            code += IR2asm::safe_load(new IR2asm::Reg(pair.first),
                                      pair.second,
                                      sp_extra_ofst,
                                      long_func);
        }
        tmp_regs_loc.clear();
        cur_tmp_regs.clear();
        free_tmp_pos.clear();
        free_tmp_pos = all_free_tmp_pos;
        return code;
    }

    std::string CodeGen::push_tmp_instr_regs(Instruction *inst) {
        std::string code;
        store_list.clear();
        to_store_set.clear();
        interval_set.clear();
        std::set<Value*> to_ld_set = {};
        std::set<int> used_tmp_regs = {};
        std::set<int> inst_reg_num_set = {};
        std::set<int> opr_reg_num_set = {};
        bool can_use_inst_reg = false;
        for (auto opr : inst->get_operands()) {
            if(dynamic_cast<Constant*>(opr) ||
            dynamic_cast<BasicBlock *>(opr) ||
            dynamic_cast<GlobalVariable *>(opr) ||
            dynamic_cast<AllocaInst *>(opr)){
                continue;
            }
            if (reg_map[opr]->reg_num >= 0) {
                inst_reg_num_set.insert(reg_map[opr]->reg_num);
                opr_reg_num_set.insert(reg_map[opr]->reg_num);
            }
        }
        if(!inst->is_void() && !dynamic_cast<AllocaInst *>(inst)){
            auto reg_inter = reg_map[inst];
            if(reg_inter->reg_num<0){
                if(!cur_tmp_regs.empty()){
                    reg_inter->reg_num = *cur_tmp_regs.begin();
                    cur_tmp_regs.erase(reg_inter->reg_num);
                    used_tmp_regs.insert(reg_inter->reg_num);
                }
                else{
                    if(!opr_reg_num_set.empty()){
                        for(int i = 0;i <= 12;i++){
                            if(i == 11){
                                continue;
                            }
                            if(opr_reg_num_set.find(i) == opr_reg_num_set.end()){
                                reg_inter->reg_num = i;
                                auto it = std::find(store_list.begin(),store_list.end(),i);
                                if(it==store_list.end()){
                                    store_list.push_back(i);
                                }
                                break;
                            }
                        }
                    }
                    else{
                        reg_inter->reg_num = store_list.size();
                        auto it = std::find(store_list.begin(),store_list.end(),reg_inter->reg_num);
                        if(it==store_list.end()){
                            store_list.push_back(reg_inter->reg_num);
                        }
                    }
                }
                interval_set.insert(reg_inter);
                to_store_set.insert(inst);
                can_use_inst_reg = true;
            }
            inst_reg_num_set.insert(reg_inter->reg_num);
        }
        for(auto opr:inst->get_operands()){
            if(dynamic_cast<Constant*>(opr) ||
            dynamic_cast<BasicBlock *>(opr) ||
            dynamic_cast<GlobalVariable *>(opr) ||
            dynamic_cast<AllocaInst *>(opr)){
                continue;
            }
            auto reg_inter = reg_map[opr];
            if(reg_inter->reg_num<0){
                if(can_use_inst_reg){
                    can_use_inst_reg = false;
                    if(opr_reg_num_set.find(reg_map[inst]->reg_num)==opr_reg_num_set.end()){
                        reg_inter->reg_num = reg_map[inst]->reg_num;
                        inst_reg_num_set.insert(reg_inter->reg_num);
                    }
                    else{
                        for(int i = 0;i <= 12;i++){
                            if(i==11){
                                continue;
                            }
                            if(inst_reg_num_set.find(i)==inst_reg_num_set.end()&&used_tmp_regs.find(i)==used_tmp_regs.end()){
                                reg_inter->reg_num = i;
                                inst_reg_num_set.insert(i);
                                auto it = std::find(store_list.begin(),store_list.end(),i);
                                if(it==store_list.end()){
                                    store_list.push_back(i);
                                }
                                break;
                            }
                        }
                    }
                }
                else{
                    if(!cur_tmp_regs.empty()){
                        reg_inter->reg_num = *cur_tmp_regs.begin();
                        cur_tmp_regs.erase(reg_inter->reg_num);
                        used_tmp_regs.insert(reg_inter->reg_num);
                        inst_reg_num_set.insert(reg_inter->reg_num);
                    }
                    else{
                        for(int i = 0;i <= 12;i++){
                            if(i==11){
                                continue;
                            }
                            if(inst_reg_num_set.find(i)==inst_reg_num_set.end()&&used_tmp_regs.find(i)==used_tmp_regs.end()){
                                reg_inter->reg_num = i;
                                inst_reg_num_set.insert(i);
                                auto it = std::find(store_list.begin(),store_list.end(),i);
                                if(it==store_list.end()){
                                    store_list.push_back(i);
                                }
                                break;
                            }
                        }
                    }
                }
                interval_set.insert(reg_inter);
                to_ld_set.insert(opr);
            }
        }
        int tmp_reg_base = have_func_call?20:0;
        for(auto reg_id:store_list){
            if(free_tmp_pos.empty()){
                std::cerr << "free pos not empty, something was wrong with inst: "<< inst->print() << std::endl;
                exit(15);
            }
            int free_pos = *free_tmp_pos.begin();
            free_tmp_pos.erase(free_pos);
            int cur_ofset = free_pos;//FIXME:offset
            auto loc = new IR2asm::Regbase(IR2asm::Reg(13),cur_ofset*4+tmp_reg_base);
            tmp_regs_loc[reg_id] = loc;
            code += IR2asm::safe_store(new IR2asm::Reg(reg_id),
                                       loc,
                                       sp_extra_ofst,
                                       long_func);
            cur_tmp_regs.insert(reg_id);
        }
        for(auto tmp_reg:used_tmp_regs){
            cur_tmp_regs.insert(tmp_reg);
        }
        for(auto opr:to_ld_set){
            code += IR2asm::safe_load(new IR2asm::Reg(reg_map[opr]->reg_num),
                                          stack_map[opr],
                                          sp_extra_ofst,
                                          long_func);
        }
        return code;
    }


    std::string CodeGen::pop_tmp_instr_regs(Instruction *inst) {
        std::string code;
        for(auto opr:to_store_set){
            code += IR2asm::safe_store(new IR2asm::Reg(reg_map[opr]->reg_num),
                                           stack_map[opr],
                                           sp_extra_ofst,
                                           long_func);
        }

        for(auto inter:interval_set){
            inter->reg_num = -1;
        }
        to_store_set.clear();
        store_list.clear();
        interval_set.clear();
        return code;
    }

    bool CodeGen::iszeroinit(Constant * init){
        if(dynamic_cast<ConstantInt *>(init)){
            return (dynamic_cast<ConstantInt *>(init)->get_value() == 0);
        }
        else{
            auto initalizer = dynamic_cast<ConstantArray *>(init);
            int init_size = initalizer->get_size_of_array();
            for(int i = 0; i < init_size; i++){
                Constant* init_iter = initalizer->get_element_value(i);
                if(!iszeroinit(init_iter))return false;
            }
        }
        return true;
    }

    std::string CodeGen::ld_tmp_regs(Instruction *inst) {
        std::string code;
        if(!inst->is_alloca() && !inst->is_phi()){
            std::set<int> to_del_set;
            std::set<int> to_ld_set;
            for(auto opr:inst->get_operands()){
                if(dynamic_cast<Constant *>(opr) ||
                dynamic_cast<BasicBlock *>(opr) ||
                dynamic_cast<GlobalVariable *>(opr) ||
                dynamic_cast<AllocaInst *>(opr) ||
                dynamic_cast<Function* >(opr)){
                    continue;
                }
                int opr_reg = reg_map[opr]->reg_num;
                if(opr_reg >= 0){
                    for(auto reg:cur_tmp_regs){
                        if(reg==opr_reg){
                            to_ld_set.insert(reg);
                            to_del_set.insert(reg);
                        }
                    }
                }
            }
            if(!inst->is_void()){
                int inst_reg = reg_map[inst]->reg_num;
                if(inst_reg >= 0){
                    if(cur_tmp_regs.find(inst_reg)!=cur_tmp_regs.end()){
                        to_del_set.insert(inst_reg);
                    }
                }
            }
            for(auto ld_reg:to_ld_set){
                code += IR2asm::safe_load(new IR2asm::Reg(ld_reg),
                                          tmp_regs_loc[ld_reg],
                                          sp_extra_ofst,
                                          long_func);
            }
            for(auto del_reg:to_del_set){
                auto del_pos = tmp_regs_loc[del_reg];
                int del_free_pos = del_pos->get_offset();
                if(have_func_call){
                    del_free_pos -= 20;
                }
                del_free_pos /= 4;
                free_tmp_pos.insert(del_free_pos);
                cur_tmp_regs.erase(del_reg);
                tmp_regs_loc.erase(del_reg);//FIXME:CUR_OFFSET
            }
        }
        return code;
    }

    std::string CodeGen::global_def_gen(Module* module){
        std::string code;
        for(auto var: module->get_global_variable()){
            std::string name = var->get_name();
            bool isconst = var->is_const();
            auto initializer = var->get_init();
            bool isinitialized = (dynamic_cast<ConstantZero *>(initializer) == nullptr);
            bool isarray = (dynamic_cast<ConstantArray *>(initializer) != nullptr);
            int size = var->get_type()->get_size();
            code += IR2asm::space;
            code += ".type .." + name + ", %object" + IR2asm::endl;
            code += IR2asm::space;
            if(isinitialized){  //initialized global var
                bool iszeroinit_ = iszeroinit(initializer);
                if(isconst){
                    code += ".section .rodata,\"a\", %progbits" + IR2asm::endl;
                }
                else{
                    if(iszeroinit_){
                        code += ".bss" + IR2asm::endl;
                    }
                    else{
                        code += ".data" + IR2asm::endl;
                    }
                }
                code += global(name);
                code += IR2asm::space;
                code += ".p2align " + std::to_string(int_p2align) + IR2asm::endl;
                code += ".." + name + ":" + IR2asm::endl;
                code += IR2asm::space;
                if(!isarray){
                    code += ".long ";
                    code += std::to_string(dynamic_cast<ConstantInt *>(initializer)->get_value());
                    code += IR2asm::endl;
                    code += IR2asm::space;
                }
                else{
                    if(iszeroinit_){
                        code += ".zero ";
                        code += std::to_string(size);
                        code += IR2asm::endl;
                        code += IR2asm::space;
                    }
                    else{
                        auto initalizer_ = dynamic_cast<ConstantArray *>(initializer);
                        int init_size = initalizer_->get_size_of_array();
                        for(int i = 0; i < init_size; i++){
                            Constant* init_iter = initalizer_->get_element_value(i);
                            code += ".long ";
                            code += std::to_string(dynamic_cast<ConstantInt *>(init_iter)->get_value());
                            code += IR2asm::endl;
                            code += IR2asm::space;
                        }
                    }
                }
                code += ".size ";
                code += ".." + name + ", ";
                code += std::to_string(size);
                code += IR2asm::endl;
            }
            else{   //uninitialized global var
                code += ".comm ..";
                code += name;
                code += ", ";
                code += std::to_string(size);
                code += ", " + std::to_string(int_align);  //int align 4
                code += IR2asm::endl;
            }
            code += IR2asm::endl;
        }
        return code;
    }

    int CodeGen::stack_space_allocation(Function* fun){
        int size = 0;
        int arg_size = 0;
        used_reg.second.clear();
        used_reg.first.clear();
        stack_map.clear();
        arg_on_stack.clear();
        reg2val.clear();
        std::vector<Value*> stack_args;
        //arg on stack in reversed sequence
        if(fun->get_num_of_args() > 4){
            for(auto arg: fun->get_args()){
                if(arg->get_arg_no() < 4)continue;
                int type_size = arg->get_type()->get_size();
                arg_on_stack.push_back(new IR2asm::Regbase(IR2asm::sp, arg_size));
                arg_size += ((type_size + 3) / 4) * 4;
                stack_args.push_back(dynamic_cast<Value*>(arg));
            }
        }
        for(auto iter: reg_map){
            Value* vreg = iter.first;
            Interval* interval = iter.second;
            if(interval->reg_num >= 0){
                if(interval->reg_num > 3){
                    used_reg.second.insert(interval->reg_num);
                }
                else{
                    used_reg.first.insert(interval->reg_num);
                }
                if(reg2val.find(interval->reg_num)!=reg2val.end()){
                    reg2val.find(interval->reg_num)->second.push_back(vreg);
                }
                else{
                    reg2val.insert({interval->reg_num, {vreg}});
                }
                continue;
            }
            if(dynamic_cast<Argument*>(vreg)){
                auto arg = dynamic_cast<Argument*>(vreg);
                if(arg->get_arg_no() > 3){
                    // stack_map.insert({vreg, arg_on_stack[arg->get_arg_no() - 4]});
                    continue;
                }
            }
            int type_size = vreg->get_type()->get_size();
            size += ((type_size + 3) / 4) * 4;
            if(have_func_call){
                stack_map.insert({vreg, new IR2asm::Regbase(IR2asm::frame_ptr, -size)});
            }
            else{
                stack_map.insert({vreg, new IR2asm::Regbase(IR2asm::sp, -size)});
            }
        }
        if(have_func_call)used_reg.second.insert(IR2asm::frame_ptr);
        for(auto inst: fun->get_entry_block()->get_instructions()){
            auto alloc = dynamic_cast<AllocaInst*>(inst);
            if(!alloc)continue;
            int type_size = alloc->get_alloca_type()->get_size();
            size += ((type_size + 3) / 4) * 4;
            if(have_func_call){
                stack_map.insert({dynamic_cast<Value *>(alloc), new IR2asm::Regbase(IR2asm::frame_ptr, -size)});
            }
            else{
                stack_map.insert({dynamic_cast<Value *>(alloc), new IR2asm::Regbase(IR2asm::sp, -size)});
            }
        }
        size += ((have_temp_reg)?(temp_reg_store_num * reg_size):0) 
                + ((have_func_call)?(caller_saved_reg_num * reg_size):0);
        if(!have_func_call){
            for(auto map: stack_map){
                int offset = map.second->get_offset();
                map.second->set_offset(size + offset);
                                        // + ((have_temp_reg)?(temp_reg_store_num * reg_size):0) 
                                        // + ((have_func_call)?20:0));
            }
        }
        /******always save lr for tmp use*******/
        // int reg_store_size = reg_size * (used_reg.second.size() + ((have_func_call)? 1 : 0) );
        int reg_store_size = reg_size * (used_reg.second.size() + 1);
        int i = 0;
        for(auto item: arg_on_stack){
            int offset = item->get_offset();
            item->set_offset(offset + reg_store_size + size);
                            // + ((have_temp_reg)?(temp_reg_store_num * reg_size):0) 
                            // + ((have_func_call)?20:0));
            stack_map.insert({stack_args[i], item});
            i++;
        }
        return size;
    }

    std::string CodeGen::callee_reg_store(Function* fun){
        std::string code;
        if(used_reg.second.empty())return IR2asm::space + "push {lr}" + IR2asm::endl;
        code += IR2asm::space;
        code += "push {";
        for(auto reg: used_reg.second){
            if(reg <= max_func_reg)continue;
            code += IR2asm::reg_name[reg];
            if(reg == *used_reg.second.rbegin())break;
            code += ", ";
        }
        /******always save lr for temporary use********/
        code += ", lr}";
        code += IR2asm::endl;
        return code;
    }

    std::string CodeGen::callee_reg_restore(Function* fun){
        std::string code;
        if(!used_reg.second.size())return IR2asm::space + "pop {lr}" + IR2asm::endl;
        code += IR2asm::space;
        code += "pop {";
        for(auto reg: used_reg.second){
            if(reg <= max_func_reg)continue;
            code += IR2asm::reg_name[reg];
            if(reg == *used_reg.second.rbegin())break;
            code += ", ";
        }
        /******always save lr for temporary use********/
        code += ", lr}";
        code += IR2asm::endl;
        return code;
    }

    std::string CodeGen::callee_stack_operation_in(Function* fun, int stack_size){
        int remain_stack_size = stack_size;
        std::string code;
        code += IR2asm::space;
        if(have_func_call){
            code += "mov ";
            code += IR2asm::Reg(IR2asm::frame_ptr).get_code();
            code += ", sp";
            // code += std::to_string(2 * int_size);
            code += IR2asm::endl;
            code += IR2asm::space;
        }
        if(remain_stack_size <= 127 && remain_stack_size > -128){
            code += "sub sp, sp, #";
            code += std::to_string(remain_stack_size);
            code += IR2asm::endl;
        }
        else{
            code += "ldr lr, =";
            code += std::to_string(remain_stack_size);
            code += IR2asm::endl;
            code += IR2asm::space;
            code += "sub sp, sp, lr";
            code += IR2asm::endl;
        }
        return code;
    }

    std::string CodeGen::callee_stack_operation_out(Function* fun, int stack_size){
        std::string code;
        code += IR2asm::space;
        if(have_func_call){
            code += "mov sp, ";
            code += IR2asm::Reg(IR2asm::frame_ptr).get_code();
            // code += std::to_string(2 * int_size);
            code += IR2asm::endl;
            return code;
        }
        int remain_stack_size = stack_size;
        if(remain_stack_size <= 127 && remain_stack_size > -128){
            code += "add sp, sp, #";
            code += std::to_string(remain_stack_size);
            code += IR2asm::endl;
        }
        else{
            code += "ldr lr, =";
            code += std::to_string(remain_stack_size);
            code += IR2asm::endl;
            code += IR2asm::space;
            code += "add sp, sp, lr";
            code += IR2asm::endl;
        }
        return code;
    }

    std::string CodeGen::caller_reg_store(Function* fun,CallInst* call){
        std::string code;
        caller_saved_pos.clear();
        to_save_reg.clear();
        int arg_num = 4;
        if(!used_reg.first.empty()){
            for(int i = 0;i < arg_num;i++){
                if(used_reg.first.find(i) != used_reg.first.end()){
                    bool not_to_save = true;
                    for(auto val:reg2val[i]){
                        not_to_save = not_to_save && !reg_map[val]->covers(call);
                    }
                    if(!not_to_save){
                        caller_saved_pos[i] = to_save_reg.size() * 4;
                        to_save_reg.push_back(i);
                    }
                }
            }
        }
        //TODO:debug caller save reg
        if(used_reg.second.find(12) != used_reg.second.end()){
            bool not_to_save = true;
            for(auto val:reg2val[12]){
                not_to_save = not_to_save && !reg_map[val]->covers(call);
            }
            if(!not_to_save){
                caller_saved_pos[12] = to_save_reg.size() * 4;
                to_save_reg.push_back(12);
            }
        }
        if(!to_save_reg.empty()){
            code += IR2asm::space;
            code += "STM SP, {";
            int save_size = to_save_reg.size();
            for(int i = 0; i < save_size - 1; i++){
                code += IR2asm::Reg(to_save_reg[i]).get_code();
                code += ", ";
            }
            code += IR2asm::Reg(to_save_reg[save_size-1]).get_code();
            code += "}";
            code += IR2asm::endl;
            //code += push_regs(to_save_reg);
        }
        return code;
    }

    std::string CodeGen::caller_reg_restore(Function* fun, CallInst* call){
        std::string code = "";
        int arg_num = fun->get_num_of_args();
        if(func_param_extra_offset>0){
            code += IR2asm::space;
            code += "ADD sp, sp, #";
            code += std::to_string(func_param_extra_offset*4);
            code += IR2asm::endl;
            sp_extra_ofst -= func_param_extra_offset*4;
        }
        if(call->is_void()){
            if(!to_save_reg.empty()){
                code += IR2asm::space;
                code += "LDM sp, {";
                int pop_size = to_save_reg.size()-1;
                for(int i=0;i<pop_size;i++){
                    code += IR2asm::Reg(to_save_reg[i]).get_code();
                    code += ", ";
                }
                code += IR2asm::Reg(to_save_reg[pop_size]).get_code();
                code += "}";
                code += IR2asm::endl;
                //sp_extra_ofst -= to_save_reg.size() * 4;
            }
            return code;
        }

        else{
            int ret_id = reg_map[call]->reg_num;
            int pop_size = caller_saved_pos.size();
            int init_id = 0;

            if(caller_saved_pos.find(0)!=caller_saved_pos.end()){
                init_id = 1;
                if((pop_size - init_id)> 0){
                    code += IR2asm::space;
                    code += "LDMIB SP, {";
                    for(int i=init_id;i<pop_size-1;i++){
                        code += IR2asm::Reg(to_save_reg[i]).get_code();
                        code += ", ";
                    }
                    code += IR2asm::Reg(to_save_reg[pop_size-1]).get_code();
                    code += "}";
                    code += IR2asm::endl;
                }
            }

            else{
                if(!to_save_reg.empty()){
                    code += IR2asm::space;
                    code += "LDM SP, {";
                    for(int i=0;i<pop_size-1;i++){
                        code += IR2asm::Reg(to_save_reg[i]).get_code();
                        code += ", ";
                    }
                    code += IR2asm::Reg(to_save_reg[pop_size-1]).get_code();
                    code += "}";
                    code += IR2asm::endl;
                }
            }
            if(ret_id!=0){
                if(ret_id > 0){
                    code += IR2asm::space;
                    code += "mov " + IR2asm::Reg(ret_id).get_code();
                    code += ", ";
                    code += IR2asm::Reg(0).get_code();
                    code += IR2asm::endl;
                }else{
                    code += IR2asm::space;
                    code += "str ";
                    code += IR2asm::Reg(0).get_code();
                    code += ", ";
                    code += stack_map[call]->get_ofst_code(sp_extra_ofst);
                    code += IR2asm::endl;
                }
            }

            if(init_id && ret_id != 0){
                code += IR2asm::space;
                code += "ldr r0, [SP]";
                code += IR2asm::endl;
            }
        }
        return code;
    }

    void CodeGen::make_global_table(Module* module){
        for(auto var: module->get_global_variable()){
            for(auto use: var->get_use_list()){
                Function* func_;
                func_ = dynamic_cast<Instruction *>(use.val_)->get_parent()->get_parent();
                // std::cout << func_->get_name() << ":" << var->get_name() << "\n";
                if(global_variable_use.find(func_) != global_variable_use.end()){
                    global_variable_use.find(func_)->second.insert(var);
                }
                else{
                    global_variable_use.insert({func_, {var}});
                }
            }
        }
    }
//                        }
    
    std::string CodeGen::print_global_table(){
        std::string code;
        for(auto iter: global_variable_table){
            GlobalVariable* var = iter.first;
            IR2asm::label label = *iter.second;
            code += label.get_code();
            code += ":" + IR2asm::endl;
            code += IR2asm::space;
            code += ".long ..";
            code += var->get_name();
            code += IR2asm::endl;
        }
        return code;
    }

    std::string CodeGen::module_gen(Module* module){
        std::string code;
        std::string globaldef;
        globaldef += global_def_gen(module);
        // std::cout << code;
        auto driver = new RegAllocDriver(module);
        driver->compute_reg_alloc();
        make_global_table(module);
        func_no = 0;
        code += IR2asm::space + ".arch armv7ve " + IR2asm::endl;
        code += IR2asm::space + ".text " + IR2asm::endl;
        for(auto func_: module->get_functions()){
            if(func_->get_basic_blocks().empty())continue;
            reg_map = driver->get_reg_alloc_in_func(func_);
            code += function_gen(func_) + IR2asm::endl;
            func_no++;
        }
        //TODO: *other machine infomation
        return code + globaldef;
    }

    void CodeGen::make_linear_bb(Function* fun,RegAllocDriver* driver){
        //sort bb and make bb label, put in CodeGen::bb_label
        //label gen, name mangling as bbx_y for yth bb in function no.x .
        bb_label.clear();
        linear_bb.clear();
        bb_no = -1;
        std::list<BasicBlock *> linear_fun_bb;
        if(driver){
            linear_fun_bb = driver->get_bb_order_in_func(fun);
        }
        else{
            linear_fun_bb = fun->get_basic_blocks();
        }
        BasicBlock* ret_bb;
        IR2asm::label *newlabel;
        std::string label_str;
        for(auto bb: linear_fun_bb){
            if(bb != fun->get_entry_block() && !bb->get_terminator()->is_ret()){
                label_str = "bb" + std::to_string(func_no) + "_" + std::to_string(bb_no);
                newlabel = new IR2asm::label(label_str);
                bb_label.insert({bb, newlabel});
            }
            else if(bb == fun->get_entry_block() && bb->get_terminator()->is_ret()){
                bb_label.insert({bb, new IR2asm::label("")});
                linear_bb.push_back(bb);
                return;
            }
            else if(bb == fun->get_entry_block()){
                // bb_label.insert({bb, new IR2asm::label(fun->get_name())});
                bb_label.insert({bb, new IR2asm::label("")});
            }
            else{
                ret_bb = bb;
                continue;
            }
            linear_bb.push_back(bb);
            bb_no++;
        }
        label_str = "bb" + std::to_string(func_no) + "_" + std::to_string(bb_no);
        newlabel = new IR2asm::label(label_str);
        bb_label.insert({ret_bb, newlabel});
        linear_bb.push_back(ret_bb);
#ifdef DEBUG
        for(auto pair:bb_label){
            auto bb = pair.first;
            auto label = pair.second;
            std::cerr << bb->get_name() << ":" << label->get_label() << std::endl;
        }
#endif
        return;
    }

    void CodeGen::global_label_gen(Function* fun){
        //global varibal address store after program(.LCPIx_y), fill in CodeGen::global_variable_table
        if(global_variable_use.find(fun) == global_variable_use.end()){
            global_variable_table.clear();
            return;        
        }
        auto used_global = global_variable_use.find(fun)->second;
        global_variable_table.clear();
        label_no = 0;
        for(auto var: used_global){
            std::string label_str = "Addr" + std::to_string(func_no) + "_" + std::to_string(label_no);
            IR2asm::label* new_label = new IR2asm::label(label_str);
            label_no++;
            global_variable_table.insert({var, new_label});
        }
    }

    void CodeGen::func_call_check(Function* fun){
        max_arg_size = 0;
        have_func_call = false;
        int inst_count = 0;
        for(auto bb: fun->get_basic_blocks()){
            for(auto inst: bb->get_instructions()){
                //TODO: better evaluation instruction cost
                switch(inst->get_instr_type()){
                    case Instruction::OpID::call :{
                        inst_count += dynamic_cast<CallInst *>(inst)->get_num_operand() + 4;
                        break;
                    }
                    case Instruction::OpID::cmpbr :{
                        inst_count += 3;
                        break;
                    }
                    case Instruction::OpID::getelementptr :{
                        inst_count += 2;
                        break;
                    }
                    case Instruction::OpID::phi :{
                        inst_count += (dynamic_cast<PhiInst *>(inst)->get_num_operand() / 2) * bb->get_pre_basic_blocks().size();
                        break;
                    }
                    default: inst_count++;
                }
                auto call = dynamic_cast<CallInst*>(inst);
                if(!call)continue;
                int arg_size = 0;
                auto callee = dynamic_cast<Function *>(call->get_operand(0));
                for(auto arg: callee->get_args()){
                    arg_size += arg->get_type()->get_size();
                }
                if(arg_size > max_arg_size)max_arg_size = arg_size;
                have_func_call = true;
            }
        }
        if(inst_count > 500)long_func = true;
        else{long_func = false;}
        return;
    }

    std::string CodeGen::arg_move(CallInst* call){
        //arg on stack in reversed sequence
        std::string regcode;
        std::string memcode;
        std::stack<Value *> push_queue;//for sequence changing
        auto fun = dynamic_cast<Function *>(call->get_operand(0));
        int i = 0;
        int num_of_arg = call->get_num_operand()-1;
        if(num_of_arg>4){
            sp_extra_ofst += (call->get_num_operand() - 1 - 4) * reg_size;
        }
        for(auto arg: call->get_operands()){
            if(dynamic_cast<Function *>(arg))continue;
            if(i < 4){
                if(dynamic_cast<ConstantInt *>(arg)){
                    regcode += IR2asm::space;
                    regcode += "ldr ";
                    regcode += IR2asm::Reg(i).get_code();
                    regcode += ", =";
                    regcode += std::to_string(dynamic_cast<ConstantInt *>(arg)->get_value());
                    regcode += IR2asm::endl;
                    i++;
                    continue;
                }
                auto reg = (reg_map).find(arg)->second->reg_num;
                IR2asm::Reg* preg;
                if(reg >= 0){
                    if(reg<=3){
                        regcode += IR2asm::safe_load(new IR2asm::Reg(i), 
                                                new IR2asm::Regbase(IR2asm::Reg(IR2asm::sp),caller_saved_pos[reg]),
                                                sp_extra_ofst, long_func);
                        i++;
                        continue;
                    }
                    else if(reg!=12){
                        regcode += IR2asm::space;
                        regcode += "mov ";
                        regcode += IR2asm::Reg(i).get_code();
                        regcode += ", ";
                        regcode += IR2asm::Reg(reg).get_code();
                        regcode += IR2asm::endl;
                        i++;
                        continue;
                    }else{
                        regcode += IR2asm::safe_load(new IR2asm::Reg(i), 
                                                new IR2asm::Regbase(IR2asm::Reg(IR2asm::sp),caller_saved_pos[12]),
                                                sp_extra_ofst, long_func);
                        i++;
                        continue;
                    }
                }
                else{
                    regcode += IR2asm::safe_load(new IR2asm::Reg(i),
                                                 stack_map.find(arg)->second,
                                                 sp_extra_ofst,
                                                 long_func);
                }
            }
            else{
                push_queue.push(arg);
            }
            i++;
        }
        if(num_of_arg>4){
            sp_extra_ofst -= (call->get_num_operand() - 1 - 4) * reg_size;
        }
        std::vector<int> to_push_regs = {};
        const int tmp_reg_id[] = {12};
        const int tmp_reg_size = 1;
        int remained_off_reg_num = 1;
        func_param_extra_offset = 0;
        while(!push_queue.empty()){
            Value* arg = push_queue.top();
            push_queue.pop();
            func_param_extra_offset ++;
            if(dynamic_cast<ConstantInt *>(arg)){
                memcode += IR2asm::space;
                memcode += "ldr ";
                memcode += IR2asm::Reg(tmp_reg_id[tmp_reg_size-remained_off_reg_num]).get_code();
                memcode += " ,=";
                memcode += std::to_string(dynamic_cast<ConstantInt *>(arg)->get_value());
                memcode += IR2asm::endl;
                to_push_regs.push_back(tmp_reg_id[tmp_reg_size-remained_off_reg_num]);
                remained_off_reg_num--;
            }else{
                auto reg = (reg_map).find(arg)->second->reg_num;
                if(reg >= 0){
                    if(reg>=4&&reg<12){
                        to_push_regs.push_back(reg);
                        remained_off_reg_num--;
                    }else{
                        memcode += IR2asm::safe_load(new IR2asm::Reg(tmp_reg_id[tmp_reg_size-remained_off_reg_num]),
                                                     new IR2asm::Regbase(IR2asm::Reg(IR2asm::sp),caller_saved_pos[reg]),
                                                     sp_extra_ofst, 
                                                     long_func);
                        to_push_regs.push_back(tmp_reg_id[tmp_reg_size-remained_off_reg_num]);
                        remained_off_reg_num--;
                        //TODO:null ptr?segment fault?
                    }
                }
                else{
                    auto srcaddr = stack_map.find(arg)->second;
                    memcode += IR2asm::safe_load(new IR2asm::Reg(tmp_reg_id[tmp_reg_size-remained_off_reg_num]),
                                                 srcaddr,
                                                 sp_extra_ofst,
                                                 long_func);
                    to_push_regs.push_back(tmp_reg_id[tmp_reg_size-remained_off_reg_num]);
                    remained_off_reg_num--;
                }
            }
            if(remained_off_reg_num==0){
                memcode += push_regs(to_push_regs);
                to_push_regs.clear();
                remained_off_reg_num = tmp_reg_size;
            }
        }
        if(!to_push_regs.empty()){
            memcode += push_regs(to_push_regs);
        }
        return memcode + regcode;
    }

    std::string CodeGen::callee_arg_move(Function* fun){
        std::string save_code;
        std::string code;
        std::set<int> conflict_src_reg;
        std::map<int, IR2asm::Regbase*> conflict_reg_loc;
        int size = 0;
        int arg_num = fun->get_args().size();
        if(!arg_num)return code;
        if(arg_num > 4)arg_num = 4;
        for(auto arg: fun->get_args()){
            if(reg_map.find(arg) != reg_map.end()){
                int target = reg_map[arg]->reg_num;
                if(target == arg->get_arg_no())continue;
                if(target >= 0 && target < arg_num)conflict_src_reg.insert(target);
            }
        }
        if(!conflict_src_reg.empty()){
            save_code += IR2asm::space + "STMDB SP, {";
            for(auto reg: conflict_src_reg){
                if(reg == *(conflict_src_reg.rbegin()))break;
                save_code += IR2asm::Reg(reg).get_code();
                save_code += ", ";
            }
            save_code += IR2asm::Reg(*(conflict_src_reg.rbegin())).get_code() + "}" + IR2asm::endl;
        }
        int conflict_store_size = conflict_src_reg.size() * reg_size;
        for(auto reg: conflict_src_reg){
            conflict_reg_loc.insert({reg, new IR2asm::Regbase(IR2asm::sp, size - conflict_store_size)});
            size += reg_size;
        }
        for(auto arg: fun->get_args()){
            int reg;
            if(reg_map.find(arg) != reg_map.end()){
                reg = reg_map[arg]->reg_num;
            }
            else continue;
            if(arg->get_arg_no() < 4){
                if(arg->get_arg_no() == reg)continue;
                if(reg >= 0){
                    if(conflict_src_reg.find(arg->get_arg_no()) == conflict_src_reg.end()){
                        code += IR2asm::space;
                        code += "Mov ";
                        code += IR2asm::Reg(reg).get_code();
                        code += ", ";
                        code += IR2asm::Reg(arg->get_arg_no()).get_code();
                        code += IR2asm::endl;
                    }
                    else{
                        code += IR2asm::space;
                        code += "Ldr ";
                        code += IR2asm::Reg(reg).get_code();
                        code += ", ";
                        code += conflict_reg_loc[arg->get_arg_no()]->get_code();
                        code += IR2asm::endl;
                    }
                }
                else{
                    code += IR2asm::safe_store(new IR2asm::Reg(arg->get_arg_no()),
                                               stack_map[arg],
                                               sp_extra_ofst,
                                               long_func);
                }
            }
            else{
                if(reg < 0)continue;
                code += IR2asm::safe_load(new IR2asm::Reg(reg),
                                          arg_on_stack[arg->get_arg_no() - 4],
                                          sp_extra_ofst,
                                          long_func);
            }
        }
        return save_code + code;
    }

    std::string CodeGen::function_gen(Function* fun,RegAllocDriver* driver){
        std::string code;
        sp_extra_ofst = 0;
        pool_number = 0;
        cur_tmp_regs.clear();
        tmp_regs_loc.clear();
        free_tmp_pos.clear();
        free_tmp_pos = all_free_tmp_pos;
        global_label_gen(fun);
        make_linear_bb(fun, driver);
        func_call_check(fun);
        int stack_size = stack_space_allocation(fun);
                        // + ((have_temp_reg)?(temp_reg_store_num * reg_size):0) 
                        // + ((have_func_call)?20:0);
//                + std::max(max_arg_size - 4 * reg_size, 0);
        code += IR2asm::space + ".globl " + fun->get_name() + IR2asm::endl;
        code += IR2asm::space + ".p2align " + std::to_string(int_p2align) + IR2asm::endl;
        code += IR2asm::space + ".type " + fun->get_name() + ", %function" + IR2asm::endl;
        code += fun->get_name() + ":" + IR2asm::endl;
        code += callee_reg_store(fun);
        if(stack_size)code += callee_stack_operation_in(fun, stack_size);
        code += callee_arg_move(fun);

        for(auto bb: linear_bb){
            code += bb_gen(bb);
        }
        if(stack_size)code += callee_stack_operation_out(fun, stack_size);
        code += callee_reg_restore(fun);
        code += IR2asm::space + "bx lr" + IR2asm::endl;
        code += make_lit_pool(true);
        code += print_global_table();
        // std::cout << code << IR2asm::endl;
        return code;
    }

    std::string CodeGen::push_regs(std::vector<int> &reg_list, std::string cond) {
        std::string code;
        code += IR2asm::space;
        code += "push" + cond + " {";
        for(auto reg: reg_list){
            code += IR2asm::Reg(reg).get_code();
            if(reg != *reg_list.rbegin())code += ", ";
        }
        code += "}" + IR2asm::endl;
        sp_extra_ofst += reg_list.size() * reg_size;
        return code;
    }

    std::string CodeGen::pop_regs(std::vector<int> &reg_list, std::string cond) {
        std::string code;
        code += IR2asm::space;
        code += "pop" + cond + " {";
        for(auto reg: reg_list){
            code += IR2asm::Reg(reg).get_code();
            if(reg != *reg_list.rbegin())code += ", ";
        }
        code += "}" + IR2asm::endl;
        sp_extra_ofst -= reg_list.size() * reg_size;
        return code;
    }

    std::string CodeGen::make_lit_pool(bool have_br){
        if(have_br){
            return IR2asm::space + ".pool" + IR2asm::endl;
        }
        std::string code;
        IR2asm::label pool_label("litpool" + std::to_string(func_no) + "_" + std::to_string(pool_number), 0);
        pool_number++;
        code += IR2asm::space;
        code += "b " + pool_label.get_code();
        code += IR2asm::endl;
        code += IR2asm::space + ".pool" + IR2asm::endl;
        code += pool_label.get_code() + ":" + IR2asm::endl;
        return code;
    }

    std::string CodeGen::bb_gen(BasicBlock* bb){
        std::string code;
        if(bb_label[bb]->get_code() != ""){
            code += bb_label[bb]->get_code()+":"+IR2asm::endl;
        }
        Instruction* br_inst = nullptr;
        for(auto inst : bb->get_instructions()){
            std::string new_code;
            if(inst->isTerminator()){
                br_inst = inst;
                break;
            }
            new_code += ld_tmp_regs(inst);
            if(dynamic_cast<CallInst*>(inst)){
                auto call_inst = dynamic_cast<CallInst*>(inst);
                new_code += caller_reg_store(bb->get_parent(),call_inst);
                new_code += arg_move(call_inst);
                new_code += instr_gen(call_inst);
                new_code += caller_reg_restore(bb->get_parent(),call_inst);
                accumulate_line_num += std::count(new_code.begin(), new_code.end(), IR2asm::endl[0]);
                if(accumulate_line_num > literal_pool_threshold){
                    code += make_lit_pool();
                    accumulate_line_num = 0;
                }
                code += new_code;
            }else if(instr_may_need_push_stack(inst)){

                new_code += push_tmp_instr_regs(inst);

                new_code += instr_gen(inst);

                new_code += pop_tmp_instr_regs(inst);

                accumulate_line_num += std::count(new_code.begin(), new_code.end(), IR2asm::endl[0]);
                if(accumulate_line_num > literal_pool_threshold){
                    code += make_lit_pool();
                    accumulate_line_num = 0;
                }

                code += new_code;
            }else{
                new_code += instr_gen(inst);
                accumulate_line_num += std::count(new_code.begin(), new_code.end(), IR2asm::endl[0]);
                if(accumulate_line_num > literal_pool_threshold){
                    code += make_lit_pool();
                    accumulate_line_num = 0;
                }
                code += new_code;
            }
        }
        std::string new_code;
        new_code += ld_tmp_regs(br_inst);
        if(br_inst->is_cmpbr()){
            new_code += ld_tmp_regs(br_inst);
            new_code += push_tmp_instr_regs(br_inst);
            accumulate_line_num += std::count(new_code.begin(), new_code.end(), IR2asm::endl[0]);
            if(accumulate_line_num > literal_pool_threshold){
                code += make_lit_pool();
                accumulate_line_num = 0;
            }
            code += new_code;
        }
        code += phi_union(bb, br_inst);
        return code;
    }

    void spilt_str(const std::string& s, std::vector<std::string>& sv, const char delim = ' ') {
        sv.clear();
        std::istringstream iss(s);
        std::string temp;
        while (std::getline(iss, temp, delim)) {
            sv.emplace_back(std::move(temp));
        }
        return;
    }

    std::string CodeGen::single_data_move(IR2asm::Location* src_loc,
                                 IR2asm::Location* target_loc,
                                 IR2asm::Reg *reg_tmp,
                                 std::string cmpop){
        std::string code;
        if(dynamic_cast<IR2asm::RegLoc *>(src_loc)){
                    auto regloc = dynamic_cast<IR2asm::RegLoc *>(src_loc);
                    if(regloc->is_constant()){
                        if(dynamic_cast<IR2asm::RegLoc*>(target_loc)){
                            auto target_reg_loc = dynamic_cast<IR2asm::RegLoc*>(target_loc);
                            code += IR2asm::space;
                            code += "Ldr" + cmpop + " ";
                            code += target_reg_loc->get_code();
                            code += ", =";
                            code += std::to_string(regloc->get_constant());
                            code += IR2asm::endl;
                        }
                        else{
                            code += IR2asm::space;
                            code += "Ldr" + cmpop + " ";
                            code += reg_tmp->get_code();
                            code += ", =";
                            code += std::to_string(regloc->get_constant());
                            code += IR2asm::endl;
                            code += IR2asm::safe_store(reg_tmp, target_loc, sp_extra_ofst, long_func, cmpop);
                        }
                    }
                    else{
                        if(dynamic_cast<IR2asm::RegLoc*>(target_loc)){
                            auto target_reg_loc = dynamic_cast<IR2asm::RegLoc*>(target_loc);
                            code += IR2asm::space;
                            code += "Mov" + cmpop + " ";
                            code += target_reg_loc->get_code();
                            code += ", ";
                            code += regloc->get_code();
                            code += IR2asm::endl;
                        }
                        else{
                            code += IR2asm::safe_store(new IR2asm::Reg(regloc->get_reg_id()),
                                                        target_loc, sp_extra_ofst, long_func, cmpop);
                        }
                    }
                }
                else{
                    auto stackloc = dynamic_cast<IR2asm::Regbase *>(src_loc);
                    if(dynamic_cast<IR2asm::RegLoc*>(target_loc)){
                        auto target_reg_loc = dynamic_cast<IR2asm::RegLoc*>(target_loc);
                        code += IR2asm::safe_load(new IR2asm::Reg(target_reg_loc->get_reg_id()),
                                                    stackloc, sp_extra_ofst, long_func, cmpop);
                    }
                    else{
                        code += IR2asm::safe_load(reg_tmp, stackloc, sp_extra_ofst, long_func, cmpop);
                        code += IR2asm::safe_store(reg_tmp, target_loc, sp_extra_ofst, long_func, cmpop);
                    }
                }
        return code;
    }

    std::string CodeGen::data_move(std::vector<IR2asm::Location*> &src,
                                   std::vector<IR2asm::Location*> &dst,
                                   std::string cmpop){
        std::string code;
        std::map<IR2asm::Location*, IR2asm::Location*> pred_locs;
        std::map<IR2asm::Location*, std::set<IR2asm::Location*>> data_graph;
        std::map<int, IR2asm::Location *> reg2loc;
        std::queue<IR2asm::Location*> ready_queue;
        std::set<IR2asm::Location*> loops;
        std::set<IR2asm::Location*> loop_breaker;
        bool need_temp = false;
        bool need_restore_temp = true;
        bool temp_forwarding = false;
        int temp_reg = 12;
        int size = src.size();
        for(int i = 0; i < size; i++){
            IR2asm::Location* srcloc = src[i];
            IR2asm::Location* dstloc = dst[i];
            if(srcloc == dstloc)continue;
            if(dynamic_cast<IR2asm::Regbase*>(srcloc)
                && dynamic_cast<IR2asm::Regbase*>(dstloc))need_temp = true;
            if(dynamic_cast<IR2asm::RegLoc *>(srcloc) 
                && dynamic_cast<IR2asm::RegLoc *>(srcloc)->is_constant()
                && dynamic_cast<IR2asm::Regbase *>(dstloc))need_temp = true;

            IR2asm::RegLoc* srcreg = dynamic_cast<IR2asm::RegLoc *>(srcloc);
            IR2asm::RegLoc* dstreg = dynamic_cast<IR2asm::RegLoc *>(dstloc);            
            if(srcreg && !srcreg->is_constant()){
                if(reg2loc.find(srcreg->get_reg_id()) != reg2loc.end()){
                    srcloc = reg2loc[srcreg->get_reg_id()];
                }
                else{
                    reg2loc.insert({srcreg->get_reg_id(), srcloc});
                }
            }
            if(dstreg && !dstreg->is_constant()){
                if(dstreg->get_reg_id() == temp_reg)need_restore_temp = false;
                if(reg2loc.find(dstreg->get_reg_id()) != reg2loc.end()){
                    dstloc = reg2loc[dstreg->get_reg_id()];
                }
                else{
                    reg2loc.insert({dstreg->get_reg_id(), dstloc});
                }
            }

            pred_locs.insert({dstloc, srcloc});
            if(data_graph.find(srcloc) != data_graph.end()){
                data_graph.find(srcloc)->second.insert(dstloc);
            }
            else{
                data_graph.insert({srcloc, {dstloc}});
            }
            if(data_graph.find(dstloc) == data_graph.end()){
                data_graph.insert({dstloc, {}});
            }

            if(pred_locs.find(srcloc) != pred_locs.end()){
                auto iter = pred_locs[srcloc];
                bool find_circ = false;
                
                int time = 0;
                while(pred_locs.find(iter) != pred_locs.end()){
                    if(iter == srcloc){
                        find_circ = true;
                        break;
                    }
                    if(loops.find(iter) != loops.end()){
                        break;
                    }
                    if(time > 2 * size + 1) exit(99);
                    iter = pred_locs[iter];
                    time++;
                }

                if(find_circ){
                    need_temp = true;
                    loops.insert(srcloc);
                }
            }
        }

        //TODO: smarter temp forwarding strategy: no temp needed on the path end at temp reg
        if(reg2loc.find(temp_reg) != reg2loc.end()
            && pred_locs.find(reg2loc[temp_reg]) != pred_locs.end()
            && pred_locs.find(pred_locs[reg2loc[temp_reg]]) == pred_locs.end()){
            temp_forwarding = true;
        }

        auto temp_reg_loc = new IR2asm::Regbase(IR2asm::sp, 0);
        IR2asm::Location* temp_src = nullptr;
        if(need_temp){
            //TODO: not always meed store temp reg
            accumulate_line_num+=1;
            if(accumulate_line_num > literal_pool_threshold){
                code += make_lit_pool();
                accumulate_line_num = 0;
            }
            code += IR2asm::store(new IR2asm::Reg(temp_reg), temp_reg_loc, cmpop);
            IR2asm::Location* temp_reg_node = nullptr;
            if(reg2loc.find(temp_reg) != reg2loc.end()){
                temp_reg_node = reg2loc[temp_reg];
            }
            if(temp_reg_node){
                data_graph.insert({temp_reg_loc, {}});
                if(data_graph.find(temp_reg_node) != data_graph.end()){
                    auto succ_list = data_graph[temp_reg_node];
                    for(auto item: succ_list){
                        data_graph[temp_reg_loc].insert(item);
                    }
                    data_graph[temp_reg_node].clear();
                }
                for(auto succ: data_graph[temp_reg_loc]){
                    if(pred_locs.find(succ) != pred_locs.end()){
                        pred_locs[succ] = temp_reg_loc;
                    }
                }

                if(!temp_forwarding){//TODO: may break loop too
                    need_restore_temp = true;
                    if(pred_locs.find(temp_reg_node) != pred_locs.end()){
                        if(data_graph.find(pred_locs[temp_reg_node]) != data_graph.end()){
                            data_graph[pred_locs[temp_reg_node]].erase(temp_reg_node);
                            data_graph[pred_locs[temp_reg_node]].insert(temp_reg_loc);
                        }
                        else{
                            data_graph[pred_locs[temp_reg_node]].insert(temp_reg_loc);
                        }
                        pred_locs.insert({temp_reg_loc, pred_locs[temp_reg_node]});
                        pred_locs.erase(temp_reg_node);
                    }
                }
                else{
                    if(pred_locs.find(temp_reg_node) != pred_locs.end()){
                        temp_src = pred_locs[temp_reg_node];
                        if(data_graph.find(temp_src) != data_graph.end()){
                            data_graph[temp_src].erase(temp_reg_node);
                        }
                        pred_locs.erase(temp_reg_node);
                    }
                }
            }
        }

        auto reg_tmp = new IR2asm::Reg(temp_reg);

        int time = 0;
        int edge_num = pred_locs.size();
        while(!pred_locs.empty()){
            time++;
            if(time > edge_num + 1)exit(101);
            bool changed = false;
            std::set<IR2asm::Location*> delete_list;
            for(auto map: pred_locs){
                auto target_loc = map.first;
                auto src_loc = map.second;
                if(data_graph.find(target_loc) != data_graph.end() 
                    && !data_graph[target_loc].empty())continue;

                accumulate_line_num+=4;
                if(accumulate_line_num > literal_pool_threshold){
                    code += make_lit_pool();
                    accumulate_line_num = 0;
                }
                
                code += single_data_move(src_loc, target_loc, reg_tmp, cmpop);

                changed = true;
                delete_list.insert(target_loc);
                data_graph[src_loc].erase(target_loc);
            }

            for(auto item: delete_list){
                pred_locs.erase(item);
            }

            if(!changed){
                auto target_loc = pred_locs.begin()->first;
                auto src_loc = pred_locs.begin()->second;
                auto temp_loc = new IR2asm::RegLoc(temp_reg);
                accumulate_line_num+=4;
                if(accumulate_line_num > literal_pool_threshold){
                    code += make_lit_pool();
                    accumulate_line_num = 0;
                }
                code += single_data_move(src_loc, temp_loc, reg_tmp, cmpop);
                pred_locs[target_loc] = temp_loc;
                data_graph.insert({temp_loc, {target_loc}});
                if(data_graph.find(src_loc) != data_graph.end())data_graph[src_loc].erase(target_loc);
            }
        }

        if(temp_forwarding && temp_src){
            accumulate_line_num+=4;
            if(accumulate_line_num > literal_pool_threshold){
                code += make_lit_pool();
                accumulate_line_num = 0;
            }
            code += single_data_move(temp_src, new IR2asm::RegLoc(reg_tmp->get_id()), reg_tmp, cmpop);
        }        

        if(need_temp && need_restore_temp && !temp_forwarding){
            accumulate_line_num+=3;
            if(accumulate_line_num > literal_pool_threshold){
                code += make_lit_pool();
                accumulate_line_num = 0;
            }
            code += IR2asm::safe_load(reg_tmp, temp_reg_loc, sp_extra_ofst, long_func, cmpop);
        }

        return code;
    }

    std::string CodeGen::phi_union(BasicBlock* bb, Instruction* br_inst){
        if(dynamic_cast<ReturnInst *>(br_inst)){
            std::string code;
            accumulate_line_num += 1;
            if(accumulate_line_num > literal_pool_threshold){
                code += make_lit_pool();
                accumulate_line_num = 0;
            }
            return code + instr_gen(br_inst);
        }
        std::string cmp;
        std::string inst_cmpop;
        std::string succ_code;
        std::string fail_code;
        std::string succ_br;
        std::string fail_br;
        std::string* code = &succ_code;
        bool is_succ = true;
        bool is_cmpbr = false;
        CmpBrInst* cmpbr = dynamic_cast<CmpBrInst*>(br_inst);
        BasicBlock* succ_bb;
        BasicBlock* fail_bb;

        //std::map<Value*,std::set<Value*>> opr2phi;
        //TODO:PHI INST CHECK
        //std::set<Value*> sux_bb_phi = {};
        std::vector<std::string> cmpbr_inst;
        std::string cmpbr_code = instr_gen(br_inst);
        std::string pop_code;
        pop_code += pop_tmp_instr_regs(br_inst);
        pop_code += tmp_reg_restore(br_inst);
        spilt_str(cmpbr_code, cmpbr_inst, IR2asm::endl[0]);
        std::vector<IR2asm::Location*> phi_target;
        std::vector<IR2asm::Location*> phi_src;

        if(cmpbr){
            is_cmpbr = true;
            succ_bb = dynamic_cast<BasicBlock*>(cmpbr->get_operand(2));
            fail_bb = dynamic_cast<BasicBlock*>(cmpbr->get_operand(3));
            cmp += cmpbr_inst[0] + IR2asm::endl;
            succ_br += cmpbr_inst[1] + IR2asm::endl;
            inst_cmpop += std::string(1, succ_br[5]);
            inst_cmpop.push_back(succ_br[6]); //bad for debugging
            fail_br += cmpbr_inst[2] + IR2asm::endl;
        }
        else{
            succ_bb = dynamic_cast<BasicBlock*>(br_inst->get_operand(0));
            succ_br += cmpbr_inst[0] + IR2asm::endl;
        }
        std::string lit_pool;
        accumulate_line_num += 1 + count(pop_code.begin(), pop_code.end(), IR2asm::endl[0]);//for possible cmp and pop code
        if(accumulate_line_num > literal_pool_threshold){
            lit_pool += make_lit_pool();
            accumulate_line_num = 0;
        }
        for(auto sux:bb->get_succ_basic_blocks()){
            std::string cmpop;
            if(sux == succ_bb){
                code = &succ_code;
                cmpop = inst_cmpop;
            }
            else{
                code = &fail_code;
                cmpop = "";
            }
            //sux_bb_phi.clear();
            //opr2phi.clear();
            phi_src.clear();
            phi_target.clear();
            bool src_reg = false;
            bool src_stack = false;
            bool src_const = false;
            bool target_reg = false;
            bool target_stack = false;
            for(auto inst:sux->get_instructions()){
                if(inst->is_phi()){
                    Value* lst_val = nullptr;
                    int target_pos = reg_map[inst]->reg_num;
                    IR2asm::Location* target_ptr = nullptr;
                    if(target_pos>=0){
                        target_ptr = new IR2asm::RegLoc(target_pos, false);
                    }else{
                        target_ptr = stack_map[inst];
                    }
                    for(auto opr:inst->get_operands()){
                        if(dynamic_cast<BasicBlock*>(opr)){
                            auto this_bb = dynamic_cast<BasicBlock*>(opr);
                            if(this_bb==bb){
                                if(target_pos>=0){
                                    target_reg = true;
                                }else{//TODO:CHECK ASSIGN IN IF?
                                    target_stack = true;
                                }
                                if(dynamic_cast<ConstantInt*>(lst_val)){
                                    auto const_val = dynamic_cast<ConstantInt*>(lst_val);
                                    auto src = new IR2asm::RegLoc(const_val->get_value(), true);
                                    src_const = true;
                                    phi_src.push_back(src);
                                    phi_target.push_back(target_ptr);
                                }else{
                                    int src_pos = reg_map[lst_val]->reg_num;
                                    if(src_pos>=0){
                                        if(src_pos!=target_pos){
                                            auto src = new IR2asm::RegLoc(src_pos, false);
                                            phi_src.push_back(src);
                                            phi_target.push_back(target_ptr);
                                            src_reg = true;
                                        }
                                    }else{
                                        auto src = stack_map[lst_val];
                                        phi_src.push_back(src);
                                        phi_target.push_back(target_ptr);
                                        src_stack = true;
                                    }
                                }
                            }
                        }else{
                            lst_val = opr;
                        }
                    }
                }else{
                    break;
                }
            }
            if(phi_src.empty()){
                continue;
            }
            *code += data_move(phi_src, phi_target, cmpop);
        }
        accumulate_line_num += std::count(succ_br.begin(), succ_br.end(), IR2asm::endl[0]);
        accumulate_line_num += std::count(fail_br.begin(), fail_br.end(), IR2asm::endl[0]);
        std::string ret_code = cmp + pop_code + lit_pool + succ_code + succ_br + fail_code + fail_br;
        if(accumulate_line_num > literal_pool_threshold){
            if(dynamic_cast<BranchInst *>(bb->get_terminator()) && bb->get_terminator()->get_num_operand() == 1){
                ret_code += make_lit_pool(true);
            }
            else{
                ret_code += make_lit_pool();
            }
            accumulate_line_num = 0;
        }
        return ret_code;
    }


    std::string CodeGen::instr_gen(Instruction * inst){
        std::string code;
        // call functions in IR2asm , deal with phi inst(mov inst)
        // may have many bugs
        auto instr_type = inst->get_instr_type();
        switch (instr_type)
        {
            case Instruction::ret:
                if (inst->get_operands().empty()) {
                    //code += IR2asm::ret();
                } else {
                    auto ret_val = inst->get_operand(0);
                    auto const_ret_val = dynamic_cast<ConstantInt*>(ret_val);
                    if (!const_ret_val&&get_asm_reg(ret_val)->get_id() == 0) {
//                        code += IR2asm::ret();
                    } else {
                        if (const_ret_val) {
                            code += IR2asm::ret(get_asm_const(const_ret_val));
                        } else {
                            if (get_asm_reg(ret_val)->get_id() < 0) {
                                code += IR2asm::safe_load(new IR2asm::Reg(0), stack_map[ret_val], sp_extra_ofst, long_func);
                            } else {
                                code += IR2asm::ret(get_asm_reg(ret_val));
                            }
                        }
                    }
                }
                break;
            case Instruction::br:
                if (inst->get_num_operand() == 1) {
                    //FIXME: too far ???
                    code += IR2asm::b(bb_label[dynamic_cast<BasicBlock*>(inst->get_operand(0))]);
                }
                break;
            case Instruction::add: {
                    auto op1 = inst->get_operand(0);
                    auto op2 = inst->get_operand(1);
                    auto const_op1 = dynamic_cast<ConstantInt*>(op1);
                    auto const_op2 = dynamic_cast<ConstantInt*>(op2);
                    Value *operand1;
                    IR2asm::Operand2 *operand2;
                    if (const_op1) {
                        operand1 = op2;
                        operand2 = new IR2asm::Operand2(const_op1->get_value());
                    } else {
                        operand1 = op1;
                        if (const_op2) {
                            operand2 = new IR2asm::Operand2(const_op2->get_value());
                        } else {
                            operand2 = new IR2asm::Operand2(*get_asm_reg(op2));
                        }
                    }
                    code += IR2asm::add(get_asm_reg(inst), get_asm_reg(operand1), operand2);
                }
                break;
            case Instruction::sub: {
                    auto op1 = inst->get_operand(0);
                    auto op2 = inst->get_operand(1);
                    auto const_op1 = dynamic_cast<ConstantInt*>(op1);
                    auto const_op2 = dynamic_cast<ConstantInt*>(op2);
                    Value *operand1;
                    IR2asm::Operand2 *operand2;
                    if (const_op1) {
                        operand1 = op2;
                        operand2 = new IR2asm::Operand2(const_op1->get_value());
                        code += IR2asm::r_sub(get_asm_reg(inst), get_asm_reg(operand1), operand2);
                    } else {
                        operand1 = op1;
                        if (const_op2) {
                            operand2 = new IR2asm::Operand2(const_op2->get_value());
                        } else {
                            operand2 = new IR2asm::Operand2(*get_asm_reg(op2));
                        }
                        code += IR2asm::sub(get_asm_reg(inst), get_asm_reg(operand1), operand2);
                    }
                }
                break;
            case Instruction::mul: {
                    auto op1 = inst->get_operand(0);
                    auto op2 = inst->get_operand(1);
                    code += IR2asm::mul(get_asm_reg(inst), get_asm_reg(op1), get_asm_reg(op2));
                }
                break;
            case Instruction::sdiv: { // divide constant can be optimized
                    auto op1 = inst->get_operand(0);
                    auto op2 = inst->get_operand(1);
                    code += IR2asm::sdiv(get_asm_reg(inst), get_asm_reg(op1), get_asm_reg(op2));
                }
                break;
            case Instruction::srem: // srem -> sdiv and msub
                break;
            case Instruction::alloca:   // no instruction in .s
                break;
            case Instruction::load: {
                    auto global_addr = dynamic_cast<GlobalVariable*>(inst->get_operand(0));
                    if (global_addr) {
                        code += IR2asm::safe_load(get_asm_reg(inst), 
                                                    global_variable_table[global_addr], 
                                                    sp_extra_ofst, 
                                                    long_func);
                        // code += IR2asm::load(get_asm_reg(inst), global_variable_table[global_addr]);
                        code += IR2asm::safe_load(get_asm_reg(inst), 
                                                    new IR2asm::Regbase(*get_asm_reg(inst), 0), 
                                                    sp_extra_ofst, 
                                                    long_func);
                        // code += IR2asm::load(get_asm_reg(inst), new IR2asm::Regbase(*get_asm_reg(inst), 0));
                    } else {
                        code += IR2asm::safe_load(get_asm_reg(inst), 
                                                    new IR2asm::Regbase(*get_asm_reg(inst->get_operand(0)), 0), 
                                                    sp_extra_ofst, 
                                                    long_func);
                        // code += IR2asm::load(get_asm_reg(inst), new IR2asm::Regbase(*get_asm_reg(inst->get_operand(0)), 0));
                    }
                }
                break;
            case Instruction::store: {
                    auto global_addr = dynamic_cast<GlobalVariable*>(inst->get_operand(1));
                    std::vector<int> tmp_reg = {};
                    int ld_reg_id = 11;
                    // OPERAND1 MUST BE GLOBAL IN LIR
                    auto opr0 = inst->get_operand(0);
                    int opr0_reg_id = reg_map[opr0]->reg_num; //must have a reg,alloc in bb_gen
                    for(int i = 0;i <= 12;i++){
                        if(i==11){
                            continue;
                        }
                        if(i!=opr0_reg_id){
                            ld_reg_id = i;
                            tmp_reg.push_back(i);
                            break;
                        }
                    }
                    code += push_regs(tmp_reg);
                    code += IR2asm::safe_load(new IR2asm::Reg(ld_reg_id),
                                              global_variable_table[global_addr],
                                              sp_extra_ofst,
                                              long_func);

                    code += IR2asm::safe_store(get_asm_reg(inst->get_operand((0))),
                                                 new IR2asm::Regbase(IR2asm::Reg(ld_reg_id), 0),
                                                 sp_extra_ofst,
                                                 long_func);
                    code += pop_regs(tmp_reg);
                }
                break;
            case Instruction::cmp: {
                    auto cmp_inst = dynamic_cast<CmpInst*>(inst);
                    auto cond1 = inst->get_operand(0);
                    auto cond2 = inst->get_operand(1);
                    auto cmp_op = cmp_inst->get_cmp_op();
                    auto const_cond1 = dynamic_cast<ConstantInt*>(cond1);
                    auto const_cond2 = dynamic_cast<ConstantInt*>(cond2);
                    IR2asm::Reg *operand1;
                    IR2asm::Operand2 *operand2;
                    switch (cmp_op)
                    {
                        case CmpInst::CmpOp::EQ: {
                            if (const_cond1) {
                                operand1 = get_asm_reg(cond2);
                                operand2 = new IR2asm::Operand2(const_cond1->get_value());
                            } else {
                                operand1 = get_asm_reg(cond1);
                                if (const_cond2) {
                                    operand2 = new IR2asm::Operand2(const_cond2->get_value());
                                } else {
                                    operand2 = new IR2asm::Operand2(*get_asm_reg(cond2));
                                }
                            }
                            code += IR2asm::cmp(operand1, operand2);
                            code += IR2asm::ldr_const(get_asm_reg(inst), new IR2asm::constant(0));
                            code += IR2asm::ldr_const(get_asm_reg(inst), new IR2asm::constant(1), "eq");
                        }
                        break;
                        case CmpInst::CmpOp::NE: {
                            if (const_cond1) {
                                operand1 = get_asm_reg(cond2);
                                operand2 = new IR2asm::Operand2(const_cond1->get_value());
                            } else {
                                operand1 = get_asm_reg(cond1);
                                if (const_cond2) {
                                    operand2 = new IR2asm::Operand2(const_cond2->get_value());
                                } else {
                                    operand2 = new IR2asm::Operand2(*get_asm_reg(cond2));
                                }
                            }
                            code += IR2asm::cmp(operand1, operand2);
                            code += IR2asm::ldr_const(get_asm_reg(inst), new IR2asm::constant(0));
                            code += IR2asm::ldr_const(get_asm_reg(inst), new IR2asm::constant(1), "ne");
                        }
                        break;
                        case CmpInst::CmpOp::GT: {
                            if (const_cond1) {
                                operand1 = get_asm_reg(cond2);
                                operand2 = new IR2asm::Operand2(const_cond1->get_value());
                            } else {
                                operand1 = get_asm_reg(cond1);
                                if (const_cond2) {
                                    operand2 = new IR2asm::Operand2(const_cond2->get_value());
                                } else {
                                    operand2 = new IR2asm::Operand2(*get_asm_reg(cond2));
                                }
                            }
                            code += IR2asm::cmp(operand1, operand2);
                            code += IR2asm::ldr_const(get_asm_reg(inst), new IR2asm::constant(0));
                            if (const_cond1) {
                                code += IR2asm::ldr_const(get_asm_reg(inst), new IR2asm::constant(1), "le");
                            } else {
                                code += IR2asm::ldr_const(get_asm_reg(inst), new IR2asm::constant(1), "gt");
                            }
                        }
                        break;
                        case CmpInst::CmpOp::GE: {
                            if (const_cond1) {
                                operand1 = get_asm_reg(cond2);
                                operand2 = new IR2asm::Operand2(const_cond1->get_value());
                            } else {
                                operand1 = get_asm_reg(cond1);
                                if (const_cond2) {
                                    operand2 = new IR2asm::Operand2(const_cond2->get_value());
                                } else {
                                    operand2 = new IR2asm::Operand2(*get_asm_reg(cond2));
                                }
                            }
                            code += IR2asm::cmp(operand1, operand2);
                            code += IR2asm::ldr_const(get_asm_reg(inst), new IR2asm::constant(0));
                            if (const_cond1) {
                                code += IR2asm::ldr_const(get_asm_reg(inst), new IR2asm::constant(1), "lt");
                            } else {
                                code += IR2asm::ldr_const(get_asm_reg(inst), new IR2asm::constant(1), "ge");
                            }
                        }
                        break;
                        case CmpInst::CmpOp::LT: {
                            if (const_cond1) {
                                operand1 = get_asm_reg(cond2);
                                operand2 = new IR2asm::Operand2(const_cond1->get_value());
                            } else {
                                operand1 = get_asm_reg(cond1);
                                if (const_cond2) {
                                    operand2 = new IR2asm::Operand2(const_cond2->get_value());
                                } else {
                                    operand2 = new IR2asm::Operand2(*get_asm_reg(cond2));
                                }
                            }
                            code += IR2asm::cmp(operand1, operand2);
                            code += IR2asm::ldr_const(get_asm_reg(inst), new IR2asm::constant(0));
                            if (const_cond1) {
                                code += IR2asm::ldr_const(get_asm_reg(inst), new IR2asm::constant(1), "ge");
                            } else {
                                code += IR2asm::ldr_const(get_asm_reg(inst), new IR2asm::constant(1), "lt");
                            }
                        }
                        break;
                        case CmpInst::CmpOp::LE: {
                            if (const_cond1) {
                                operand1 = get_asm_reg(cond2);
                                operand2 = new IR2asm::Operand2(const_cond1->get_value());
                            } else {
                                operand1 = get_asm_reg(cond1);
                                if (const_cond2) {
                                    operand2 = new IR2asm::Operand2(const_cond2->get_value());
                                } else {
                                    operand2 = new IR2asm::Operand2(*get_asm_reg(cond2));
                                }
                            }
                            code += IR2asm::cmp(operand1, operand2);
                            code += IR2asm::ldr_const(get_asm_reg(inst), new IR2asm::constant(0));
                            if (const_cond1) {
                                code += IR2asm::ldr_const(get_asm_reg(inst), new IR2asm::constant(1), "gt");
                            } else {
                                code += IR2asm::ldr_const(get_asm_reg(inst), new IR2asm::constant(1), "le");
                            }
                        }
                        break;
                        default:
                        break;
                    }
                }
                break;
            case Instruction::phi:  // has done before
                break;
            case Instruction::call:
                    code += IR2asm::call(new IR2asm::label(inst->get_operand(0)->get_name()));
                break;
            case Instruction::getelementptr: {
                    auto base_addr = inst->get_operand(0);
                    if (dynamic_cast<GlobalVariable*>(base_addr)) {
                        auto addr = global_variable_table[dynamic_cast<GlobalVariable*>(base_addr)];
                        code += IR2asm::safe_load(get_asm_reg(inst), addr, sp_extra_ofst, long_func);
                    } else if (dynamic_cast<AllocaInst*>(base_addr)) {
                        auto addr = stack_map[base_addr];
                        code += IR2asm::getelementptr(get_asm_reg(inst), addr);
                    } else {
                        auto addr = new IR2asm::Regbase(*get_asm_reg(base_addr), 0);
                        code += IR2asm::getelementptr(get_asm_reg(inst), addr);
                    }
                }
                break;
            case Instruction::land: {
                    auto op1 = inst->get_operand(0);
                    auto op2 = inst->get_operand(1);
                    auto const_op1 = dynamic_cast<ConstantInt*>(op1);
                    auto const_op2 = dynamic_cast<ConstantInt*>(op2);
                    Value *operand1;
                    IR2asm::Operand2 *operand2;
                    if (const_op1) {
                        operand1 = op2;
                        operand2 = new IR2asm::Operand2(const_op1->get_value());
                    } else {
                        operand1 = op1;
                        if (const_op2) {
                            operand2 = new IR2asm::Operand2(const_op2->get_value());
                        } else {
                            operand2 = new IR2asm::Operand2(*get_asm_reg(op2));
                        }
                    }
                    code += IR2asm::land(get_asm_reg(inst), get_asm_reg(operand1), operand2);
                }
                break;
            case Instruction::lor: {
                    auto op1 = inst->get_operand(0);
                    auto op2 = inst->get_operand(1);
                    auto const_op1 = dynamic_cast<ConstantInt*>(op1);
                    auto const_op2 = dynamic_cast<ConstantInt*>(op2);
                    Value *operand1;
                    IR2asm::Operand2 *operand2;
                    if (const_op1) {
                        operand1 = op2;
                        operand2 = new IR2asm::Operand2(const_op1->get_value());
                    } else {
                        operand1 = op1;
                        if (const_op2) {
                            operand2 = new IR2asm::Operand2(const_op2->get_value());
                        } else {
                            operand2 = new IR2asm::Operand2(*get_asm_reg(op2));
                        }
                    }
                    code += IR2asm::lor(get_asm_reg(inst), get_asm_reg(operand1), operand2);
                }
                break;
            case Instruction::lxor: {
                    auto op1 = inst->get_operand(0);
                    auto op2 = inst->get_operand(1);
                    auto const_op1 = dynamic_cast<ConstantInt*>(op1);
                    auto const_op2 = dynamic_cast<ConstantInt*>(op2);
                    Value *operand1;
                    IR2asm::Operand2 *operand2;
                    if (const_op1) {
                        operand1 = op2;
                        operand2 = new IR2asm::Operand2(const_op1->get_value());
                    } else {
                        operand1 = op1;
                        if (const_op2) {
                            operand2 = new IR2asm::Operand2(const_op2->get_value());
                        } else {
                            operand2 = new IR2asm::Operand2(*get_asm_reg(op2));
                        }
                    }
                    code += IR2asm::lxor(get_asm_reg(inst), get_asm_reg(operand1), operand2);
                }
                break;
            case Instruction::zext:
                break;
            case Instruction::asr: {
                    auto op1 = inst->get_operand(0);
                    auto op2 = inst->get_operand(1);
                    auto const_op2 = dynamic_cast<ConstantInt*>(op2);
                    auto operand1 = op1;
                    IR2asm::Operand2 *operand2;
                    if (const_op2) {
                        operand2 = new IR2asm::Operand2(const_op2->get_value());
                    } else {
                        operand2 = new IR2asm::Operand2(*get_asm_reg(op2));
                    }
                    code += IR2asm::asr(get_asm_reg(inst), get_asm_reg(operand1), operand2);
                }
                break;
            case Instruction::lsl: {
                    auto op1 = inst->get_operand(0);
                    auto op2 = inst->get_operand(1);
                    auto const_op2 = dynamic_cast<ConstantInt*>(op2);
                    auto operand1 = op1;
                    IR2asm::Operand2 *operand2;
                    if (const_op2) {
                        operand2 = new IR2asm::Operand2(const_op2->get_value());
                    } else {
                        operand2 = new IR2asm::Operand2(*get_asm_reg(op2));
                    }
                    code += IR2asm::lsl(get_asm_reg(inst), get_asm_reg(operand1), operand2);
                }
                break;
            case Instruction::lsr: {
                    auto op1 = inst->get_operand(0);
                    auto op2 = inst->get_operand(1);
                    auto const_op2 = dynamic_cast<ConstantInt*>(op2);
                    auto operand1 = op1;
                    IR2asm::Operand2 *operand2;
                    if (const_op2) {
                        operand2 = new IR2asm::Operand2(const_op2->get_value());
                    } else {
                        operand2 = new IR2asm::Operand2(*get_asm_reg(op2));
                    }
                    code += IR2asm::lsr(get_asm_reg(inst), get_asm_reg(operand1), operand2);
                }
                break;
            case Instruction::cmpbr: {
                    auto cmpbr_inst = dynamic_cast<CmpBrInst*>(inst);
                    auto cond1 = inst->get_operand(0);
                    auto cond2 = inst->get_operand(1);
                    auto cmp_op = cmpbr_inst->get_cmp_op();
                    auto true_bb = dynamic_cast<BasicBlock*>(inst->get_operand(2));
                    auto false_bb = dynamic_cast<BasicBlock*>(inst->get_operand(3));
                    auto const_cond1 = dynamic_cast<ConstantInt*>(cond1);
                    auto const_cond2 = dynamic_cast<ConstantInt*>(cond2);
                    IR2asm::Reg *operand1;
                    IR2asm::Operand2 *operand2;
                    switch (cmp_op)
                    {
                        case CmpBrInst::CmpOp::EQ: {
                            if (const_cond1) {
                                operand1 = get_asm_reg(cond2);
                                operand2 = new IR2asm::Operand2(const_cond1->get_value());
                            } else {
                                operand1 = get_asm_reg(cond1);
                                if (const_cond2) {
                                    operand2 = new IR2asm::Operand2(const_cond2->get_value());
                                } else {
                                    operand2 = new IR2asm::Operand2(*get_asm_reg(cond2));
                                }
                            }
                            code += IR2asm::cmp(operand1, operand2);
                            code += IR2asm::beq(bb_label[true_bb]);
                            code += IR2asm::b(bb_label[false_bb]);
                        }
                        break;
                        case CmpBrInst::CmpOp::NE: {
                            if (const_cond1) {
                                operand1 = get_asm_reg(cond2);
                                operand2 = new IR2asm::Operand2(const_cond1->get_value());
                            } else {
                                operand1 = get_asm_reg(cond1);
                                if (const_cond2) {
                                    operand2 = new IR2asm::Operand2(const_cond2->get_value());
                                } else {
                                    operand2 = new IR2asm::Operand2(*get_asm_reg(cond2));
                                }
                            }
                            code += IR2asm::cmp(operand1, operand2);
                            code += IR2asm::bne(bb_label[true_bb]);
                            code += IR2asm::b(bb_label[false_bb]);
                        }
                        break;
                        case CmpBrInst::CmpOp::GT: {
                            if (const_cond1) {
                                operand1 = get_asm_reg(cond2);
                                operand2 = new IR2asm::Operand2(const_cond1->get_value());
                            } else {
                                operand1 = get_asm_reg(cond1);
                                if (const_cond2) {
                                    operand2 = new IR2asm::Operand2(const_cond2->get_value());
                                } else {
                                    operand2 = new IR2asm::Operand2(*get_asm_reg(cond2));
                                }
                            }
                            code += IR2asm::cmp(operand1, operand2);
                            if (const_cond1) {
                                code += IR2asm::ble(bb_label[true_bb]);
                            } else {
                                code += IR2asm::bgt(bb_label[true_bb]);
                            }
                            code += IR2asm::b(bb_label[false_bb]);
                        }
                        break;
                        case CmpBrInst::CmpOp::GE: {
                            if (const_cond1) {
                                operand1 = get_asm_reg(cond2);
                                operand2 = new IR2asm::Operand2(const_cond1->get_value());
                            } else {
                                operand1 = get_asm_reg(cond1);
                                if (const_cond2) {
                                    operand2 = new IR2asm::Operand2(const_cond2->get_value());
                                } else {
                                    operand2 = new IR2asm::Operand2(*get_asm_reg(cond2));
                                }
                            }
                            code += IR2asm::cmp(operand1, operand2);
                            if (const_cond1) {
                                code += IR2asm::blt(bb_label[true_bb]);
                            } else {
                                code += IR2asm::bge(bb_label[true_bb]);
                            }
                            code += IR2asm::b(bb_label[false_bb]);
                        }
                        break;
                        case CmpBrInst::CmpOp::LT: {
                            if (const_cond1) {
                                operand1 = get_asm_reg(cond2);
                                operand2 = new IR2asm::Operand2(const_cond1->get_value());
                            } else {
                                operand1 = get_asm_reg(cond1);
                                if (const_cond2) {
                                    operand2 = new IR2asm::Operand2(const_cond2->get_value());
                                } else {
                                    operand2 = new IR2asm::Operand2(*get_asm_reg(cond2));
                                }
                            }
                            code += IR2asm::cmp(operand1, operand2);
                            if (const_cond1) {
                                code += IR2asm::bge(bb_label[true_bb]);
                            } else {
                                code += IR2asm::blt(bb_label[true_bb]);
                            }
                            code += IR2asm::b(bb_label[false_bb]);
                        }
                        break;
                        case CmpBrInst::CmpOp::LE: {
                            if (const_cond1) {
                                operand1 = get_asm_reg(cond2);
                                operand2 = new IR2asm::Operand2(const_cond1->get_value());
                            } else {
                                operand1 = get_asm_reg(cond1);
                                if (const_cond2) {
                                    operand2 = new IR2asm::Operand2(const_cond2->get_value());
                                } else {
                                    operand2 = new IR2asm::Operand2(*get_asm_reg(cond2));
                                }
                            }
                            code += IR2asm::cmp(operand1, operand2);
                            if (const_cond1) {
                                code += IR2asm::bgt(bb_label[true_bb]);
                            } else {
                                code += IR2asm::ble(bb_label[true_bb]);
                            }
                            code += IR2asm::b(bb_label[false_bb]);
                        }
                        break;
                        default:
                        break;
                    }
                }
                break;
            case Instruction::muladd: {
                auto op1 = inst->get_operand(0);
                auto op2 = inst->get_operand(1);
                auto op3 = inst->get_operand(2);
                code += IR2asm::muladd(get_asm_reg(inst), get_asm_reg(op1), get_asm_reg(op2), get_asm_reg(op3));
            }
                break;
            case Instruction::mulsub: {
                auto op1 = inst->get_operand(0);
                auto op2 = inst->get_operand(1);
                auto op3 = inst->get_operand(2);
                code += IR2asm::mulsub(get_asm_reg(inst), get_asm_reg(op1), get_asm_reg(op2), get_asm_reg(op3));
            }
                break;
            case Instruction::asradd: {
                auto op1 = inst->get_operand(0);
                auto op2 = inst->get_operand(1);
                auto op3 = inst->get_operand(2);
                auto const_op3 = dynamic_cast<ConstantInt*>(op3);
                IR2asm::Operand2 *operand2;
                if (const_op3) {
                    operand2 = new IR2asm::Operand2(*get_asm_reg(op2), IR2asm::ASR, get_asm_const(const_op3)->get_val());
                } else {
                    operand2 = new IR2asm::Operand2(*get_asm_reg(op2), IR2asm::ASR, *get_asm_reg(op3));
                }
                code += IR2asm::add(get_asm_reg(inst), get_asm_reg(op1), operand2);
            }
                break;
            case Instruction::lsladd: {
                auto op1 = inst->get_operand(0);
                auto op2 = inst->get_operand(1);
                auto op3 = inst->get_operand(2);
                auto const_op3 = dynamic_cast<ConstantInt*>(op3);
                IR2asm::Operand2 *operand2;
                if (const_op3) {
                    operand2 = new IR2asm::Operand2(*get_asm_reg(op2), IR2asm::LSL, get_asm_const(const_op3)->get_val());
                } else {
                    operand2 = new IR2asm::Operand2(*get_asm_reg(op2), IR2asm::LSL, *get_asm_reg(op3));
                }
                code += IR2asm::add(get_asm_reg(inst), get_asm_reg(op1), operand2);
            }
                break;
            case Instruction::lsradd: {
                auto op1 = inst->get_operand(0);
                auto op2 = inst->get_operand(1);
                auto op3 = inst->get_operand(2);
                auto const_op3 = dynamic_cast<ConstantInt*>(op3);
                IR2asm::Operand2 *operand2;
                if (const_op3) {
                    operand2 = new IR2asm::Operand2(*get_asm_reg(op2), IR2asm::LSR, get_asm_const(const_op3)->get_val());
                } else {
                    operand2 = new IR2asm::Operand2(*get_asm_reg(op2), IR2asm::LSR, *get_asm_reg(op3));
                }
                code += IR2asm::add(get_asm_reg(inst), get_asm_reg(op1), operand2);
            }
                break;
            case Instruction::asrsub: {
                auto op1 = inst->get_operand(0);
                auto op2 = inst->get_operand(1);
                auto op3 = inst->get_operand(2);
                auto const_op3 = dynamic_cast<ConstantInt*>(op3);
                IR2asm::Operand2 *operand2;
                if (const_op3) {
                    operand2 = new IR2asm::Operand2(*get_asm_reg(op2), IR2asm::ASR, get_asm_const(const_op3)->get_val());
                } else {
                    operand2 = new IR2asm::Operand2(*get_asm_reg(op2), IR2asm::ASR, *get_asm_reg(op3));
                }
                code += IR2asm::sub(get_asm_reg(inst), get_asm_reg(op1), operand2);
            }
                break;
            case Instruction::lslsub: {
                auto op1 = inst->get_operand(0);
                auto op2 = inst->get_operand(1);
                auto op3 = inst->get_operand(2);
                auto const_op3 = dynamic_cast<ConstantInt*>(op3);
                IR2asm::Operand2 *operand2;
                if (const_op3) {
                    operand2 = new IR2asm::Operand2(*get_asm_reg(op2), IR2asm::LSL, get_asm_const(const_op3)->get_val());
                } else {
                    operand2 = new IR2asm::Operand2(*get_asm_reg(op2), IR2asm::LSL, *get_asm_reg(op3));
                }
                code += IR2asm::sub(get_asm_reg(inst), get_asm_reg(op1), operand2);
            }
                break;
            case Instruction::lsrsub: {
                auto op1 = inst->get_operand(0);
                auto op2 = inst->get_operand(1);
                auto op3 = inst->get_operand(2);
                auto const_op3 = dynamic_cast<ConstantInt*>(op3);
                IR2asm::Operand2 *operand2;
                if (const_op3) {
                    operand2 = new IR2asm::Operand2(*get_asm_reg(op2), IR2asm::LSR, get_asm_const(const_op3)->get_val());
                } else {
                    operand2 = new IR2asm::Operand2(*get_asm_reg(op2), IR2asm::LSR, *get_asm_reg(op3));
                }
                code += IR2asm::sub(get_asm_reg(inst), get_asm_reg(op1), operand2);
            }
                break;
            case Instruction::smul_lo:
                break;
            case Instruction::smul_hi:
                break;
            case Instruction::smmul: {
                    auto op1 = inst->get_operand(0);
                    auto op2 = inst->get_operand(1);
                    code += IR2asm::smmul(get_asm_reg(inst), get_asm_reg(op1), get_asm_reg(op2));
                }
                break;
            case Instruction::load_offset: {
                    auto load_offset_inst = dynamic_cast<LoadOffsetInst*>(inst);
                    auto offset = load_offset_inst->get_offset();
                    auto const_offset = dynamic_cast<ConstantInt*>(offset);
                    if (const_offset) {
                        code += IR2asm::load(get_asm_reg(inst), new IR2asm::Regbase(*get_asm_reg(inst->get_operand(0)), const_offset->get_value(), IR2asm::LSL));
                    } else {
                        code += IR2asm::load(get_asm_reg(inst), new IR2asm::Regbase(*get_asm_reg(inst->get_operand(0)), *get_asm_reg(offset), IR2asm::LSL));
                    }
                }
                break;
            case Instruction::store_offset: {
                    auto store_offset_inst = dynamic_cast<StoreOffsetInst*>(inst);
                    auto offset = store_offset_inst->get_offset();
                    auto const_offset = dynamic_cast<ConstantInt*>(offset);
                    if (const_offset) {
                        code += IR2asm::store(get_asm_reg(inst->get_operand(0)), new IR2asm::Regbase(*get_asm_reg(inst->get_operand(1)), const_offset->get_value(), IR2asm::LSL));
                    } else {
                        code += IR2asm::store(get_asm_reg(inst->get_operand(0)), new IR2asm::Regbase(*get_asm_reg(inst->get_operand(1)), *get_asm_reg(offset), IR2asm::LSL));
                    }
                }
                break;
            case Instruction::mov_const: {
                    auto mov_inst = dynamic_cast<MovConstInst*>(inst);
                    auto const_val = mov_inst->get_const()->get_value();
                    code += IR2asm::ldr_const(get_asm_reg(inst), new IR2asm::constant(const_val));
                }
                break;
                default:
                    break;
        }
        return code;
    }
//     } // namespace CodeGen