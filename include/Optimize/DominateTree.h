//
// Created by cjb on 7/20/21.
//

#ifndef MHSJ_DOMINATETREE_H
#define MHSJ_DOMINATETREE_H

#include <list>
#include <vector>
#include <memory>
#include <set>
#include <map>
#include "Pass.h"
#include "BasicBlock.h"
#include "Module.h"

//class DomNode;

//using DomNodePtr = std::shared_ptr<DomNode>;

class DominateTree : public Pass{
public:
    explicit DominateTree(Module* module): Pass(module){}
    void execute()final;
    void get_revserse_post_order(Function* f);
    void get_post_order(BasicBlock* bb,std::set<BasicBlock*>& visited);
    void get_bb_idom(Function* f);
    void get_bb_dom_front(Function* f);
    BasicBlock* intersect(BasicBlock* b1, BasicBlock* b2);
    const std::string get_name() const override {return name;}

private:
    std::list<BasicBlock*> reverse_post_order;
    std::map<BasicBlock*,int> bb2int;
    std::vector<BasicBlock*> doms;
    std::string name = "DominateTree";
};


class RDominateTree : public Pass{//reverse dominate tree
public:
    explicit RDominateTree(Module* module): Pass(module){}
    void execute()final;
    void get_revserse_post_order(Function* f);
    void get_post_order(BasicBlock* bb,std::set<BasicBlock*>& visited);
    void get_bb_irdom(Function* f);
    void get_bb_rdoms(Function* f);
    void get_bb_rdom_front(Function* f);
    //void clear_tmp_block();
    const std::string get_name() const override {return name;}
    BasicBlock* intersect(BasicBlock* b1, BasicBlock* b2);
private:
    //std::vector<Instruction*> ret_instr;
    BasicBlock* exit_block = nullptr;
    std::list<BasicBlock*> reverse_post_order;
    std::map<BasicBlock*,int> bb2int;
    std::vector<BasicBlock*> rdoms;
    std::string name = "RDominateTree";
};

#endif //MHSJ_DOMINATETREE_H
