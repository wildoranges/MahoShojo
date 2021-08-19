//
// Created by cjb on 7/24/21.
//

#include "RegAlloc.h"
#include <iostream>
#include <cmath>
#include <set>

void Interval::add_range(int from, int to) {
    if(range_list.empty()){
        range_list.push_front(new Range(from, to));
        return;
    }
    auto top_range = *range_list.begin();
    if(from>=top_range->from && from<=top_range->to){
        top_range->to = to > top_range->to?to:top_range->to;
    }else if(from < top_range->from){
        if(to <= top_range->to && to>=top_range->from){
            top_range->from = from;
        }else{
            auto new_range = new Range(from,to);
            range_list.push_front(new_range);
        }
    }else{
        auto new_range = new Range(from,to);
        range_list.push_front(new_range);
    }
}

bool Interval::covers(int id){
    for(auto range:range_list){
        if(range->from<=id&&range->to>id){
            return true;
        }
    }
    return false;
}

bool Interval::covers(Instruction* inst){
    return covers(inst->get_id());
}

bool Interval::intersects(Interval *interval) {
    auto target_it = range_list.begin();
    auto with_it = interval->range_list.begin();
    while(with_it!=interval->range_list.end()&&target_it!=range_list.end()){
        auto target_range = *target_it;
        auto with_range = *with_it;
        if(target_range->to<=with_range->from){
            target_it++;
            continue;
        }else if(with_range->to<=target_range->from){
            with_it++;
            continue;
        }else{
            return true;
        }
    }
    return false;
}


struct cmp_range{
    bool operator()(const Range* a,const Range* b) const {
        return a->from > b->from;
    }
};

void Interval::union_interval(Interval *interval) {
    std::priority_queue<Range*, std::vector<Range*>, cmp_range> all_range;
    for(auto range:range_list){
        all_range.push(range);
    }
    for(auto range:interval->range_list){
        all_range.push(range);
    }
    if(all_range.empty()){
        return;
    }
    range_list.clear();
    auto cur_range = all_range.top();
    all_range.pop();
    while(!all_range.empty()){
        auto merge_range = all_range.top();
        all_range.pop();
        if(merge_range->from > cur_range->to){
            range_list.push_back(cur_range);
            cur_range = merge_range;
        }else{
            cur_range->to = cur_range->to >= merge_range->to?cur_range->to:merge_range->to;
        }
    }
    range_list.push_back(cur_range);
}


void RegAllocDriver::compute_reg_alloc() {
    for(auto func:module->get_functions()){
        if(func->get_basic_blocks().empty()){
            continue;
        }else{
#ifdef DEBUG
            std::cerr << "function " << func->get_name() << std::endl;
#endif
            auto allocator = new RegAlloc(func);
            allocator->execute();
            reg_alloc[func] = allocator->get_reg_alloc();
            bb_order[func] = allocator->get_block_order();
        }
    }
#ifdef DEBUG
    std::cerr << "finish reg alloc\n";
#endif
}

void RegAlloc::execute() {
    init();
    compute_block_order();
    number_operations();
    compute_bonus_and_cost();
    build_intervals();
    //union_phi_val();
    walk_intervals();
    set_unused_reg_num();
}

void RegAlloc::init() {
    for(auto reg_id:remained_all_reg_id){
        reg2ActInter[reg_id] = std::set<Interval*>();
    }
}

struct cmp_block_depth{
    bool operator()(BasicBlock* a,BasicBlock* b){
        return a->get_loop_depth() < b->get_loop_depth();
    }
};

//ref: https://ssw.jku.at/Research/Papers/Wimmer04Master/Wimmer04Master.pdf
void RegAlloc::compute_block_order() {
//TODO:USE LOOP INFO
//TODO:CHECK CLEAR
    std::priority_queue<BasicBlock*,std::vector<BasicBlock*>,cmp_block_depth>work_list;
    block_order.clear();
    auto entry = func->get_entry_block();
    work_list.push(entry);
    while(!work_list.empty()){
        auto bb = work_list.top();
        work_list.pop();
        block_order.push_back(bb);
        // std::cerr << "add "<<bb->get_name()<<" to block order" << std::endl;

        for(auto sux : bb->get_succ_basic_blocks()){
            sux->incoming_decrement();
            if(sux->is_incoming_zero()){
                work_list.push(sux);
            }
        }
    }
//    std::set<BasicBlock*> visited = {};
//    get_dfs_order(entry,visited);
}

void RegAlloc::get_dfs_order(BasicBlock *bb, std::set<BasicBlock *> &visited) {
    visited.insert(bb);
    block_order.push_back(bb);
    auto children = bb->get_succ_basic_blocks();
    for(auto child : children){
        auto is_visited = visited.find(child);
        if(is_visited == visited.end()){
            get_dfs_order(child,visited);
        }
    }
}

