#ifndef MHSJ_LOOPINVARIANT_H
#define MHSJ_LOOPINVARIANT_H

#include "Module.h"
#include "Pass.h"
#include <vector>
#include <map>
#include <stack>
#include <set>
#include "CFG_analyse.h"
#include <memory>

class LoopInvariant : public Pass
{
private:
    /* data */
    std::unique_ptr<CFG_analyse> CFG_analyser;
    std::vector<std::pair<BasicBlock *, std::set<Instruction *>>> invariants;
    std::string name = "LoopInvariant";

public:
    explicit LoopInvariant(Module* module): Pass(module){}
    void execute() final;
    void invariants_find(std::vector<BasicBlock *>* );
    const std::string get_name() const override {return name;}
};




#endif