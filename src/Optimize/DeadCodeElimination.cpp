#include "DeadCodeElimination.h"

#include <algorithm>

// 函数形参到实参的映射(只限数组)
std::map<Function *, std::map<int, std::set<Value *>>> func_array_args_to_params_map;
std::map<Function *, std::set<Function *>> func_caller_map;
std::map<Function *, bool> visited_func;

void build_func_array_args_to_params_map(Module *module) {
    func_array_args_to_params_map.clear();
    for (auto func : module->get_functions()) {
        func_array_args_to_params_map.insert({func, {}});
        for (auto arg : func->get_args()) {
            if (arg->get_type()->is_pointer_type()) {
                func_array_args_to_params_map[func].insert({arg->get_arg_no(), {}});
            }
        }
        if (func->get_num_basic_blocks() == 0) continue;
        for (auto use : func->get_use_list()) {
            auto call_instr = dynamic_cast<CallInst*>(use.val_);
            for (int i = 1; i < call_instr->get_num_operand(); i++) {
                auto param = call_instr->get_operand(i);
                if (param->get_type()->is_pointer_type()) {
                    func_array_args_to_params_map[func][i-1].insert(param);
                }
            }
        }
    }
    bool changed = true;
    while (changed) {
        changed = false;
        for (auto func : module->get_functions()) {
            for (auto arg : func_array_args_to_params_map[func]) {
                for (auto param : arg.second) {
                    auto caller_arg_to_callee_param = dynamic_cast<Argument*>(param);
                    if (caller_arg_to_callee_param) {
                        auto old_func_array_args_to_params_size = func_array_args_to_params_map[func][arg.first].size();
                        std::set_union(func_array_args_to_params_map[func][arg.first].begin(), 
                                        func_array_args_to_params_map[func][arg.first].end(), 
                                        func_array_args_to_params_map[func][caller_arg_to_callee_param->get_arg_no()].begin(), 
                                        func_array_args_to_params_map[func][caller_arg_to_callee_param->get_arg_no()].end(), 
                                        std::inserter(func_array_args_to_params_map[func][arg.first], func_array_args_to_params_map[func][arg.first].begin()));
                        if (old_func_array_args_to_params_size < func_array_args_to_params_map[func][arg.first].size()) {
                            changed = true;
                        }
                    }
                }
            }
        }
    }
    /*for (auto func : module->get_functions()) {
        for (auto arg : func_array_args_to_params_map[func]) {
            std::vector<Value*> args_to_delete;
            for (auto param : arg.second) {
                if (dynamic_cast<Argument*>(param)) {
                    args_to_delete.push_back(param);
                }
            }
            for (auto arg_to_delete : args_to_delete) {
                arg.second.erase(arg_to_delete);
            }
        }
    }*/
    return ;
}

void DeadCodeElimination::execute() {
    build_func_array_args_to_params_map(module);
    for (auto func : module->get_functions()) {
        if (func->get_num_basic_blocks() == 0) {
            continue;
        }
        func_ = func;
        instr_mark.clear();
        RDominateTree r_dom_tree(module);
        r_dom_tree.execute();
        SideEffectAnalysis side_effect_analysis(module);
        side_effect_analysis.execute();
        mark();
        remove_unmarked_bb();
        RDominateTree r_dom_tree_new(module);
        r_dom_tree_new.execute();
        sweep();
    }
    return ;
}

Value* get_store_base_addr(Instruction *store_instr) {
    auto base = store_instr->get_operand(1);
    auto base_ptr = dynamic_cast<Instruction*>(base);
    Value *base_addr;
    if (dynamic_cast<GlobalVariable*>(base)) {
        base_addr = base;
    } else if (base_ptr) {
        if (base_ptr->is_gep()) {
            base_addr = base_ptr->get_operand(0);
        } else if (base_ptr->is_add()) {
            base_addr = dynamic_cast<Instruction*>(base_ptr->get_operand(0))->get_operand(0);
        } else if (base_ptr->is_muladd()) {
            base_addr = dynamic_cast<Instruction*>(base_ptr->get_operand(2))->get_operand(0);
        } else if (base_ptr->is_lsladd()) {
            base_addr = dynamic_cast<Instruction*>(base_ptr->get_operand(0))->get_operand(0);
        } else {
            base_addr = nullptr;
        }
    }
    return base_addr;
}

Value* get_load_base_addr(Instruction *load_instr) {
    auto base = load_instr->get_operand(0);
    auto base_ptr = dynamic_cast<Instruction*>(base);
    Value *base_addr;
    if (dynamic_cast<GlobalVariable*>(base)) {
        base_addr = base;
    } else if (base_ptr) {
        if (base_ptr->is_gep()) {
            base_addr = base_ptr->get_operand(0);
        } else if (base_ptr->is_add()) {
            base_addr = dynamic_cast<Instruction*>(base_ptr->get_operand(0))->get_operand(0);
        } else if (base_ptr->is_muladd()) {
            base_addr = dynamic_cast<Instruction*>(base_ptr->get_operand(2))->get_operand(0);
        } else if (base_ptr->is_lsladd()) {
            base_addr = dynamic_cast<Instruction*>(base_ptr->get_operand(0))->get_operand(0);
        } else {
            base_addr = nullptr;
        }
    }
    return base_addr;
}