void RegAlloc::compute_bonus_and_cost() {
    int callee_arg_cnt = 0;
    for(auto arg:func->get_args()){
        if(callee_arg_cnt < 4){
            callee_arg_bonus[arg][callee_arg_cnt] += mov_cost;
            spill_cost[arg] += store_cost;
        }//TODO:FOR ARG > 4?
        callee_arg_cnt ++;
    }
    for(auto bb:block_order){
        auto loop_depth = bb->get_loop_depth();
        double scale_factor = pow(loop_scale, loop_depth);
        for(auto inst:bb->get_instructions()){
            auto ops = inst->get_operands();
            if(inst->is_phi()){
                for(auto op:ops){
                    if(dynamic_cast<BasicBlock*>(op)||dynamic_cast<Constant*>(op)) continue;
                    phi_bonus[inst][op] += mov_cost * scale_factor;
                    phi_bonus[op][inst] += mov_cost * scale_factor;
                }
            }
            if(inst->is_call()){
                int caller_arg_cnt = -1;
                for(auto op:ops){
                    if(caller_arg_cnt >= 0){
                        if(caller_arg_cnt < 4){
                            caller_arg_bonus[op][caller_arg_cnt] += mov_cost * scale_factor;
                        }
                        caller_arg_cnt ++;
                        continue;
                    }
                    else{
                        caller_arg_cnt ++;
                        continue;
                    }
                }
                if(!inst->is_void()){
                    call_bonus[inst] += mov_cost * scale_factor;
                }
            }
            if(inst->is_ret()){
                if(inst->get_num_operand()){
                    ret_bonus[inst->get_operand(0)] += mov_cost;//TODO:whether to add scale_factor?
                }
            }
            if(!inst->is_alloca()&&!inst->is_gep()){
                for(auto op:ops){
                    if(dynamic_cast<BasicBlock*>(op)||dynamic_cast<Constant*>(op)||dynamic_cast<GlobalVariable*>(op))
                        continue;
                    spill_cost[op] += load_cost * scale_factor;
                }
            }
            if(inst->is_gep()){
                for(auto op:ops){
                    if(dynamic_cast<GlobalVariable*>(op)||dynamic_cast<AllocaInst*>(op)||dynamic_cast<Constant*>(op))
                        continue;
                    spill_cost[op] += load_cost * scale_factor;
                }
            }
            if(!inst->is_void()){
                if(!inst->is_alloca()){
                    spill_cost[inst] += store_cost * scale_factor;
                }
            }
        }
    }
}

void RegAlloc::number_operations() {
    int next_id = 0;
    for(auto bb:block_order){
        auto instrs = bb->get_instructions();
        for(auto instr:instrs){
            instr->set_id(next_id);
            next_id += 2;
        }
    }
}

void RegAlloc::build_intervals() {//TODO:CHECK EMPTY BLOCK
    for(auto iter = block_order.rbegin();iter != block_order.rend();iter++)
    {
        //TODO:CONST CHECK
        auto bb = *iter;
        auto instrs = bb->get_instructions();
        int block_from = (*(instrs.begin()))->get_id();
        auto lst_instr = instrs.rbegin();
        int block_to = (*(lst_instr))->get_id() + 2;
        for(auto opr:bb->get_live_out()){//TODO:NEW
            if((!dynamic_cast<Instruction*>(opr) && !dynamic_cast<Argument*>(opr))||dynamic_cast<AllocaInst *>(opr)){
                continue;
            }
            if(val2Inter.find(opr)==val2Inter.end()){
                auto new_interval = new Interval(opr);
                val2Inter[opr] = new_interval;
                //add_interval(new_interval);
            }
            val2Inter[opr]->add_range(block_from,block_to);
        }
        for(auto instr_iter = instrs.rbegin();instr_iter!=instrs.rend();instr_iter++){
            auto instr = *instr_iter;
//            if(instr->is_phi()){
//                continue;
//            }//TODO:SAME COLOR OF PHI?
            //TODO:ADD FUN CALL?

//            if(instr->is_call()){
//                //TODO:ADD PHYSICAL REG?
//            }

            if(!instr->is_void()){
                if(dynamic_cast<AllocaInst *>(instr))continue;
                if(val2Inter.find(instr)==val2Inter.end()){//TODO:MUST BE NOT NULL?
                    auto new_interval = new Interval(instr);
                    new_interval->add_range(block_from,block_to);
                    val2Inter[instr] = new_interval;
                    //add_interval(new_interval);
                }
                auto cur_inter = val2Inter[instr];
                auto top_range = *(cur_inter->range_list.begin());
                //interval_list.erase(cur_inter);
                //val2Inter[instr]->range_list.pop();
                top_range->from = instr->get_id();
                //val2Inter[instr]->range_list.push(top_range);
                cur_inter->add_use_pos(instr->get_id());
                //interval_list.insert(cur_inter);
            }

            if(instr->is_phi()){//analyze
                continue;
            }

            for(auto opr:instr->get_operands()){//TODO:CONST ALLOC
                if((!dynamic_cast<Instruction*>(opr) && !dynamic_cast<Argument*>(opr))||dynamic_cast<AllocaInst *>(opr)){
                    continue;
                }
                if(val2Inter.find(opr)==val2Inter.end()){
                    auto new_interval = new Interval(opr);
                    val2Inter[opr] = new_interval;
                    new_interval->add_range(block_from,instr->get_id()+2);
                    new_interval->add_use_pos(instr->get_id());
                    //add_interval(new_interval);
                }
                else{
                    auto cur_inter = val2Inter[opr];
                    //interval_list.erase(cur_inter);
                    cur_inter->add_range(block_from,instr->get_id()+2);
                    cur_inter->add_use_pos(instr->get_id());
                    //interval_list.insert(cur_inter);
                }
            }
        }
    }
    for(auto pair:val2Inter){
#ifdef DEBUG
        std::cerr << "op:" <<pair.first->get_name() << std::endl;
#endif
        add_interval(pair.second);
#ifdef DEBUG
        for(auto range:pair.second->range_list){
            std::cerr << "from: " << range->from << " to: " << range->to << std::endl;
        }
#endif
    }
}

