//
// Created by cjb on 7/23/21.
//

#ifndef MHSJS_LIR_H
#define MHSJS_LIR_H

#include "Module.h"
#include "Pass.h"
#include "ConstPropagation.h"

class LIR:public Pass
{
public:
    explicit LIR(Module* module): Pass(module){}
    void execute() final;
    void merge_cmp_br(BasicBlock* bb);
    void merge_mul_add(BasicBlock* bb);
    void merge_mul_sub(BasicBlock* bb);
    void merge_shift_add(BasicBlock* bb);
    void merge_shift_sub(BasicBlock* bb);
    void load_offset(BasicBlock *bb);
    void store_offset(BasicBlock *bb);
    //void mov_const(BasicBlock *bb);
    void split_gep(BasicBlock* bb);
    void split_srem(BasicBlock* bb);
    void mul_const2shift(BasicBlock* bb);
    void div_const2mul(BasicBlock* bb);
    void srem_const2and(BasicBlock* bb);
    void remove_unused_op(BasicBlock* bb);
    const std::string get_name() const override {return name;}
private:
    std::string name = "LIR";
};

#endif //MHSJS_LIR_H
