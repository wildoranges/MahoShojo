//
// Created by cjb on 7/23/21.
//

#ifndef MHSJ_REG_ALLOC_H
#define MHSJ_REG_ALLOC_H

#include "Value.h"
#include "Module.h"
#include "ValueGen.h"
#include <map>
#include <set>
#include <queue>


//const std::set<int> param_reg_id = {0,1,2,3};
//const std::set<int> ret_reg_id = {0};


class Interval;
class RegAlloc;


struct Range{
    Range(int f,int t):from(f),to(t){}
    int from;
    int to;
};


class Interval{
public:
    explicit Interval(Value* value):val(value){}
    int reg_num = -1;
    Value* val;
    //int assigned_reg;
    std::list<Range*> range_list;
    std::list<int> position_list;
    //Interval* parent;
    //std::list<Interval*> children;
    void add_range(int from,int to);
    void add_use_pos(int pos){position_list.push_front(pos);}
    //Interval* split(int id);
    bool covers(int id);
    bool covers(Instruction* inst);
    //bool covers(int from,int to);
    bool intersects(Interval* interval);
    void union_interval(Interval* interval);
    //Interval* child_at(int id);
};


struct cmp_interval{
    bool operator()(const Interval* a, const Interval* b) const {
        auto a_from = (*(a->range_list.begin()))->from;
        auto b_from = (*(b->range_list.begin()))->from;
        if(a_from!=b_from){
            return a_from < b_from;
        }else{
            return a->val->get_name() < b->val->get_name();
        }
    }
};

const int priority[] = {
        5,//r0
        4,//r1
        3,//r2
        2,//r3
        12,//r4
        11,//r5
        10,//r6
        9,//r7
        8,//r8
        7,//r9
        6,//r10
        -1,//r11
        1//r12
};


struct cmp_reg {
    bool operator()(const int reg1,const int reg2)const{
#ifdef DEBUG
        assert(reg1>=0&&reg1<=12&&reg2<=12&&reg2>=0&&"invalid reg id");
#endif
        return priority[reg1] > priority[reg2];
    }
};

//const std::set<int> general_reg_id = {10,9,8,7,6,5,4};
//const std::set<int> func_reg_id = {3,2,1,0,12};
const std::vector<int> all_reg_id = {0,1,2,3,4,5,6,7,8,9,10,12};

class RegAllocDriver{
public:
    explicit RegAllocDriver(Module* m):module(m){}
    void compute_reg_alloc();
    std::map<Value*, Interval*>& get_reg_alloc_in_func(Function* f){return reg_alloc[f];}
    std::list<BasicBlock*>& get_bb_order_in_func(Function* f){return bb_order[f];}
private:
    std::map<Function*, std::list<BasicBlock*>> bb_order;
    std::map<Function*,std::map<Value*,Interval*>> reg_alloc;
    Module* module;
};

/*****************Linear Scan Register Allocation*******************/

class RegAlloc{
public:
    explicit RegAlloc(Function* f):func(f){}
    //int get_reg(Value* value);
    void execute();
    std::map<Value*,Interval*>& get_reg_alloc(){return val2Inter;}
    std::list<BasicBlock*>& get_block_order(){return block_order;}
private:
    void init();
    void compute_block_order();
    void compute_bonus_and_cost();
    void number_operations();
    void build_intervals();
    void union_phi_val();
    void walk_intervals();
    void set_unused_reg_num();
    void get_dfs_order(BasicBlock* bb,std::set<BasicBlock*>& visited);
    void add_interval(Interval* interval){interval_list.insert(interval);}
    void add_reg_to_pool(Interval* inter);
    bool try_alloc_free_reg();
    std::set<int> unused_reg_id = {all_reg_id.begin(),all_reg_id.end()};
    //std::priority_queue<int,std::vector<int>,cmp_reg> remained_general_reg_id = {general_reg_id.begin(),general_reg_id.end()};
    //std::priority_queue<int,std::vector<int>,cmp_reg> remained_func_reg_id = {func_reg_id.begin(),func_reg_id.end()};
    std::set<int,cmp_reg> remained_all_reg_id = {all_reg_id.begin(),all_reg_id.end()};
    std::set<Interval *> active = {};
    Interval* current = nullptr;
    std::map<Value*, Interval*> val2Inter;
    Function* func;
    std::list<BasicBlock*> block_order={};
    std::set<Interval*,cmp_interval> interval_list;
    std::map<int,std::set<Interval*>> reg2ActInter;
    const double load_cost = 5.0;
    const double store_cost = 3.0;
    const double loop_scale = 100.0;
    const double mov_cost = 1.0;
    std::map<Value* ,double> spill_cost;
    std::map<Value* ,std::map<Value*, double>> phi_bonus;
    std::map<Value* ,std::map<int, double>> caller_arg_bonus;
    std::map<Value* ,std::map<int, double>> callee_arg_bonus;
    std::map<Value* ,double> call_bonus;
    std::map<Value* ,double> ret_bonus;
};

#endif //MHSJ_REG_ALLOC_H