void RegAlloc::walk_intervals() {
    active.clear();
    reg2ActInter.clear();
    //inactive = {};
    //handled = {};
    for(auto current_it=interval_list.begin();current_it!=interval_list.end();current_it++){
        //auto current_it = interval_list.begin();
        current = *current_it;//TODO:CHECK WARNING
        //interval_list.erase(current_it);
        //interval_list.pop();
        auto position = (*current->range_list.begin())->from;

        std::vector<Interval *> delete_list = {};
        for(auto it : active){
            if((*(it->range_list.rbegin()))->to <= position){//TODO:CHECK equal?
                add_reg_to_pool(it);
                //handled.insert(*it);
                delete_list.push_back(it);
            }
        }

        for(auto inter:delete_list){
            active.erase(inter);
            reg2ActInter[inter->reg_num].erase(inter);
        }

        try_alloc_free_reg();
    }
}

bool RegAlloc::try_alloc_free_reg() {//TODO:spill by cost eval
    if(!remained_all_reg_id.empty()){
        double bonus = -1;
        int assigned_id = *remained_all_reg_id.begin();
        for(auto new_reg_id:remained_all_reg_id){
            double new_bonus = 0;
            for(auto pair:phi_bonus[current->val]){
                auto phi_val = pair.first;
                if(!val2Inter.count(phi_val))continue;
                if(!val2Inter[phi_val]->intersects(current)){//TODO:CHECK must be true?;
                    if(val2Inter[phi_val]->reg_num == new_reg_id){
                        new_bonus += pair.second;
                    }
                }
            }
            new_bonus += caller_arg_bonus[current->val][new_reg_id];
            new_bonus += callee_arg_bonus[current->val][new_reg_id];
            if(new_reg_id == 0){
                new_bonus += ret_bonus[current->val];
                new_bonus += call_bonus[current->val];
            }
            if(new_bonus > bonus){
                bonus = new_bonus;
                assigned_id = new_reg_id;
            }
        }
        remained_all_reg_id.erase(assigned_id);
        current->reg_num = assigned_id;
        unused_reg_id.erase(assigned_id);
        active.insert(current);
        reg2ActInter[assigned_id].insert(current);
#ifdef DEBUG
        std::cerr << "alloc reg " << current->reg_num << " for val " << current->val->get_name() << std::endl;
#endif
        return true;
    }
    else{
        std::set<int, cmp_reg> spare_reg_id = {};
        for(const auto& pair:reg2ActInter){
            bool insert_in_hole = true;
            for(auto inter:pair.second){
                insert_in_hole = insert_in_hole && (!inter->intersects(current));
            }
            if(insert_in_hole){
                spare_reg_id.insert(pair.first);
            }
        }
        if(!spare_reg_id.empty()){
            int assigned_id = *spare_reg_id.begin();
            double bonus = -1;
            for(auto new_reg_id:spare_reg_id){
                double new_bonus = 0;
                for(auto pair:phi_bonus[current->val]){
                    auto phi_val = pair.first;
                    if(!val2Inter.count(phi_val))continue;
                    if(!val2Inter[phi_val]->intersects(current)){
                        if(val2Inter[phi_val]->reg_num == new_reg_id){
                            new_bonus += pair.second;
                        }
                    }
                }
                new_bonus += caller_arg_bonus[current->val][new_reg_id];
                new_bonus += callee_arg_bonus[current->val][new_reg_id];
                if(new_reg_id == 0){
                    new_bonus += ret_bonus[current->val];
                    new_bonus += call_bonus[current->val];
                }
                if(new_bonus > bonus){
                    bonus = new_bonus;
                    assigned_id = new_reg_id;
                }
            }
            current->reg_num = assigned_id;
            unused_reg_id.erase(assigned_id);
            active.insert(current);
            reg2ActInter[assigned_id].insert(current);
#ifdef DEBUG
            std::cerr << "alloc reg " << current->reg_num << " for val " << current->val->get_name() << std::endl;
#endif
            return true;
        }
        auto spill_val = current;
        auto min_expire_val = spill_cost[current->val];
        int spilled_reg_id = -1;
        for(const auto& pair:reg2ActInter){
            double cur_expire_val = 0.0;
            for(auto inter:pair.second){
                if(inter->intersects(current)){
                    cur_expire_val += spill_cost[inter->val];
                }
            }
            if(cur_expire_val < min_expire_val){
                spilled_reg_id = pair.first;
                min_expire_val = cur_expire_val;
                spill_val = nullptr;
            }
        }
        if(spill_val==current){
            current->reg_num = -1;
            return false;
        }else{
            if(spilled_reg_id < 0){
                std::cerr << "spilled reg id is -1,something was wrong while register allocation" << std::endl;
                exit(16);
            }
            std::set<Interval *> to_spill_set;
            current->reg_num = spilled_reg_id;
            unused_reg_id.erase(spilled_reg_id);
            for(auto inter:reg2ActInter.at(spilled_reg_id)){
                if(inter->intersects(current)){
#ifdef DEBUG
                    std::cerr << "spill "<< inter->val->get_name() <<" to stack" << std::endl;
#endif
                    to_spill_set.insert(inter);
                }
            }
            for(auto spill_inter:to_spill_set){
                spill_inter->reg_num = -1;
                active.erase(spill_inter);
                reg2ActInter.at(spilled_reg_id).erase(spill_inter);
            }
            reg2ActInter.at(spilled_reg_id).insert(current);
            active.insert(current);
#ifdef DEBUG
            std::cerr << "alloc reg " << current->reg_num << " for val " << current->val->get_name() << std::endl;
#endif
            return true;
        }
    }
}

