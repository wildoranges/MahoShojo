#ifndef MHSJ_DEADCODEELIMINATION_H
#define MHSJ_DEADCODEELIMINATION_H

#include "Pass.h"
#include "Module.h"
#include "DominateTree.h"
#include "SideEffectAnalysis.h"

class DeadCodeElimination : public Pass
{
public:
    DeadCodeElimination(Module *module) : Pass(module) {}
    void execute() final;
    void mark();
    void sweep();
    void remove_unmarked_bb();
    bool is_critical(Instruction *);
    bool has_side_effect(Instruction *);
    bool has_side_effect_to_cur_bb(Instruction *, BasicBlock *);
    bool has_side_effect_to_path(Instruction *, BasicBlock *);
    bool has_side_effect_to_caller(Instruction *, Function *);
    BasicBlock* get_nearest_marked_postdominator(Instruction *);
    const std::string get_name() const override {return name;}
private:
    Function *func_;
    std::map<Instruction *, bool> instr_mark;
    std::string name = "DeadCodeElimination";
};


#endif  // MHSJ_DEADCODEELIMINATION_H
