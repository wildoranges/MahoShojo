#include "SideEffectAnalysis.h"

#include <algorithm>

void SideEffectAnalysis::execute() {
    func_callee_map.clear();
    func_args_side_effect.clear();
    func_args_effected_by_side_effect.clear();
    func_total_local_array_side_effect.clear();
    func_total_local_array_effected_by_side_effect.clear();
    func_global_var_side_effect.clear();
    func_total_global_var_side_effect.clear();
    func_global_array_side_effect.clear();
    func_total_global_array_side_effect.clear();
    func_global_var_effected_by_side_effect.clear();
    func_total_global_var_effected_by_side_effect.clear();
    func_global_array_effected_by_side_effect.clear();
    func_total_global_array_effected_by_side_effect.clear();
    for (auto func : module->get_functions()) {
        func_callee_map.insert({func, {}});
        func_args_side_effect.insert({func, {}});
        func_args_effected_by_side_effect.insert({func, {}});
        func_total_local_array_side_effect.insert({func, {}});
        func_total_local_array_effected_by_side_effect.insert({func, {}});
        func_global_var_side_effect.insert({func, {}});
        func_total_global_var_side_effect.insert({func, {}});
        func_global_array_side_effect.insert({func, {}});
        func_total_global_array_side_effect.insert({func, {}});
        func_global_var_effected_by_side_effect.insert({func, {}});
        func_total_global_var_effected_by_side_effect.insert({func, {}});
        func_global_array_effected_by_side_effect.insert({func, {}});
        func_total_global_array_effected_by_side_effect.insert({func, {}});
    }
    for (auto func : module->get_functions()) {
        get_func_side_effect(func);
        // TODO: may need improvement
        get_func_var_effected_by_side_effect(func);
    }
    get_func_callee_map();
    get_func_total_side_effect();
    // TODO: may need improvement
    get_func_total_var_effected_by_side_effect();
    for (auto func : module->get_functions()) {
        func->set_callee_set(func_callee_map[func]);
        func->set_args_side_effect_store(func_args_side_effect[func]);
        func->set_args_side_effect_load(func_args_effected_by_side_effect[func]);
        func->set_local_array_side_effect_store(func_total_local_array_side_effect[func]);
        func->set_local_array_side_effect_load(func_total_local_array_effected_by_side_effect[func]);
        func->set_global_var_side_effect_store(func_total_global_var_side_effect[func]);
        func->set_global_var_side_effect_load(func_total_global_var_effected_by_side_effect[func]);
        func->set_global_array_side_effect_store(func_total_global_array_side_effect[func]);
        func->set_global_array_side_effect_load(func_total_global_array_effected_by_side_effect[func]);
    }
    return ;
}

void SideEffectAnalysis::get_func_callee_map() {
    for (auto func : module->get_functions()) {
        if (func->get_num_basic_blocks() == 0) continue;
        for (auto bb : func->get_basic_blocks()) {
            for (auto instr : bb->get_instructions()) {
                if (instr->is_call()) {
                    func_callee_map[func].insert(dynamic_cast<Function*>(instr->get_operand(0)));
                }
            }
        }
    }
    bool changed = true;
    while (changed) {
        changed = false;
        for (auto func : module->get_functions()) {
            if (func->get_num_basic_blocks() == 0) continue;
            auto old_callee_num = func_callee_map[func].size();
            std::set<Function*> tmp_callee_map;
            for (auto callee : func_callee_map[func]) {
                std::set_union(tmp_callee_map.begin(), 
                                tmp_callee_map.end(), 
                                func_callee_map[callee].begin(), 
                                func_callee_map[callee].end(), 
                                std::inserter(tmp_callee_map, tmp_callee_map.begin()));
            }
            std::set_union(func_callee_map[func].begin(), 
                            func_callee_map[func].end(), 
                            tmp_callee_map.begin(), 
                            tmp_callee_map.end(), 
                            std::inserter(func_callee_map[func], func_callee_map[func].begin()));
            if (old_callee_num < func_callee_map[func].size()) {
                changed = true;
            }
        }
    }
    return ;
}

