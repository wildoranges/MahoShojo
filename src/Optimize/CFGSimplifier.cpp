#include "CFGSimplifier.h"

#include <algorithm>

// FIXME: may have bugs

void CFGSimplifier::execute() {
    for (auto func : module->get_functions()) {
        if (func->get_num_basic_blocks() == 0) {
            continue;
        }
        func_ = func;
        postorder_bb_list.clear();
        bool changed = true;
        while (changed) {
            postorder_bb_list.clear();
            compute_postorder();
            changed = delete_unreachable_bb() || one_pass() || delete_redundant_phi();
        }
    }
    return ;
}

bool CFGSimplifier::delete_redundant_phi() {
    bool changed = false;
    for (auto bb : func_->get_basic_blocks()) {
        std::vector<Instruction*> wait_delete_instr;
        for (auto instr : bb->get_instructions()) {
            if (instr->is_phi()) {
                if (instr->get_num_operand() == 2 && bb->get_pre_basic_blocks().size() == 1) {
                    instr->replace_all_use_with(instr->get_operand(0));
                    wait_delete_instr.push_back(instr);
                    changed = true;
                } else {
                    if (instr->get_num_operand()/2 < bb->get_pre_basic_blocks().size()) continue;
                    auto val = instr->get_operand(0);
                    auto const_val = dynamic_cast<ConstantInt*>(val);
                    bool can_replace = true;
                    if (const_val) {
                        auto const_value = const_val->get_value();
                        for (int i = 2; i < instr->get_num_operand(); i+=2) {
                            auto cur_val = instr->get_operand(i);
                            auto cur_const_val = dynamic_cast<ConstantInt*>(cur_val);
                            if (cur_const_val) {
                                if (cur_const_val->get_value() != const_value) {
                                    can_replace = false;
                                    break;
                                }
                            } else {
                                can_replace = false;
                                break;
                            }
                        }
                    } else {
                        auto val_to_instr = dynamic_cast<Instruction*>(val);
                        if (val_to_instr && val_to_instr->is_phi() && val_to_instr->get_parent() == instr->get_parent()) {
                            can_replace = false;
                            break;
                        } else {
                            for (int i = 2; i < instr->get_num_operand(); i+=2) {
                                auto cur_val = instr->get_operand(i);
                                if (cur_val != val) {
                                    can_replace = false;
                                    break;
                                }
                            }
                        }
                    }
                    if (can_replace == true) {
                        //have bug, use is phi in the same bb
                        instr->replace_all_use_with(instr->get_operand(0));
                        wait_delete_instr.push_back(instr);
                        changed = true;
                    }
                }
            }
        }
        for (auto delete_instr : wait_delete_instr) {
            bb->delete_instr(delete_instr);
        }
        wait_delete_instr.clear();
    }
    return changed;
}

bool CFGSimplifier::delete_unreachable_bb() {
    bool changed = false;
    std::vector<BasicBlock*> unreachable_bb_list;
    for (auto bb : func_->get_basic_blocks()) {
        if (std::find(postorder_bb_list.begin(), postorder_bb_list.end(), bb) == postorder_bb_list.end()) {
            unreachable_bb_list.push_back(bb);
            changed = true;
        }
    }
    for (auto unreachable_bb : unreachable_bb_list) {
        for (auto bb : func_->get_basic_blocks()) {
            std::vector<Instruction*> wait_delete_instr_list;
            for (auto instr : bb->get_instructions()) {
                if (instr->is_phi()) {
                    for (int i = 1; i < instr->get_num_operand(); i+=2) {
                        if (instr->get_operand(i) == unreachable_bb) {
                            for (int j = i + 1; j < instr->get_num_operand(); j++) {
                                for (auto &use : instr->get_operand(j)->get_use_list()) {
                                    if (use.val_ == instr && use.arg_no_ == j) {
                                        use.arg_no_ -= 2;
                                    }
                                }
                            }
                            instr->remove_operands(i - 1, i);
                        }
                    }
                    if (instr->get_num_operand() == 0) {
                        wait_delete_instr_list.push_back(instr);
                    }
                }
            }
            for (auto delete_instr : wait_delete_instr_list) {
                bb->delete_instr(delete_instr);
            }
        }
        auto delete_instr_list = unreachable_bb->get_instructions();
        for (auto instr : delete_instr_list) {
            unreachable_bb->delete_instr(instr);
        }
        func_->remove(unreachable_bb);
    }
    return changed;
}

