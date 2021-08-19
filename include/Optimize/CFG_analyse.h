#ifndef MHSJ_CFG_ANALYSE_H
#define MHSJ_CFG_ANALYSE_H

#include "Module.h"
#include "Pass.h"
#include <vector>
#include <map>
#include <stack>

/***************searching incoming forwarding branches****************/
/***************searching loops****************/


class CFG_analyse : public Pass{
private:
    std::vector<std::vector<BasicBlock *>*> loops;

    std::map<BasicBlock *,std::vector<BasicBlock *>*> bb_loop;
    std::map<std::vector<BasicBlock *>*,std::vector<BasicBlock *>*> outer_loop;

    std::map<BasicBlock*,int>color;
    std::string name = "CFG_analyse";
    


public:
    explicit CFG_analyse(Module* module): Pass(module){}
    void execute() final;
    void incoming_find(Function* func);
    void incoming_DFS(BasicBlock* BB);
    void loop_find(Function* func);
    void tarjan_DFS(BasicBlock* BB);

    BasicBlock* find_loop_entry(std::vector<BasicBlock *>* loop){return *(*loop).rbegin();};
    std::vector<BasicBlock *>* find_bb_loop(BasicBlock* BB){return bb_loop[BB];};
    std::vector<BasicBlock *>* find_outer_loop(std::vector<BasicBlock *>* loop){return outer_loop[loop];};
    std::vector<std::vector<BasicBlock *>*>* get_loops(){return &loops;};
    const std::string get_name() const override {return name;}
};





#endif