void SideEffectAnalysis::get_func_side_effect(Function *func) {
    if (func->get_num_basic_blocks() == 0) {
        for (auto arg : func->get_args()) {
            if (arg->get_type()->is_pointer_type()) {
                func_args_side_effect[func].insert(arg->get_arg_no());
            }
        }
        return ;
    } else {
        for (auto bb : func->get_basic_blocks()) {
            for (auto instr : bb->get_instructions()) {
                if (instr->is_store() || instr->is_store_offset()) {
                    auto val = instr->get_operand(0);
                    if (val->get_type()->is_pointer_type()) {
                        auto arg_ptr = dynamic_cast<Argument*>(val);
                        if (arg_ptr) {
                            func_args_side_effect[func].insert(arg_ptr->get_arg_no());
                        }
                    }
                    auto base = instr->get_operand(1);
                    auto base_ptr = dynamic_cast<Instruction*>(base);
                    if (dynamic_cast<GlobalVariable*>(base)) {
                        func_global_var_side_effect[func].insert(base);
                    } else if (base_ptr) {
                        Value *base_addr;
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
                        if (base_addr) {
                            auto arg_base_addr = dynamic_cast<Argument*>(base_addr);
                            auto global_array_base_addr = dynamic_cast<GlobalVariable*>(base_addr);
                            if (arg_base_addr) {
                                func_args_side_effect[func].insert(arg_base_addr->get_arg_no());
                            } else if (global_array_base_addr) {
                                func_global_array_side_effect[func].insert(global_array_base_addr);
                            }
                        }
                    }
                }
            }
        }
    }
    return ;
}

void SideEffectAnalysis::get_func_var_effected_by_side_effect(Function *func) {
    if (func->get_num_basic_blocks() == 0) return ;
    for (auto bb : func->get_basic_blocks()) {
        for (auto instr : bb->get_instructions()) {
            if (instr->is_load() || instr->is_load_offset()) {
                auto base = instr->get_operand(0);
                auto base_ptr = dynamic_cast<Instruction*>(base);
                if (dynamic_cast<GlobalVariable*>(base)) {
                    func_global_var_effected_by_side_effect[func].insert(base);
                } else if (base_ptr) {
                    Value *base_addr;
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
                    if (base_addr) {
                        auto arg_base_addr = dynamic_cast<Argument*>(base_addr);
                        auto global_array_base_addr = dynamic_cast<GlobalVariable*>(base_addr);
                        if (arg_base_addr) {
                            func_args_effected_by_side_effect[func].insert(arg_base_addr->get_arg_no());
                        } else if (global_array_base_addr) {
                            func_global_array_effected_by_side_effect[func].insert(global_array_base_addr);
                        }
                    }
                }
            }
        }
    }
    return ;
}