void CFGSimplifier::compute_postorder() {
    std::map<BasicBlock*, bool> visited_bb;
    std::vector<BasicBlock*> dfs_bb_list;
    auto bb_list = func_->get_basic_blocks();
    for (auto bb : bb_list) {
        visited_bb.insert({bb, false});
    }
    dfs_bb_list.push_back(func_->get_entry_block());
    while (dfs_bb_list.empty() == false) {
        auto cur_bb = dfs_bb_list.back();
        bool flag = false;
        for (auto next_bb : cur_bb->get_succ_basic_blocks()) {
            if (visited_bb[next_bb] == false) {
                visited_bb[next_bb] = true;
                dfs_bb_list.push_back(next_bb);
                flag = true;
                break;
            }
        }
        if (flag == true) {
            continue;
        } else {
            postorder_bb_list.push_back(cur_bb);
            dfs_bb_list.pop_back();
        }
    }
    return ;
}

bool CFGSimplifier::bb_can_delete(BasicBlock *bb) {
    if (bb == bb->get_parent()->get_entry_block()) return false;
    auto terminator = bb->get_terminator();
    auto target_bb = terminator->get_operand(0);
    for (auto pre_bb : bb->get_pre_basic_blocks()) {
        auto pre_terminator = pre_bb->get_terminator();
        BasicBlock *pre_true_bb;
        BasicBlock *pre_false_bb;
        if (pre_terminator->is_br() && pre_terminator->get_num_operand() == 3) {
            pre_true_bb = dynamic_cast<BasicBlock*>(pre_terminator->get_operand(1));
            pre_false_bb = dynamic_cast<BasicBlock*>(pre_terminator->get_operand(2));
        } else if (pre_terminator->is_cmpbr() && pre_terminator->get_num_operand() == 4) {
            pre_true_bb = dynamic_cast<BasicBlock*>(pre_terminator->get_operand(2));
            pre_false_bb = dynamic_cast<BasicBlock*>(pre_terminator->get_operand(3));
        }
        if ((pre_true_bb == bb && pre_false_bb == target_bb) || (pre_false_bb == bb && pre_true_bb == target_bb)) {
            return false;
        }
    }
    return true;
}

bool CFGSimplifier::is_self_loop(BasicBlock *bb) {
    //no use
    return (bb->get_pre_basic_blocks().size() == 1 && bb == bb->get_pre_basic_blocks().front());
}

void CFGSimplifier::replace_phi(BasicBlock *victim_bb, std::list<BasicBlock*> pre_bb_list, BasicBlock *succ_bb) {
    std::vector<Instruction*> wait_delete_instr;
    for (auto instr : succ_bb->get_instructions()) {
        if (instr->is_phi()) {
            for (int i = 1; i < instr->get_num_operand(); i+=2) {
                auto target_bb = dynamic_cast<BasicBlock*>(instr->get_operand(i));
                if (target_bb == victim_bb) {
                    auto val = instr->get_operand(i - 1);
                    bool first_flag = true;
                    for (auto pre_bb : pre_bb_list) {
                        if (first_flag == true) {
                            first_flag = false;
                            instr->set_operand(i, pre_bb);
                        } else {
                            dynamic_cast<PhiInst*>(instr)->add_phi_pair_operand(val, pre_bb);
                        }
                    }
                    if (instr->get_num_operand() == 2  && instr->get_parent()->get_pre_basic_blocks().size() == 1) {
                        instr->replace_all_use_with(instr->get_operand(0));
                        wait_delete_instr.push_back(instr);
                    }
                }
            }
        }
    }
    for (auto delete_instr : wait_delete_instr) {
        succ_bb->delete_instr(delete_instr);
    }
}