bool DeadCodeElimination::has_side_effect(Instruction *store_instr) {
    auto cur_bb = store_instr->get_parent();
    auto cur_func = cur_bb->get_parent();
    if (has_side_effect_to_cur_bb(store_instr, cur_bb)) {
        return true;
    } else if (has_side_effect_to_path(store_instr, cur_bb)) {
        return true;
    } else {
        for (auto use : cur_func->get_use_list()) {
            auto call_instr = dynamic_cast<CallInst*>(use.val_);
            if (call_instr) {
                auto caller = dynamic_cast<Function*>(call_instr->get_operand(0));
                if (caller) {
                    if (has_side_effect_to_caller(store_instr, caller)) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool DeadCodeElimination::has_side_effect_to_caller(Instruction *store_instr, Function *caller) {
    if (caller->get_num_basic_blocks() == 0) return false;
    // FIXME:
    return true;
}

bool DeadCodeElimination::has_side_effect_to_path(Instruction *store_instr, BasicBlock *start_bb) {
    if (has_side_effect_to_cur_bb(store_instr, start_bb)) return true;
    std::set<BasicBlock*> visited_bb = {start_bb};
    std::list<BasicBlock*> bb_to_visit = {start_bb};
    while (bb_to_visit.empty() == false) {
        auto cur_bb = bb_to_visit.front();
        for (auto succ_bb : cur_bb->get_succ_basic_blocks()) {
            if (visited_bb.find(succ_bb) != visited_bb.end()) continue;
            if (has_side_effect_to_cur_bb(store_instr, succ_bb)) {
                return true;
            }
            visited_bb.insert(succ_bb);
        }
        bb_to_visit.pop_front();
    }
    return false;
}

bool DeadCodeElimination::has_side_effect_to_cur_bb(Instruction *store_instr, BasicBlock *cur_bb) {
    auto base_addr = get_store_base_addr(store_instr);
    if (base_addr == nullptr) return false;
    auto cur_func = cur_bb->get_parent();
    auto global_var = dynamic_cast<GlobalVariable*>(store_instr->get_operand(1));
    auto arg_base_addr = dynamic_cast<Argument*>(base_addr);
    auto alloca_base_addr = dynamic_cast<AllocaInst*>(base_addr);
    auto global_array_base_addr = dynamic_cast<GlobalVariable*>(base_addr);
    auto store_instr_pos = cur_bb->find_instruction(store_instr);
    std::list<Instruction*>::iterator start_pos;
    if (store_instr_pos != cur_bb->get_instructions().end()) {
        start_pos = store_instr_pos;
    } else {
        start_pos = cur_bb->get_instructions().begin();
    }
    for (auto iter = start_pos; iter != cur_bb->get_instructions().end(); iter++) {
        auto instr = *iter;
        if (instr->is_call()) {
            auto callee = dynamic_cast<Function*>(instr->get_operand(0));
            if (callee) {
                if (callee->get_global_array_side_effect_load().find(base_addr) != callee->get_global_array_side_effect_load().end() || 
                    callee->get_global_var_side_effect_load().find(base_addr) != callee->get_global_var_side_effect_load().end()) {
                    return true;
                }
                for (int i = 1; i < instr->get_num_operand(); i++) {
                    if (callee->get_args_side_effect_load().find(i-1) != callee->get_args_side_effect_load().end()) {
                        if (func_array_args_to_params_map[cur_func][i-1].find(base_addr) 
                            != func_array_args_to_params_map[cur_func][i-1].end()) {
                                return true;
                        }
                    }
                }
            }
        } else if (instr->is_load() || instr->is_load_offset()) {
            auto load_base_addr = get_load_base_addr(instr);
            if (global_var) {
                if (load_base_addr == base_addr) {
                    return true;
                }
            } else {
                if (load_base_addr == base_addr) {
                    return true;
                } else {
                    if (arg_base_addr) {
                        if (func_array_args_to_params_map[cur_func][arg_base_addr->get_arg_no()].find(base_addr) 
                            != func_array_args_to_params_map[cur_func][arg_base_addr->get_arg_no()].end()) {
                                return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool DeadCodeElimination::is_critical(Instruction *instr) {
    if (instr->isTerminator()) return true;
    else if (instr->is_call()) {
        auto callee = dynamic_cast<Function*>(instr->get_operand(0));
        if (callee->get_num_basic_blocks() == 0) return true;
        if (callee->get_global_array_side_effect_store().size() > 0 || 
            callee->get_global_var_side_effect_store().size() > 0 ||
            callee->get_local_array_side_effect_store().size() > 0 || 
            callee->get_args_side_effect_store().size() > 0) {
                return true;
            } else {
                for (auto callee_callee : callee->get_callee_set()) {
                    if (callee_callee->get_num_basic_blocks() == 0) {
                        return true;
                    }
                }
                return false;
            }
    }
    else if (instr->is_store() || instr->is_store_offset()) {
        // TODO: better strategy
        // return has_side_effect(instr);
        return true;
    }
    else return false;
}

BasicBlock *DeadCodeElimination::get_nearest_marked_postdominator(Instruction *instr) {
    std::list<BasicBlock*> visit_bb;
    std::map<BasicBlock*, bool> visited_bb_map;
    auto instr_bb = instr->get_parent();
    auto instr_bb_rdoms = instr_bb->get_rdoms();
    for (auto bb : instr_bb_rdoms) {
        visited_bb_map.insert({bb, false});
    }
    visit_bb.push_back(instr_bb);
    while (visit_bb.empty() == false) {
        auto curr_bb = visit_bb.front();
        auto next_bb_list = curr_bb->get_succ_basic_blocks();
        for (auto next_bb : next_bb_list) {
            if (next_bb != instr_bb && instr_bb_rdoms.find(next_bb) != instr_bb_rdoms.end()) {
                if (instr_mark[next_bb->get_terminator()] == true) {
                    return next_bb;
                }
            }
            if (visited_bb_map[next_bb] == false) {
                visit_bb.push_back(next_bb);
                visited_bb_map[next_bb] = true;
            }
        }
        visit_bb.pop_front();
    }
    return nullptr;
}

void DeadCodeElimination::remove_unmarked_bb() {
    std::vector<BasicBlock*> wait_delete_bb;
    for (auto bb : func_->get_basic_blocks()) {
        if (bb == func_->get_entry_block()) continue;
        bool marked = false;
        for (auto instr : bb->get_instructions()) {
            if (instr_mark[instr] == true) {
                marked = true;
                break;
            }
        }
        if (marked == false) {
            wait_delete_bb.push_back(bb);
        }
    }
    for (auto delete_bb : wait_delete_bb) {
        auto delete_instr_list = delete_bb->get_instructions();
        for (auto instr : delete_instr_list) {
            delete_bb->delete_instr(instr);
        }
        func_->remove(delete_bb);
    }
}

void DeadCodeElimination::mark() {
    std::vector<Instruction *> work_list;
    for (auto bb : func_->get_basic_blocks()) {
        for (auto instr : bb->get_instructions()) {
            if (is_critical(instr)) {
                instr_mark.insert({instr, true});
                work_list.push_back(instr);
            } else {
                instr_mark.insert({instr, false});
            }
        }
    }
    while (work_list.empty() == false) {
        auto instr = work_list.back();
        work_list.pop_back();
        for (auto operand : instr->get_operands()) {
            auto def = dynamic_cast<Instruction*>(operand);
            auto bb = dynamic_cast<BasicBlock*>(operand);
            if (def != nullptr && instr_mark[def] == false) {
                instr_mark[def] = true;
                work_list.push_back(def);
            }
            if (bb != nullptr) {
                auto bb_terminator = bb->get_terminator();
                if (instr_mark[bb_terminator] == false) {
                    instr_mark[bb_terminator] = true;
                    work_list.push_back(bb_terminator);
                }
            }
        }
        for (auto reverse_dom_froniter_bb : instr->get_parent()->get_rdom_frontier()) {
            auto terminator = reverse_dom_froniter_bb->get_terminator();
            if ((terminator->is_br() || terminator->is_cmpbr()) && instr_mark[terminator] == false) {
                instr_mark[terminator] = true;
                work_list.push_back(terminator);
            }
        }
    }
    return ;
}

void DeadCodeElimination::sweep() {
    std::vector<Instruction*> wait_delete;
    for (auto bb : func_->get_basic_blocks()) {
        for (auto instr : bb->get_instructions()) {
            if (instr_mark[instr] == false) {
                if (instr->is_br() && instr->get_num_operand() == 3) {
                    auto targetBB = get_nearest_marked_postdominator(instr);
                    instr->remove_use_of_ops();
                    instr->set_operand(0, targetBB);
                } else if (instr->is_cmpbr() && instr->get_num_operand() == 4) {
                    auto targetBB = get_nearest_marked_postdominator(instr);
                    auto true_bb = dynamic_cast<BasicBlock* >(instr->get_operand(2));
                    auto false_bb = dynamic_cast<BasicBlock* >(instr->get_operand(3));
                    true_bb->remove_pre_basic_block(bb);
                    false_bb->remove_pre_basic_block(bb);
                    bb->remove_succ_basic_block(true_bb);
                    bb->remove_succ_basic_block(false_bb);;
                    BranchInst::create_br(targetBB, bb);
                }
                if (!(instr->is_br() && instr->get_num_operand() == 1)) {
                    wait_delete.push_back(instr);
                }
            }
        }
        for (auto instr : wait_delete) {
            //std::cerr<<"delete "<<instr->print()<<std::endl;
            // TODO: 绑定成一条汇编指令的ir指令集合(如smul_lo和smul_hi绑定为smul)要么全部传播,要么都不传播
            if (instr->is_smul_lo() || instr->is_smul_hi()) {
                continue;
            }
            bb->delete_instr(instr);
        }
    }
    return ;
}
