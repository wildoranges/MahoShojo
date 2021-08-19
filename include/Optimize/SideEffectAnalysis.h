#ifndef MHSJ_SIDEEFFECTANALYSIS_H
#define MHSJ_SIDEEFFECTANALYSIS_H

#include "Pass.h"
#include "Module.h"

class SideEffectAnalysis : public Pass
{
public:
    SideEffectAnalysis(Module *module) : Pass(module) {}
    void execute() final;
    void get_func_callee_map(); 
    void get_func_side_effect(Function*);
    void get_func_var_effected_by_side_effect(Function*);
    void get_func_total_side_effect();
    void get_func_total_var_effected_by_side_effect();
    const std::string get_name() const override {return name;}
private:
    std::map<Function *, std::set<Function *>> func_callee_map;
    std::map<Function *, std::set<int>> func_args_side_effect, func_args_effected_by_side_effect;
    std::map<Function *, std::set<Value *>> func_total_local_array_side_effect, func_total_local_array_effected_by_side_effect;
    std::map<Function *, std::set<Value *>> func_global_var_side_effect, func_total_global_var_side_effect;
    std::map<Function *, std::set<Value *>> func_global_array_side_effect, func_total_global_array_side_effect;
    std::map<Function *, std::set<Value *>> func_global_var_effected_by_side_effect, func_total_global_var_effected_by_side_effect;
    std::map<Function *, std::set<Value *>> func_global_array_effected_by_side_effect, func_total_global_array_effected_by_side_effect;
    std::string name = "SideEffectAnalysis";
};

#endif  // MHSJ_SIDEEFFECTANALYSIS_H