void CFGSimplifier::combine_bb(BasicBlock *bb, BasicBlock *succ_bb) {
    // bb has only one successor succ_bb and succ_bb has only one predecessor bb
    std::vector<Instruction*> wait_delete_instr;
    bb->remove_succ_basic_block(succ_bb);
    succ_bb->remove_pre_basic_block(bb);
    for (auto succ_succ_bb : succ_bb->get_succ_basic_blocks()) {
        succ_succ_bb->remove_pre_basic_block(succ_bb);
        bb->add_succ_basic_block(succ_succ_bb);
        succ_succ_bb->add_pre_basic_block(bb);
        replace_phi(succ_bb, {bb}, succ_succ_bb);
    }
    bb->delete_instr(bb->get_terminator());
    std::list<Instruction*>::iterator phi_pos = bb->get_instructions().begin();
    for (auto iter = bb->get_instructions().begin(); iter != bb->get_instructions().end(); iter++) {
        if ((*iter)->is_phi() == false) {
            phi_pos = iter;
            break;
        }
    }
    for (auto instr : succ_bb->get_instructions()) {
        if (instr->is_phi()) {
            //succ_bb have phi???
            //not check
            for (int i = 1; i < instr->get_num_operand(); i+=2) {
                if (instr->get_operand(i) == bb) {
                    if (instr->get_num_operand() == 2  && succ_bb->get_pre_basic_blocks().size() == 0) {
                        instr->replace_all_use_with(instr->get_operand(0));
                        wait_delete_instr.push_back(instr);
                    } else {
                        for (int j = i + 1; j < instr->get_num_operand(); j++) {
                            for (auto &use : instr->get_operand(j)->get_use_list()) {
                                if (use.val_ == instr && use.arg_no_ == j) {
                                    use.arg_no_ -= 2;
                                }
                            }
                        }
                        instr->remove_operands(i - 1, i);
                        i -= 2;
                        if (instr->get_num_operand() == 2  && succ_bb->get_pre_basic_blocks().size() == 0) {
                            instr->replace_all_use_with(instr->get_operand(0));
                            wait_delete_instr.push_back(instr);
                        } else if (instr->get_num_operand() == 0) {
                            wait_delete_instr.push_back(instr);
                        }
                    }
                }
            }
            bb->add_instruction(phi_pos, instr);
        } else {
            bb->add_instruction(instr);
        }
        instr->set_parent(bb);
    }
    for (auto delete_instr : wait_delete_instr) {
        bb->delete_instr(delete_instr);
    }
    return ;
}

bool CFGSimplifier::one_pass() {
    std::vector<BasicBlock*> wait_delete_bb;
    std::vector<BasicBlock*> wait_remove_bb;
    bool changed = false;
    for (auto bb : postorder_bb_list) {
        if (is_self_loop(bb)) {
            wait_delete_bb.push_back(bb);
            continue;
        }
        auto terminator = bb->get_terminator();
        if (terminator->is_br() && terminator->get_num_operand() == 3) {
            if (terminator->is_br()) {
                auto true_bb = dynamic_cast<BasicBlock*>(terminator->get_operand(1));
                auto false_bb = dynamic_cast<BasicBlock*>(terminator->get_operand(2));
                if (true_bb != nullptr && false_bb != nullptr && true_bb == false_bb) {
                    //??? num_operand == 3 ???
                    terminator->remove_operands(0,2);
                    terminator->add_operand(true_bb);
                    changed = true;
                }
            }
        }
        if (terminator->is_br() && terminator->get_num_operand() == 1) {
            auto succ_bb = dynamic_cast<BasicBlock*>(terminator->get_operand(0));
            auto succ_bb_terminator = dynamic_cast<Instruction*>(succ_bb->get_terminator());
            if (bb->get_num_of_instr() == 1 && bb_can_delete(bb) == true) {  // empty bb
                for (auto pre_bb : bb->get_pre_basic_blocks()) {
                    pre_bb->remove_succ_basic_block(bb);
                    pre_bb->add_succ_basic_block(succ_bb);
                    succ_bb->add_pre_basic_block(pre_bb);
                    auto pre_terminator = pre_bb->get_terminator();
                    for (int i = 0; i < pre_terminator->get_num_operand() ; i++) {
                        auto target_bb = dynamic_cast<BasicBlock*>(pre_terminator->get_operand(i));
                        if (target_bb == bb) {
                            pre_terminator->set_operand(i, succ_bb);
                        }
                    }
                }
                replace_phi(bb, bb->get_pre_basic_blocks(), succ_bb);
                wait_delete_bb.push_back(bb);
                changed = true;
            }
            if (succ_bb->get_pre_basic_blocks().size() == 1) {  // succ_bb only has one predecessor, combine them
                combine_bb(bb, succ_bb);
                wait_remove_bb.push_back(succ_bb);
                changed = true;
            }
            if (succ_bb->get_num_of_instr() == 1 && succ_bb_terminator->is_br() && succ_bb_terminator->is_cmpbr() && succ_bb_terminator->get_num_operand() > 1) {
                bb->delete_instr(terminator);
                bb->add_instruction(succ_bb_terminator);
                for (auto succ_succ_bb : succ_bb->get_succ_basic_blocks()) {
                    succ_succ_bb->remove_pre_basic_block(succ_bb);
                    bb->add_succ_basic_block(succ_succ_bb);
                    succ_succ_bb->add_pre_basic_block(bb);
                    replace_phi(succ_bb, {bb}, succ_succ_bb);
                }
                changed = true;
            }
        }
    }
    for (auto delete_bb : wait_delete_bb) {
        auto delete_instr_list = delete_bb->get_instructions();
        for (auto instr : delete_instr_list) {
            delete_bb->delete_instr(instr);
        }
        func_->remove(delete_bb);
    }
    for (auto remove_bb : wait_remove_bb) {
        func_->remove(remove_bb);
    }
    return changed;
}
