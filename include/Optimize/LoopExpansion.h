#ifndef MHSJ_LOOPEXPANSION_H
#define MHSJ_LOOPEXPANSION_H

#include "Pass.h"
#include "CFG_analyse.h"
#include <memory>

class LoopExpansion : public Pass
{
private:
    /* data */
    std::unique_ptr<CFG_analyse> CFG_analyser;
    std::string name = "LoopExpansion";
public:
    explicit LoopExpansion(Module* module): Pass(module){}
    void execute() final;
    void find_try();
    int loop_check(std::vector<BasicBlock*>* loop);
    int const_val_check(Instruction *inst);
    void expand(int time, std::vector<BasicBlock *>* loop);
    const std::string get_name() const override {return name;}
};




#endif
