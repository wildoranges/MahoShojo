#ifndef MHSJ_ACTIVEVAR_H
#define MHSJ_ACTIVEVAR_H

#include "Pass.h"
#include "Module.h"

class ActiveVar : public Pass
{
public:
    ActiveVar(Module *module) : Pass(module) {}
    void execute() final;
    void get_def_var();
    void get_use_var();
    void get_live_in_live_out();
    const std::string get_name() const override {return name;}
private:
    Function *func_;
    std::map<BasicBlock *, std::set<Value *>> use_var, def_var, phi_use_before_def;
    std::map<BasicBlock *, std::set<Value *>> live_in, live_out;
    std::string name = "ActiveVar";
};

#endif  // MHSJ_ACTIVEVAR_H