void SideEffectAnalysis::get_func_total_side_effect() {
    func_total_global_var_side_effect = func_global_var_side_effect;
    func_total_global_array_side_effect = func_global_array_side_effect;
    for (auto func : module->get_functions()) {
        if (func->get_num_basic_blocks() == 0) continue;
        for (auto bb : func->get_basic_blocks()) {
            for (auto instr : bb->get_instructions()) {
                if (instr->is_call()) {
                    auto callee = dynamic_cast<Function*>(instr->get_operand(0));
                    for (int i = 1; i < instr->get_num_operand(); i++) {
                        if (func_args_side_effect[callee].find(i-1) != func_args_side_effect[callee].end()) {
                            if (dynamic_cast<Argument*>(instr->get_operand(i))) {
                                func_total_local_array_side_effect[func].insert(instr->get_operand(i));
                            } else {
                                auto base_ptr = dynamic_cast<Instruction*>(instr->get_operand(i));
                                Value *base_addr;
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
                                if (base_addr) {
                                    if (dynamic_cast<GlobalVariable*>(base_addr)) {
                                        func_total_global_array_side_effect[func].insert(base_addr);
                                    } else if (dynamic_cast<AllocaInst*>(base_addr)) {
                                        func_total_local_array_side_effect[func].insert(base_addr);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    for (auto func : module->get_functions()) {
        if (func->get_num_basic_blocks() == 0) continue;
        for (auto callee : func_callee_map[func]) {
            std::set_union(func_total_global_var_side_effect[func].begin(), 
                            func_total_global_var_side_effect[func].end(), 
                            func_global_var_side_effect[callee].begin(), 
                            func_global_var_side_effect[callee].end(), 
                            std::inserter(func_total_global_var_side_effect[func], func_total_global_var_side_effect[func].begin()));
            std::set_union(func_total_global_array_side_effect[func].begin(), 
                            func_total_global_array_side_effect[func].end(), 
                            func_global_array_side_effect[callee].begin(), 
                            func_global_array_side_effect[callee].end(), 
                            std::inserter(func_total_global_array_side_effect[func], func_total_global_array_side_effect[func].begin()));
        }
    }
    bool changed = true;
    while (changed) {
        changed = false;
        for (auto func : module->get_functions()) {
            if (func->get_num_basic_blocks() == 0) continue;
            auto old_func_total_global_var_side_effect_size = func_total_global_var_side_effect[func].size();
            auto old_func_total_global_array_side_effect_size = func_total_global_array_side_effect[func].size();
            for (auto callee : func_callee_map[func]) {
                std::set_union(func_total_global_var_side_effect[func].begin(), 
                                func_total_global_var_side_effect[func].end(), 
                                func_total_global_var_side_effect[callee].begin(), 
                                func_total_global_var_side_effect[callee].end(), 
                                std::inserter(func_total_global_var_side_effect[func], func_total_global_var_side_effect[func].begin()));
                std::set_union(func_total_global_array_side_effect[func].begin(), 
                                func_total_global_array_side_effect[func].end(), 
                                func_total_global_array_side_effect[callee].begin(), 
                                func_total_global_array_side_effect[callee].end(), 
                                std::inserter(func_total_global_array_side_effect[func], func_total_global_array_side_effect[func].begin()));
            }
            if (old_func_total_global_var_side_effect_size < func_total_global_var_side_effect[func].size() || 
                old_func_total_global_array_side_effect_size < func_total_global_array_side_effect[func].size()) {
                changed = true;
            }
        }
    }
    return ;
}

void SideEffectAnalysis::get_func_total_var_effected_by_side_effect() {
    func_total_global_var_effected_by_side_effect = func_global_var_effected_by_side_effect;
    func_total_global_array_effected_by_side_effect = func_global_array_effected_by_side_effect;
    for (auto func : module->get_functions()) {
        if (func->get_num_basic_blocks() == 0) continue;
        for (auto bb : func->get_basic_blocks()) {
            for (auto instr : bb->get_instructions()) {
                if (instr->is_call()) {
                    auto callee = dynamic_cast<Function*>(instr->get_operand(0));
                    for (int i = 1; i < instr->get_num_operand(); i++) {
                        if (func_args_effected_by_side_effect[callee].find(i-1) != func_args_effected_by_side_effect[callee].end()) {
                            if (dynamic_cast<Argument*>(instr->get_operand(i))) {
                                func_total_local_array_effected_by_side_effect[func].insert(instr->get_operand(i));
                            } else {
                                auto base_ptr = dynamic_cast<Instruction*>(instr->get_operand(i));
                                Value *base_addr;
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
                                if (base_addr) {
                                    if (dynamic_cast<GlobalVariable*>(base_addr)) {
                                        func_total_global_array_effected_by_side_effect[func].insert(base_addr);
                                    } else if (dynamic_cast<AllocaInst*>(base_addr)) {
                                        func_total_local_array_effected_by_side_effect[func].insert(base_addr);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    for (auto func : module->get_functions()) {
        if (func->get_num_basic_blocks() == 0) continue;
        for (auto callee : func_callee_map[func]) {
            std::set_union(func_total_global_var_effected_by_side_effect[func].begin(), 
                            func_total_global_var_effected_by_side_effect[func].end(), 
                            func_global_var_effected_by_side_effect[callee].begin(), 
                            func_global_var_effected_by_side_effect[callee].end(), 
                            std::inserter(func_total_global_var_effected_by_side_effect[func], func_total_global_var_effected_by_side_effect[func].begin()));
            std::set_union(func_total_global_array_effected_by_side_effect[func].begin(), 
                            func_total_global_array_effected_by_side_effect[func].end(), 
                            func_global_array_effected_by_side_effect[callee].begin(), 
                            func_global_array_effected_by_side_effect[callee].end(), 
                            std::inserter(func_total_global_array_effected_by_side_effect[func], func_total_global_array_effected_by_side_effect[func].begin()));
        }
    }
    bool changed = true;
    while (changed) {
        changed = false;
        for (auto func : module->get_functions()) {
            if (func->get_num_basic_blocks() == 0) continue;
            auto old_func_total_global_var_effected_by_side_effect_size = func_total_global_var_effected_by_side_effect[func].size();
            auto old_func_total_global_array_effected_by_side_effect_size = func_total_global_array_effected_by_side_effect[func].size();
            for (auto callee : func_callee_map[func]) {
                std::set_union(func_total_global_var_effected_by_side_effect[func].begin(), 
                                func_total_global_var_effected_by_side_effect[func].end(), 
                                func_total_global_var_effected_by_side_effect[callee].begin(), 
                                func_total_global_var_effected_by_side_effect[callee].end(), 
                                std::inserter(func_total_global_var_effected_by_side_effect[func], func_total_global_var_effected_by_side_effect[func].begin()));
                std::set_union(func_total_global_array_effected_by_side_effect[func].begin(), 
                                func_total_global_array_effected_by_side_effect[func].end(), 
                                func_total_global_array_effected_by_side_effect[callee].begin(), 
                                func_total_global_array_effected_by_side_effect[callee].end(), 
                                std::inserter(func_total_global_array_effected_by_side_effect[func], func_total_global_array_effected_by_side_effect[func].begin()));
            }
            if (old_func_total_global_var_effected_by_side_effect_size < func_total_global_var_effected_by_side_effect[func].size() || 
                old_func_total_global_array_effected_by_side_effect_size < func_total_global_array_effected_by_side_effect[func].size()) {
                changed = true;
            }
        }
    }
    return ;
}
