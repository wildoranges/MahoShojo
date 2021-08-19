#include "ActiveVar.h"

#include <algorithm>

void ActiveVar::execute() {
    for (auto &func : this->module->get_functions()) {
        if (func->get_basic_blocks().empty()) {
            continue;
        } else {
            func_ = func;  
            live_in.clear();
            live_out.clear();
            def_var.clear();
            use_var.clear();
            get_def_var();
            get_use_var();
            get_live_in_live_out();
            for (auto bb : func_->get_basic_blocks()) {
                bb->set_use_var(use_var[bb]);
                bb->set_def_var(def_var[bb]);
                bb->set_live_in(live_in[bb]);
                bb->set_live_out(live_out[bb]);
            }
        }
    }
    return ;
}

void ActiveVar::get_def_var() {
    for (auto bb : func_->get_basic_blocks()) {
        def_var.insert({bb, {}});
        for (auto instr : bb->get_instructions()) {
            if (instr->is_void()) {
                continue;
            }
            def_var[bb].insert(instr);
        }
    }
    return ;
}

void ActiveVar::get_use_var() {
    for (auto bb : func_->get_basic_blocks()) {
        use_var.insert({bb, {}});
        phi_use_before_def.insert({bb, {}});
        for (auto instr : bb->get_instructions()) {
            if (instr->is_phi()){
                for (auto i = 0; i < instr->get_num_operand(); i+=2){
                    phi_use_before_def[bb].insert(instr->get_operand(i));
                }
            }
            for (auto op: instr->get_operands()) {
                if (dynamic_cast<ConstantInt*>(op)) continue;
                if (dynamic_cast<BasicBlock*>(op)) continue;
                if (dynamic_cast<Function*>(op)) continue;
                use_var[bb].insert(op);
            }
        }
        for (auto var : def_var[bb]) {
            if (use_var[bb].find(var) != use_var[bb].end()) {
                use_var[bb].erase(var);
            }
        }
    }
    return ;
}

void ActiveVar::get_live_in_live_out() {
    get_def_var();
    get_use_var();
    for (auto bb : func_->get_basic_blocks()) {
        live_in.insert({bb, {}});
        live_out.insert({bb, {}});
    }
    bool repeat = true;
    while (repeat) {
        repeat = false;
        for (auto bb : func_->get_basic_blocks()) {
            std::set<Value *> tmp_live_out = {};
            for (auto succBB : bb->get_succ_basic_blocks()) {
                auto succ_phi_use = phi_use_before_def[succBB];
                std::set<Value *> other_phi;
                for (auto inst : succBB->get_instructions()){
                    if (inst->is_phi()){
                        for (auto i = 0; i < inst->get_num_operand(); i+=2){
                            if (inst->get_operand(i+1) != bb){
                                //tocheck
                                other_phi.insert(inst->get_operand(i));
                            }
                        }
                    }
                    else{
                        break;
                    }
                }
                std::set<Value *> not_in_other_phi = {};
                for (auto to_check : other_phi){
                    auto succ_out = live_out[succBB];
                    auto succ_def = def_var[succBB];
                    std::set<Value *> from_succ_child = {};
                    std::set_difference(succ_out.begin(), succ_out.end(), succ_def.begin(), succ_def.end(), std::inserter(from_succ_child, from_succ_child.begin()));
                    if (from_succ_child.find(to_check) != from_succ_child.end()){
                        not_in_other_phi.insert(to_check);
                        continue;
                    }
                    auto in_other_phi = true;
                    for (auto pair : to_check->get_use_list()){
                        auto use_inst = dynamic_cast<Instruction *>(pair.val_);
                        auto use_no = pair.arg_no_;
                        if (use_inst->get_parent() != succBB){
                            continue;
                        }
                        if (use_inst->is_phi()){
                            auto from_BB = use_inst->get_operand(use_no+1);
                            if (from_BB == bb){
                                in_other_phi = false;
                                break;
                            }
                        }
                        else{
                            in_other_phi = false;
                            break;
                        }
                    }
                    if (in_other_phi == false){
                        not_in_other_phi.insert(to_check);
                    }
                }
                for (auto to_delete : not_in_other_phi){
                    other_phi.erase(to_delete);
                }
                auto succ_live_in = live_in[succBB];
                std::set<Value *> succ_live_in_without_other_phi;
                std::set_difference(succ_live_in.begin(),succ_live_in.end(), other_phi.begin(),other_phi.end(), std::inserter(succ_live_in_without_other_phi, succ_live_in_without_other_phi.begin()));
                std::set<Value *> tmp;
                std::set_union(tmp_live_out.begin(), tmp_live_out.end(), succ_live_in_without_other_phi.begin(), succ_live_in_without_other_phi.end(), std::inserter(tmp, tmp.begin()));
                tmp_live_out = tmp;
            }
            // 迭代后的in和out必不可能小于迭代前的in和out(归纳法可证)
            live_out[bb] = tmp_live_out;
            auto tmp_live_in = tmp_live_out;
            std::set<Value *> tmp1,tmp2,tmp3;
            std::set_difference(tmp_live_in.begin(), tmp_live_in.end(), def_var[bb].begin(), def_var[bb].end(), std::inserter(tmp1, tmp1.begin()));
            std::set_union(tmp1.begin(), tmp1.end(), use_var[bb].begin(), use_var[bb].end(), std::inserter(tmp2, tmp2.begin()));
            std::set_union(tmp2.begin(), tmp2.end(), phi_use_before_def[bb].begin(), phi_use_before_def[bb].end(), std::inserter(tmp3, tmp3.begin()));
            if (tmp3.size() > live_in[bb].size()){
                live_in[bb] = tmp3;
                repeat = true;
            }
        }
    }
    return ;
}
