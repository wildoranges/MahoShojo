#ifndef MHSJ_SCCP_H
#define MHSJ_SCCP_H

#include "Pass.h"
#include "Module.h"
#include "ActiveVar.h"

enum Mark {
    unexecuted,
    executed
};

enum LatticeValue {
    top,
    bottom,
    other
};

class SCCP : public Pass
{
public:
    SCCP(Module *module) : Pass(module) {}
    void execute() final;
    void SSA_add(Instruction *);
    void lattice_add(Value *, Value *);
    void evaluate_assign(Instruction *);
    void evaluate_conditional(Instruction *);
    void evaluate_phi(Instruction *, Instruction *);
    void evaluate_all_phis_in_block(std::pair<BasicBlock*, BasicBlock*>);
    void evaluate_operands(Instruction *);
    void evaluate_result(Instruction *);
    const std::string get_name() const override {return name;}
private:
    Function *func_;
    std::vector<std::pair<BasicBlock*, BasicBlock*>> CFGWorkList;
    std::vector<std::pair<Instruction*, Instruction*>> SSAWorkList;
    std::map<std::pair<BasicBlock*, BasicBlock*>, Mark> CFGWorkList_mark;
    std::map<Instruction *, int> Inst_mark;
    std::map<Value*, LatticeValue> lattice_value_map;
    std::map<Value*, int> real_value;
    std::string name = "SCCP";
};

#endif  // MHSJ_SCCP_H