void RegAlloc::add_reg_to_pool(Interval* inter) {//TODO:FIX BUG:INTERVAL WITH HOLES
    int reg_id = inter->reg_num;
    if(reg_id<0||reg_id>12){
        return;
    }
    if(reg2ActInter[reg_id].size() <= 1){
#ifdef DEBUG
        std::cerr << "add "<<reg_id <<" to pool" << std::endl;
#endif
        remained_all_reg_id.insert(reg_id);
    }
    reg2ActInter[reg_id].erase(inter);
}

void RegAlloc::union_phi_val() {
    bool change = true;
    while(change){
        change = false;
        auto vreg_sets = func->get_vreg_set();
        for(auto& set:vreg_sets){
            std::set<Value*> to_del_set = {};
            Value* final_vreg = nullptr;
            for(auto vreg:set){
                if(val2Inter.find(vreg) == val2Inter.end())continue;
                if(final_vreg == nullptr){
                    final_vreg = vreg;
                }else{
                    auto vreg_ptr = val2Inter[vreg];
                    auto final_ptr = val2Inter[final_vreg];
                    //TODO: smarter union strategy
                    if(vreg_ptr->intersects(final_ptr))continue;
                    interval_list.erase(vreg_ptr);
                    interval_list.erase(final_ptr);
#ifdef DEBUG
                    std::cerr << "union "<<final_ptr->val->get_name()<<" with "<<vreg_ptr->val->get_name()<<std::endl;
#endif
                    final_ptr->union_interval(vreg_ptr);
#ifdef DEBUG
                    std::cerr << "after union:\n";

                    for(auto range:final_ptr->range_list){
                        std::cerr << "from: "<<range->from<<" to: "<<range->to<<std::endl;
                    }
#endif
                    val2Inter.erase(vreg);
                    val2Inter[vreg] = final_ptr;
                    interval_list.insert(final_ptr);
                    change = true;
                    to_del_set.insert(final_vreg);
                    to_del_set.insert(vreg);
                }
            }
            for(auto val:to_del_set){
                set.erase(val);
            }
        }
    }
}

void RegAlloc::set_unused_reg_num() {
    func->set_unused_reg_num(unused_reg_id);//TODO:CHECK ACC?
}