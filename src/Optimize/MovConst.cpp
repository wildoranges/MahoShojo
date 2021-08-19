//
// Created by cjb on 8/14/21.
//

#include "MovConst.h"

void MovConst::execute() {
    for(auto fun:module->get_functions()){
        if(fun->get_basic_blocks().empty()){
            continue;
        }
        for(auto bb:fun->get_basic_blocks()){
            mov_const(bb);
        }
    }
}

void MovConst::mov_const(BasicBlock *bb) {
    // TODO: need support other LIR instructions
    auto &instructions = bb->get_instructions();
    for (auto iter = instructions.begin(); iter != instructions.end(); iter++){
        auto instr = *iter;
        if (instr->is_asr() || instr->is_lsl() || instr->is_lsr() || instr->is_store() || instr->is_store_offset()) {
            auto op1 = instr->get_operand(0);
            auto const_op1 = dynamic_cast<ConstantInt*>(op1);
            if (const_op1) {
                auto mov_const_instr = MovConstInst::create_mov_const(const_op1, bb);
                instructions.pop_back();
                bb->add_instruction(iter, mov_const_instr);
                (instr->get_operand(0))->remove_use(instr);
                instr->set_operand(0, mov_const_instr);
            }
        }
        if (instr->is_store_offset()) {
            auto offset = instr->get_operand(2);
            auto const_offset = dynamic_cast<ConstantInt*>(offset);
            if (const_offset) {
                auto const_offset_val = const_offset->get_value();
                if (const_offset_val >= (1<<8) || const_offset_val < 0) {
                    auto mov_const_instr = MovConstInst::create_mov_const(const_offset, bb);
                    instructions.pop_back();
                    bb->add_instruction(iter, mov_const_instr);
                    (instr->get_operand(2))->remove_use(instr);
                    instr->set_operand(2, mov_const_instr);
                }
            }
        }
        if (instr->is_load_offset()) {
            auto offset = instr->get_operand(1);
            auto const_offset = dynamic_cast<ConstantInt*>(offset);
            if (const_offset) {
                auto const_offset_val = const_offset->get_value();
                if (const_offset_val >= (1<<8) || const_offset_val < 0) {
                    auto mov_const_instr = MovConstInst::create_mov_const(const_offset, bb);
                    instructions.pop_back();
                    bb->add_instruction(iter, mov_const_instr);
                    (instr->get_operand(1))->remove_use(instr);
                    instr->set_operand(1, mov_const_instr);
                }
            }
        }
        if (instr->is_add() || instr->is_and() || instr->is_or() || instr->is_xor()) {
            auto op1 = instr->get_operand(0);
            auto op2 = instr->get_operand(1);
            auto const_op1 = dynamic_cast<ConstantInt*>(op1);
            auto const_op2 = dynamic_cast<ConstantInt*>(op2);
            if (const_op1) {
                auto const_op1_val = const_op1->get_value();
                if (const_op1_val >= (1<<8) || const_op1_val < 0) {
                    auto mov_const_instr = MovConstInst::create_mov_const(const_op1, bb);
                    instructions.pop_back();
                    bb->add_instruction(iter, mov_const_instr);
                    (instr->get_operand(0))->remove_use(instr);
                    instr->set_operand(0, mov_const_instr);
                }
            }
            if (const_op2) {
                auto const_op2_val = const_op2->get_value();
                if (const_op2_val >= (1<<8) || const_op2_val < 0) {
                    auto mov_const_instr = MovConstInst::create_mov_const(const_op2, bb);
                    instructions.pop_back();
                    bb->add_instruction(iter, mov_const_instr);
                    (instr->get_operand(1))->remove_use(instr);
                    instr->set_operand(1, mov_const_instr);
                }
            }
        }
        if (instr->is_sub()) {
            auto op1 = instr->get_operand(0);
            auto op2 = instr->get_operand(1);
            auto const_op1 = dynamic_cast<ConstantInt*>(op1);
            auto const_op2 = dynamic_cast<ConstantInt*>(op2);
            if (const_op1) {
                auto const_op1_val = const_op1->get_value();
                if (const_op1_val >= (1<<8) || const_op1_val < 0) {
                    auto mov_const_instr = MovConstInst::create_mov_const(const_op1, bb);
                    instructions.pop_back();
                    bb->add_instruction(iter, mov_const_instr);
                    (instr->get_operand(0))->remove_use(instr);
                    instr->set_operand(0, mov_const_instr);
                }
            }
            if (const_op2) {
                auto const_op2_val = const_op2->get_value();
                if (const_op2_val >= (1<<8) || const_op2_val < 0) {
                    auto mov_const_instr = MovConstInst::create_mov_const(const_op2, bb);
                    instructions.pop_back();
                    bb->add_instruction(iter, mov_const_instr);
                    (instr->get_operand(1))->remove_use(instr);
                    instr->set_operand(1, mov_const_instr);
                }
            }
        }
        if (instr->is_cmp() || instr->is_cmpbr() || instr->is_mul() || instr->is_div() || instr->is_rem() || instr->is_smmul()) {
            auto op1 = instr->get_operand(0);
            auto op2 = instr->get_operand(1);
            auto const_op1 = dynamic_cast<ConstantInt*>(op1);
            auto const_op2 = dynamic_cast<ConstantInt*>(op2);
            if (const_op1) {
                auto mov_const_instr = MovConstInst::create_mov_const(const_op1, bb);
                instructions.pop_back();
                bb->add_instruction(iter, mov_const_instr);
                (instr->get_operand(0))->remove_use(instr);
                instr->set_operand(0, mov_const_instr);
            }
            if (const_op2) {
                auto mov_const_instr = MovConstInst::create_mov_const(const_op2, bb);
                instructions.pop_back();
                bb->add_instruction(iter, mov_const_instr);
                (instr->get_operand(1))->remove_use(instr);
                instr->set_operand(1, mov_const_instr);
            }
        }
        if (instr->is_muladd() || instr->is_mulsub()) {
            auto op1 = instr->get_operand(0);
            auto op2 = instr->get_operand(1);
            auto op3 = instr->get_operand(2);
            auto const_op1 = dynamic_cast<ConstantInt*>(op1);
            auto const_op2 = dynamic_cast<ConstantInt*>(op2);
            auto const_op3 = dynamic_cast<ConstantInt*>(op3);
            if (const_op1) {
                auto mov_const_instr = MovConstInst::create_mov_const(const_op1, bb);
                instructions.pop_back();
                bb->add_instruction(iter, mov_const_instr);
                (instr->get_operand(0))->remove_use(instr);
                instr->set_operand(0, mov_const_instr);
            }
            if (const_op2) {
                auto mov_const_instr = MovConstInst::create_mov_const(const_op2, bb);
                instructions.pop_back();
                bb->add_instruction(iter, mov_const_instr);
                (instr->get_operand(1))->remove_use(instr);
                instr->set_operand(1, mov_const_instr);
            }
            if (const_op3) {
                auto mov_const_instr = MovConstInst::create_mov_const(const_op3, bb);
                instructions.pop_back();
                bb->add_instruction(iter, mov_const_instr);
                (instr->get_operand(2))->remove_use(instr);
                instr->set_operand(2, mov_const_instr);
            }
        }
        if (instr->is_asradd() || instr->is_lsladd() || instr->is_lsradd() || instr->is_asrsub() || instr->is_lslsub() || instr->is_lsrsub()) {
            auto op1 = instr->get_operand(0);
            auto op2 = instr->get_operand(1);
            auto const_op1 = dynamic_cast<ConstantInt*>(op1);
            auto const_op2 = dynamic_cast<ConstantInt*>(op2);
            if (const_op1) {
                auto mov_const_instr = MovConstInst::create_mov_const(const_op1, bb);
                instructions.pop_back();
                bb->add_instruction(iter, mov_const_instr);
                (instr->get_operand(0))->remove_use(instr);
                instr->set_operand(0, mov_const_instr);
            }
            if (const_op2) {
                auto mov_const_instr = MovConstInst::create_mov_const(const_op2, bb);
                instructions.pop_back();
                bb->add_instruction(iter, mov_const_instr);
                (instr->get_operand(1))->remove_use(instr);
                instr->set_operand(1, mov_const_instr);
            }
        }
    }
}

