//
// Created by cjb on 7/23/21.
//

#include "LIR.h"
#include "ConstPropagation.h"
#include <cmath>

void LIR::execute() {
    for (auto func : module->get_functions()){
        if (func->get_num_basic_blocks()>0){
            for (auto bb : func->get_basic_blocks()){
                split_gep(bb);
            }
            for (auto bb : func->get_basic_blocks()){
                load_offset(bb);
                store_offset(bb);
            }
            for (auto bb : func->get_basic_blocks()){
                //srem_const2and(bb);
                split_srem(bb);
                div_const2mul(bb);
            }
            for (auto bb : func->get_basic_blocks()){
                mul_const2shift(bb);
            }
            for (auto bb : func->get_basic_blocks()){
                remove_unused_op(bb);
            }
            ConstPropagation const_prop(module);
            const_prop.execute();
//            for (auto bb : func->get_basic_blocks()){
//                mov_const(bb);
//            }
            for (auto bb : func->get_basic_blocks()){
                // merge instr (when all optimization finished)
                merge_shift_add(bb);
                merge_shift_sub(bb);
                merge_mul_add(bb);
                merge_mul_sub(bb);
                merge_cmp_br(bb);
            }
        }
    }
}

void LIR::load_offset(BasicBlock *bb) {
    auto &instructions = bb->get_instructions();
    for (auto iter = instructions.begin(); iter != instructions.end(); iter++){
        auto instr = *iter;
        if (instr->is_load()) {
            auto load_instr = dynamic_cast<LoadInst*>(instr);
            auto ptr = load_instr->get_lval();
            auto gep_ptr = dynamic_cast<GetElementPtrInst*>(ptr);
            auto inst_ptr = dynamic_cast<Instruction*>(ptr);
            Value *base;
            Value *offset;
            if (gep_ptr) {
                base = gep_ptr;
                if (gep_ptr->get_num_operand() == 2) {
                    offset = gep_ptr->get_operand(1);
                } else if (gep_ptr->get_num_operand() == 3) {
                    offset = gep_ptr->get_operand(2);
                }
            } else if (inst_ptr) {
                if (inst_ptr->is_add()) {
                    base = inst_ptr->get_operand(0);
                    auto offset_instr = dynamic_cast<Instruction*>(inst_ptr->get_operand(1));
                    if (offset_instr->is_mul()) {
                        offset = offset_instr->get_operand(0);
                    } else if (offset_instr->is_lsl()) {
                        offset = offset_instr->get_operand(0);
                    }
                } else if (inst_ptr->is_muladd()) {
                    base = inst_ptr->get_operand(2);
                    offset = inst_ptr->get_operand(0);
                } else if (inst_ptr->is_lsladd()) {
                    base = inst_ptr->get_operand(0);
                    offset = inst_ptr->get_operand(1);
                }
            }
            if (gep_ptr || inst_ptr) {
                auto load_offset_instr = LoadOffsetInst::create_load_offset(base->get_type()->get_pointer_element_type(), base, offset, bb);
                instructions.pop_back();
                bb->add_instruction(iter, load_offset_instr);
                iter--;
                instr->replace_all_use_with(load_offset_instr);
                bb->delete_instr(instr);
            }
        }
    }
}

void LIR::store_offset(BasicBlock *bb) {
    auto &instructions = bb->get_instructions();
    for (auto iter = instructions.begin(); iter != instructions.end(); iter++){
        auto instr = *iter;
        if (instr->is_store()) {
            auto store_instr = dynamic_cast<StoreInst*>(instr);
            auto ptr = store_instr->get_lval();
            auto gep_ptr = dynamic_cast<GetElementPtrInst*>(ptr);
            auto inst_ptr = dynamic_cast<Instruction*>(ptr);
            Value *base;
            Value *offset;
            if (gep_ptr) {
                base = gep_ptr;
                if (gep_ptr->get_num_operand() == 2) {
                    offset = gep_ptr->get_operand(1);
                } else if (gep_ptr->get_num_operand() == 3) {
                    offset = gep_ptr->get_operand(2);
                }
            } else if (inst_ptr) {
                if (inst_ptr->is_add()) {
                    base = inst_ptr->get_operand(0);
                    auto offset_instr = dynamic_cast<Instruction*>(inst_ptr->get_operand(1));
                    if (offset_instr->is_mul()) {
                        offset = offset_instr->get_operand(0);
                    } else if (offset_instr->is_lsl()) {
                        offset = offset_instr->get_operand(0);
                    }
                } else if (inst_ptr->is_muladd()) {
                    base = inst_ptr->get_operand(2);
                    offset = inst_ptr->get_operand(0);
                } else if (inst_ptr->is_lsladd()) {
                    base = inst_ptr->get_operand(0);
                    offset = inst_ptr->get_operand(1);
                }
            }
            if (gep_ptr || inst_ptr) {
                auto store_offset_instr = StoreOffsetInst::create_store_offset(store_instr->get_rval(), base, offset, bb);
                instructions.pop_back();
                bb->add_instruction(iter, store_offset_instr);
                iter--;
                instr->replace_all_use_with(store_offset_instr);
                bb->delete_instr(instr);
            }
        }
    }
}

//void LIR::mov_const(BasicBlock *bb) {
//    // TODO: need support other LIR instructions
//    auto &instructions = bb->get_instructions();
//    for (auto iter = instructions.begin(); iter != instructions.end(); iter++){
//        auto instr = *iter;
//        if (instr->is_asr() || instr->is_lsl() || instr->is_lsr() || instr->is_store() || instr->is_store_offset()) {
//            auto op1 = instr->get_operand(0);
//            auto const_op1 = dynamic_cast<ConstantInt*>(op1);
//            if (const_op1) {
//                auto mov_const_instr = MovConstInst::create_mov_const(const_op1, bb);
//                instructions.pop_back();
//                bb->add_instruction(iter, mov_const_instr);
//                instr->set_operand(0, mov_const_instr);
//            }
//        }
//        if (instr->is_store_offset()) {
//            auto offset = instr->get_operand(2);
//            auto const_offset = dynamic_cast<ConstantInt*>(offset);
//            if (const_offset) {
//                auto const_offset_val = const_offset->get_value();
//                if (const_offset_val >= (1<<7) || const_offset_val < -(1<<7)) {
//                    auto mov_const_instr = MovConstInst::create_mov_const(const_offset, bb);
//                    instructions.pop_back();
//                    bb->add_instruction(iter, mov_const_instr);
//                    instr->set_operand(2, mov_const_instr);
//                }
//            }
//        }
//        if (instr->is_load_offset()) {
//            auto offset = instr->get_operand(1);
//            auto const_offset = dynamic_cast<ConstantInt*>(offset);
//            if (const_offset) {
//                auto const_offset_val = const_offset->get_value();
//                if (const_offset_val >= (1<<7) || const_offset_val < -(1<<7)) {
//                    auto mov_const_instr = MovConstInst::create_mov_const(const_offset, bb);
//                    instructions.pop_back();
//                    bb->add_instruction(iter, mov_const_instr);
//                    instr->set_operand(1, mov_const_instr);
//                }
//            }
//        }
//        if (instr->is_add()) {
//            auto op2 = instr->get_operand(1);
//            auto const_op2 = dynamic_cast<ConstantInt*>(op2);
//            if (const_op2) {
//                auto const_op2_val = const_op2->get_value();
//                if (const_op2_val >= (1<<7) || const_op2_val < -(1<<7)) {
//                    auto mov_const_instr = MovConstInst::create_mov_const(const_op2, bb);
//                    instructions.pop_back();
//                    bb->add_instruction(iter, mov_const_instr);
//                    instr->set_operand(1, mov_const_instr);
//                }
//            }
//        }
//        if (instr->is_sub()) {
//            auto op1 = instr->get_operand(0);
//            auto op2 = instr->get_operand(1);
//            auto const_op1 = dynamic_cast<ConstantInt*>(op1);
//            auto const_op2 = dynamic_cast<ConstantInt*>(op2);
//            if (const_op1) {
//                auto const_op1_val = const_op1->get_value();
//                if (const_op1_val >= (1<<7) || const_op1_val < -(1<<7)) {
//                    auto mov_const_instr = MovConstInst::create_mov_const(const_op1, bb);
//                    instructions.pop_back();
//                    bb->add_instruction(iter, mov_const_instr);
//                    instr->set_operand(0, mov_const_instr);
//                }
//            }
//            if (const_op2) {
//                auto const_op2_val = const_op2->get_value();
//                if (const_op2_val >= (1<<7) || const_op2_val < -(1<<7)) {
//                    auto mov_const_instr = MovConstInst::create_mov_const(const_op2, bb);
//                    instructions.pop_back();
//                    bb->add_instruction(iter, mov_const_instr);
//                    instr->set_operand(1, mov_const_instr);
//                }
//            }
//        }
//        if (instr->is_add() || instr->is_sub() || instr->is_and() || instr->is_or() || instr->is_xor() || instr->is_cmp() || instr->is_cmpbr() || instr->is_mul() || instr->is_div() || instr->is_rem() || instr->is_smmul() || instr->is_smul_lo() || instr->is_smul_hi()) {
//            auto op1 = instr->get_operand(0);
//            auto op2 = instr->get_operand(1);
//            auto const_op1 = dynamic_cast<ConstantInt*>(op1);
//            auto const_op2 = dynamic_cast<ConstantInt*>(op2);
//            if (const_op1) {
//                auto mov_const_instr = MovConstInst::create_mov_const(const_op1, bb);
//                instructions.pop_back();
//                bb->add_instruction(iter, mov_const_instr);
//                instr->set_operand(0, mov_const_instr);
//            }
//            if (const_op2) {
//                auto mov_const_instr = MovConstInst::create_mov_const(const_op2, bb);
//                instructions.pop_back();
//                bb->add_instruction(iter, mov_const_instr);
//                instr->set_operand(1, mov_const_instr);
//            }
//        }
//    }
//}

void LIR::merge_cmp_br(BasicBlock* bb) { // FIXME: may have bugs
    auto terminator = bb->get_terminator();
    if (terminator->is_br()){
        auto br = dynamic_cast<BranchInst *>(terminator);
        if (br->is_cond_br()){
            auto inst = dynamic_cast<Instruction *>(br->get_operand(0));

            if (inst && inst->is_cmp()) {
                auto br_operands = br->get_operands();
                auto inst_cmp = dynamic_cast<CmpInst *>(inst);
                auto cmp_ops = inst_cmp->get_operands();
                auto cmp_op = inst_cmp->get_cmp_op();
                auto true_bb = dynamic_cast<BasicBlock* >(br_operands[1]);
                auto false_bb = dynamic_cast<BasicBlock* >(br_operands[2]);
                true_bb->remove_pre_basic_block(bb);
                false_bb->remove_pre_basic_block(bb);
                bb->remove_succ_basic_block(true_bb);
                bb->remove_succ_basic_block(false_bb);
                auto cmp_br = CmpBrInst::create_cmpbr(cmp_op,cmp_ops[0],cmp_ops[1],
                                                    true_bb,false_bb,
                                                    bb,module);
                bb->delete_instr(br);
            }
        }
    }
}

void LIR::merge_mul_add(BasicBlock* bb) {
    auto &instructions = bb->get_instructions();
    for (auto iter = instructions.begin();iter != instructions.end();iter++){
        auto inst_add = *iter;
        if (inst_add->is_add()){
            auto op1 = inst_add->get_operand(0);
            auto op_ins1 = dynamic_cast<Instruction *>(op1);
            auto op2 = inst_add->get_operand(1);
            auto op_ins2 = dynamic_cast<Instruction *>(op2);
            if (op_ins1!=nullptr){
                if (op_ins1->is_mul() && op_ins1->get_parent() == bb && op_ins1->get_use_list().size() == 1){
                    auto mul_add = MulAddInst::create_muladd(op_ins1->get_operand(0),op_ins1->get_operand(1),op2,bb,module);
                    instructions.pop_back();
                    bb->add_instruction(iter,mul_add);
                    bb->delete_instr(op_ins1);
                    iter--;
                    inst_add->replace_all_use_with(mul_add);
                    bb->delete_instr(inst_add);
                    continue;
                }
            }
            if (op_ins2!=nullptr){
                if (op_ins2->is_mul() && op_ins2->get_parent() == bb && op_ins2->get_use_list().size() == 1){
                    auto mul_add = MulAddInst::create_muladd(op_ins2->get_operand(0),op_ins2->get_operand(1),op1,bb,module);
                    instructions.pop_back();
                    bb->add_instruction(iter,mul_add);
                    bb->delete_instr(op_ins2);
                    iter--;
                    inst_add->replace_all_use_with(mul_add);
                    bb->delete_instr(inst_add);
                    continue;
                }
            }
        }
    }
}

void LIR::merge_mul_sub(BasicBlock* bb) {
    auto &instructions = bb->get_instructions();
    for (auto iter = instructions.begin();iter != instructions.end();iter++){
        auto inst_sub = *iter;
        if (inst_sub->is_sub()){
            auto op1 = inst_sub->get_operand(0);
            auto op2 = inst_sub->get_operand(1);
            auto op_ins2 = dynamic_cast<Instruction *>(op2);
            if (op_ins2!=nullptr){
                if (op_ins2->is_mul() && op_ins2->get_parent() == bb && op_ins2->get_use_list().size() == 1){
                    auto mul_sub = MulSubInst::create_mulsub(op_ins2->get_operand(0),op_ins2->get_operand(1),op1,bb,module);
                    instructions.pop_back();
                    bb->add_instruction(iter,mul_sub);
                    bb->delete_instr(op_ins2);
                    iter--;
                    inst_sub->replace_all_use_with(mul_sub);
                    bb->delete_instr(inst_sub);
                    continue;
                }
            }
        }
    }
}

void LIR::merge_shift_add(BasicBlock* bb) {
    auto &instructions = bb->get_instructions();
    for (auto iter = instructions.begin();iter != instructions.end();iter++){
        auto inst_add = *iter;
        if (inst_add->is_add()){
            auto op1 = inst_add->get_operand(0);
            auto op_ins1 = dynamic_cast<Instruction *>(op1);
            auto op2 = inst_add->get_operand(1);
            auto op_ins2 = dynamic_cast<Instruction *>(op2);
            if (op_ins1!=nullptr){
                if ((op_ins1->is_lsl() || op_ins1->is_lsr() || op_ins1->is_asr()) && op_ins1->get_parent() == bb && op_ins1->get_use_list().size() == 1){
                    Instruction *shift_add;
                    if (op_ins1->is_lsl()) {
                        shift_add = ShiftBinaryInst::create_lsladd(op2,op_ins1->get_operand(0),op_ins1->get_operand(1),bb,module);
                    } else if (op_ins1->is_lsr()) {
                        shift_add = ShiftBinaryInst::create_lsradd(op2,op_ins1->get_operand(0),op_ins1->get_operand(1),bb,module);
                    } else if (op_ins1->is_asr()) {
                        shift_add = ShiftBinaryInst::create_asradd(op2,op_ins1->get_operand(0),op_ins1->get_operand(1),bb,module);
                    }
                    instructions.pop_back();
                    bb->add_instruction(iter,shift_add);
                    bb->delete_instr(op_ins1);
                    iter--;
                    inst_add->replace_all_use_with(shift_add);
                    bb->delete_instr(inst_add);
                    continue;
                }
            }
            if (op_ins2!=nullptr){
                if ((op_ins2->is_lsl() || op_ins2->is_lsr() || op_ins2->is_asr()) && op_ins2->get_parent() == bb && op_ins2->get_use_list().size() == 1){
                    Instruction *shift_add;
                    if (op_ins2->is_lsl()) {
                        shift_add = ShiftBinaryInst::create_lsladd(op1,op_ins2->get_operand(0),op_ins2->get_operand(1),bb,module);
                    } else if (op_ins2->is_lsr()) {
                        shift_add = ShiftBinaryInst::create_lsradd(op1,op_ins2->get_operand(0),op_ins2->get_operand(1),bb,module);
                    } else if (op_ins2->is_asr()) {
                        shift_add = ShiftBinaryInst::create_asradd(op1,op_ins2->get_operand(0),op_ins2->get_operand(1),bb,module);
                    }
                    instructions.pop_back();
                    bb->add_instruction(iter,shift_add);
                    bb->delete_instr(op_ins2);
                    iter--;
                    inst_add->replace_all_use_with(shift_add);
                    bb->delete_instr(inst_add);
                    continue;
                }
            }
        }
    }
}

void LIR::merge_shift_sub(BasicBlock* bb) {
    auto &instructions = bb->get_instructions();
    for (auto iter = instructions.begin();iter != instructions.end();iter++){
        auto inst_sub = *iter;
        if (inst_sub->is_sub()){
            auto op1 = inst_sub->get_operand(0);
            auto op2 = inst_sub->get_operand(1);
            auto op_ins2 = dynamic_cast<Instruction *>(op2);
            if (op_ins2!=nullptr){
                if ((op_ins2->is_lsl() || op_ins2->is_lsr() || op_ins2->is_asr()) && op_ins2->get_parent() == bb && op_ins2->get_use_list().size() == 1){
                    Instruction *shift_sub;
                    if (op_ins2->is_lsl()) {
                        shift_sub = ShiftBinaryInst::create_lslsub(op1,op_ins2->get_operand(0),op_ins2->get_operand(1),bb,module);
                    } else if (op_ins2->is_lsr()) {
                        shift_sub = ShiftBinaryInst::create_lsrsub(op1,op_ins2->get_operand(0),op_ins2->get_operand(1),bb,module);
                    } else if (op_ins2->is_asr()) {
                        shift_sub = ShiftBinaryInst::create_asrsub(op1,op_ins2->get_operand(0),op_ins2->get_operand(1),bb,module);
                    }
                    instructions.pop_back();
                    bb->add_instruction(iter,shift_sub);
                    bb->delete_instr(op_ins2);
                    iter--;
                    inst_sub->replace_all_use_with(shift_sub);
                    bb->delete_instr(inst_sub);
                    continue;
                }
            }
        }
    }
}

void LIR::split_gep(BasicBlock* bb) {
    auto &instructions = bb->get_instructions();
    for (auto iter = instructions.begin(); iter != instructions.end(); iter++) {
        auto inst_gep = *iter;
        if (inst_gep->is_gep()) {
            int offset_op_num;
            if (inst_gep->get_num_operand() == 2) {
                offset_op_num = 1;
            } else if (inst_gep->get_num_operand() == 3) {
                offset_op_num = 2;
            }
            auto size = ConstantInt::get(inst_gep->get_type()->get_pointer_element_type()->get_size(), module);
            auto offset = inst_gep->get_operand(offset_op_num);
            inst_gep->remove_operands(offset_op_num, offset_op_num);
            inst_gep->add_operand(ConstantInt::get(0, module));
            auto real_offset = BinaryInst::create_mul(offset, size, bb, module);
            bb->add_instruction(++iter, instructions.back());
            instructions.pop_back();
            auto real_ptr = BinaryInst::create_add(inst_gep, real_offset, bb, module);
            bb->add_instruction(iter--, instructions.back());
            instructions.pop_back();
            inst_gep->remove_use(real_ptr);
            inst_gep->replace_all_use_with(real_ptr);
            inst_gep->get_use_list().clear();
            inst_gep->add_use(real_ptr);
        }
    }
}

void LIR::split_srem(BasicBlock* bb) {
    auto &instructions = bb->get_instructions();
    for (auto iter = instructions.begin();iter != instructions.end();iter++){
        auto inst_rem = *iter;
        if (inst_rem->is_rem()){
            auto op1 = inst_rem->get_operand(0);
            auto op2 = inst_rem->get_operand(1);
            if (op1 == op2) continue;
            if (dynamic_cast<ConstantInt*>(op2) && dynamic_cast<ConstantInt*>(op2)->get_value() == 1) continue;
            auto inst_div = BinaryInst::create_sdiv(op1,op2,bb,module);
            instructions.pop_back();
            auto inst_mul = BinaryInst::create_mul(inst_div,op2,bb,module);
            instructions.pop_back();
            auto inst_sub = BinaryInst::create_sub(op1,inst_mul,bb,module);
            instructions.pop_back();
            bb->add_instruction(iter,inst_div);
            bb->add_instruction(iter,inst_mul);
            bb->add_instruction(iter,inst_sub);
            inst_rem->replace_all_use_with(inst_sub);
            iter--;
            bb->delete_instr(inst_rem);
        }
    }
}

bool is_power_of_two(int x) {
    // First x in the below expression is for the case when x is 0
    return x && (!(x & (x - 1)));
}

void LIR::mul_const2shift(BasicBlock* bb) {
    auto &instructions = bb->get_instructions();
    for (auto iter = instructions.begin(); iter != instructions.end(); iter++) {
        auto instruction = *iter;
        if (instruction->is_mul()) {
            auto op1 = instruction->get_operand(0);
            auto op2 = instruction->get_operand(1);
            auto op_const2 = dynamic_cast<ConstantInt*>(op2);
            if (op_const2 != nullptr) {
                if (is_power_of_two(op_const2->get_value())) {
                    if (op_const2->get_value() == 0 || op_const2->get_value() == 1) continue;
                    int k = (int)(ceil(std::log2(op_const2->get_value())))%32;
                    iter++;
                    auto lsl = BinaryInst::create_lsl(op1, ConstantInt::get(k, module), bb, module);
                    bb->add_instruction(iter, instructions.back());
                    instructions.pop_back();
                    instruction->replace_all_use_with(lsl);
                    bb->delete_instr(instruction);
                    iter--;
                }
            }
        }
    }
}

void LIR::srem_const2and(BasicBlock *bb) {
    auto &instructions = bb->get_instructions();
    for (auto iter = instructions.begin(); iter != instructions.end(); iter++) {
        auto instruction = *iter;
        if (instruction->is_rem()) {
            auto op1 = instruction->get_operand(0);
            auto op2 = instruction->get_operand(1);
            auto op_const2 = dynamic_cast<ConstantInt*>(op2);
            if (op_const2 != nullptr) {
                if (is_power_of_two(op_const2->get_value())) {
                    if (op_const2->get_value() == 1) continue;
                    int k = op_const2->get_value() - 1;
                    iter++;
                    auto land = BinaryInst::create_and(op1, ConstantInt::get(k, module), bb, module);
                    bb->add_instruction(iter, instructions.back());
                    instructions.pop_back();
                    instruction->replace_all_use_with(land);
                    bb->delete_instr(instruction);
                    iter--;
                }
            }
        }
    }
}

void LIR::div_const2mul(BasicBlock* bb) {
    // FIXME: may have bugs, need many tests
    auto &instructions = bb->get_instructions();
    for (auto iter = instructions.begin(); iter != instructions.end(); iter++) {
        auto instruction = *iter;
        if (instruction->is_div()) {
            auto op1 = instruction->get_operand(0);
            auto op2 = instruction->get_operand(1);
            auto op_const2 = dynamic_cast<ConstantInt*>(op2);
            if (op_const2 != nullptr) {
                auto divisor = op_const2->get_value();
                if (divisor == 0) {
                    std::cerr<<"divided by zero!"<<std::endl;
                    exit(-1);
                } else if (op_const2->get_value() == 1) {
                    continue;
                } else if (op_const2->get_value() == 2) {
                    iter++;
                    auto lsr = BinaryInst::create_lsr(op1, ConstantInt::get(31, module), bb, module);
                    bb->add_instruction(iter, instructions.back());
                    instructions.pop_back();
                    auto add = BinaryInst::create_add(op1, lsr, bb, module);
                    bb->add_instruction(iter, instructions.back());
                    instructions.pop_back();
                    auto asr = BinaryInst::create_asr(add, ConstantInt::get(1, module), bb, module);
                    bb->add_instruction(iter, instructions.back());
                    instructions.pop_back();
                    instruction->replace_all_use_with(asr);
                    bb->delete_instr(instruction);
                    iter--;
                } else if (is_power_of_two(op_const2->get_value())) {
                    int k = (int)(ceil(std::log2(op_const2->get_value())))%32;
                    iter++;
                    auto asr_1 = BinaryInst::create_asr(op1, ConstantInt::get(31, module), bb, module);
                    bb->add_instruction(iter, instructions.back());
                    instructions.pop_back();
                    auto lsr = BinaryInst::create_lsr(asr_1, ConstantInt::get(32 - k, module), bb, module);
                    bb->add_instruction(iter, instructions.back());
                    instructions.pop_back();
                    auto add = BinaryInst::create_add(op1, lsr, bb, module);
                    bb->add_instruction(iter, instructions.back());
                    instructions.pop_back();
                    auto asr_2 = BinaryInst::create_asr(add, ConstantInt::get(k, module), bb, module);
                    bb->add_instruction(iter, instructions.back());
                    instructions.pop_back();
                    instruction->replace_all_use_with(asr_2);
                    bb->delete_instr(instruction);
                    iter--;
                } else {
                    int abs_divisor = (divisor > 0) ? divisor : -divisor;
                    int c = 31 + floor(log2(abs_divisor));
                    long long L = pow(2, c);
                    int B = (divisor > 0) ? (floor(L / abs_divisor) + 1) : - (floor(L / abs_divisor) + 1);
                    iter++;
                    auto smmul = BinaryInst::create_smmul(op1, ConstantInt::get(B, module), bb, module);
                    bb->add_instruction(iter, instructions.back());
                    instructions.pop_back();
                    auto asr = BinaryInst::create_asr(smmul, ConstantInt::get(c - 32, module), bb, module);
                    bb->add_instruction(iter, instructions.back());
                    instructions.pop_back();
                    auto lsr = BinaryInst::create_lsr(smmul, ConstantInt::get(31, module), bb, module);
                    bb->add_instruction(iter, instructions.back());
                    instructions.pop_back();
                    auto add = BinaryInst::create_add(asr, lsr, bb, module);
                    bb->add_instruction(iter, instructions.back());
                    instructions.pop_back();
                    instruction->replace_all_use_with(add);
                    bb->delete_instr(instruction);
                    iter--;
                }
            }
        }
    }
}

void LIR::remove_unused_op(BasicBlock* bb) {
    // TODO: x%1, x%x; x+0; x-0, x-x; x*0, x*1; x/1, 0/x, x/x; x or x, x or 0; x and x, x and 0; x xor x, x xor 0;
    // TODO: x asr 0, 0 asr x; x lsl 0, 0 lsl x; x lsr 0, 0 lsr x; and so on
    std::vector<Instruction*> unused_instr_list;
    for (auto instr : bb->get_instructions()) {
        auto instr_type = instr->get_instr_type();
        switch (instr_type)
        {
//        case Instruction::srem: {
//            auto op1 = instr->get_operand(0);
//            auto op2 = instr->get_operand(1);
//            auto const_op2 = dynamic_cast<ConstantInt*>(op2);
//            if (op1 == op2) {
//                instr->replace_all_use_with(ConstantInt::get(0, module));
//                unused_instr_list.push_back(instr);
//            } else if (const_op2) {
//                if (const_op2->get_value() == 1) {
//                    instr->replace_all_use_with(ConstantInt::get(0, module));
//                    unused_instr_list.push_back(instr);
//                }
//            }
//        }
//            break;
        case Instruction::add: {
            auto op1 = instr->get_operand(0);
            auto op2 = instr->get_operand(1);
            auto const_op1 = dynamic_cast<ConstantInt*>(op1);
            auto const_op2 = dynamic_cast<ConstantInt*>(op2);
            if (const_op1) {
                if (const_op1->get_value() == 0) {
                    instr->replace_all_use_with(op2);
                    unused_instr_list.push_back(instr);
                }
            } else if (const_op2) {
                if (const_op2->get_value() == 0) {
                    instr->replace_all_use_with(op1);
                    unused_instr_list.push_back(instr);
                }
            }
        }
            break;
        case Instruction::sub: {
            auto op1 = instr->get_operand(0);
            auto op2 = instr->get_operand(1);
            auto const_op1 = dynamic_cast<ConstantInt*>(op1);
            auto const_op2 = dynamic_cast<ConstantInt*>(op2);
            if (const_op1) {
                if (const_op1->get_value() == 0) {
                    // use rsb instruction
                }
            } else if (const_op2) {
                if (const_op2->get_value() == 0) {
                    instr->replace_all_use_with(op1);
                    unused_instr_list.push_back(instr);
                }
            } else {
                if (op1 == op2) {
                    instr->replace_all_use_with(ConstantInt::get(0, module));
                    unused_instr_list.push_back(instr);
                }
            }
        }
            break;
        case Instruction::mul: {
            auto op1 = instr->get_operand(0);
            auto op2 = instr->get_operand(1);
            auto const_op1 = dynamic_cast<ConstantInt*>(op1);
            auto const_op2 = dynamic_cast<ConstantInt*>(op2);
            if (const_op1) {
                if (const_op1->get_value() == 0) {
                    instr->replace_all_use_with(ConstantInt::get(0, module));
                    unused_instr_list.push_back(instr);
                } else if (const_op1->get_value() == 1) {
                    instr->replace_all_use_with(op2);
                    unused_instr_list.push_back(instr);
                }
            } else if (const_op2) {
                if (const_op2->get_value() == 0) {
                    instr->replace_all_use_with(ConstantInt::get(0, module));
                    unused_instr_list.push_back(instr);
                } else if (const_op2->get_value() == 1) {
                    instr->replace_all_use_with(op1);
                    unused_instr_list.push_back(instr);
                }
            }
        }
            break;
        case Instruction::sdiv: {
            auto op1 = instr->get_operand(0);
            auto op2 = instr->get_operand(1);
            auto const_op1 = dynamic_cast<ConstantInt*>(op1);
            auto const_op2 = dynamic_cast<ConstantInt*>(op2);
            if (const_op1) {
                if (const_op1->get_value() == 0) {
                    instr->replace_all_use_with(ConstantInt::get(0, module));
                    unused_instr_list.push_back(instr);
                }
            } else if (const_op2) {
                if (const_op2->get_value() == 0) {
                    std::cerr<<"divided by zero!"<<std::endl;
                    exit(-1);
                } else if (const_op2->get_value() == 1) {
                    instr->replace_all_use_with(op1);
                    unused_instr_list.push_back(instr);
                }
            } else {
                if (op1 == op2) {
                    instr->replace_all_use_with(ConstantInt::get(1, module));
                    unused_instr_list.push_back(instr);
                }
            }
        }
            break;
        case Instruction::lor: {
            auto op1 = instr->get_operand(0);
            auto op2 = instr->get_operand(1);
            auto const_op1 = dynamic_cast<ConstantInt*>(op1);
            auto const_op2 = dynamic_cast<ConstantInt*>(op2);
            if (const_op1) {
                if (const_op1->get_value() == 0) {
                    instr->replace_all_use_with(op2);
                    unused_instr_list.push_back(instr);
                }
            } else if (const_op2) {
                if (const_op2->get_value() == 0) {
                    instr->replace_all_use_with(op1);
                    unused_instr_list.push_back(instr);
                }
            } else {
                if (op1 == op2) {
                    instr->replace_all_use_with(op1);
                    unused_instr_list.push_back(instr);
                }
            }
        }
            break;
        case Instruction::land: {
            auto op1 = instr->get_operand(0);
            auto op2 = instr->get_operand(1);
            auto const_op1 = dynamic_cast<ConstantInt*>(op1);
            auto const_op2 = dynamic_cast<ConstantInt*>(op2);
            if (const_op1) {
                if (const_op1->get_value() == 0) {
                    instr->replace_all_use_with(ConstantInt::get(0, module));
                    unused_instr_list.push_back(instr);
                }
            } else if (const_op2) {
                if (const_op2->get_value() == 0) {
                    instr->replace_all_use_with(ConstantInt::get(0, module));
                    unused_instr_list.push_back(instr);
                }
            } else {
                if (op1 == op2) {
                    instr->replace_all_use_with(op1);
                    unused_instr_list.push_back(instr);
                }
            }
        }
            break;
        case Instruction::lxor: {
            auto op1 = instr->get_operand(0);
            auto op2 = instr->get_operand(1);
            auto const_op1 = dynamic_cast<ConstantInt*>(op1);
            auto const_op2 = dynamic_cast<ConstantInt*>(op2);
            if (const_op1) {
                if (const_op1->get_value() == 0) {
                    instr->replace_all_use_with(op2);
                    unused_instr_list.push_back(instr);
                }
            } else if (const_op2) {
                if (const_op2->get_value() == 0) {
                    instr->replace_all_use_with(op1);
                    unused_instr_list.push_back(instr);
                }
            } else {
                if (op1 == op2) {
                    instr->replace_all_use_with(ConstantInt::get(0, module));
                    unused_instr_list.push_back(instr);
                }
            }
        }
            break;
        case Instruction::asr: {
            auto op1 = instr->get_operand(0);
            auto op2 = instr->get_operand(1);
            auto const_op1 = dynamic_cast<ConstantInt*>(op1);
            auto const_op2 = dynamic_cast<ConstantInt*>(op2);
            if (const_op1) {
                if (const_op1->get_value() == 0) {
                    instr->replace_all_use_with(ConstantInt::get(0, module));
                    unused_instr_list.push_back(instr);
                }
            } else if (const_op2) {
                if (const_op2->get_value()%32 == 0) {
                    instr->replace_all_use_with(op1);
                    unused_instr_list.push_back(instr);
                }
            }
        }
            break;
        case Instruction::lsl: {
            auto op1 = instr->get_operand(0);
            auto op2 = instr->get_operand(1);
            auto const_op1 = dynamic_cast<ConstantInt*>(op1);
            auto const_op2 = dynamic_cast<ConstantInt*>(op2);
            if (const_op1) {
                if (const_op1->get_value() == 0) {
                    instr->replace_all_use_with(ConstantInt::get(0, module));
                    unused_instr_list.push_back(instr);
                }
            } else if (const_op2) {
                if (const_op2->get_value()%32 == 0) {
                    instr->replace_all_use_with(op1);
                    unused_instr_list.push_back(instr);
                }
            }
        }
            break;
        case Instruction::lsr: {
            auto op1 = instr->get_operand(0);
            auto op2 = instr->get_operand(1);
            auto const_op1 = dynamic_cast<ConstantInt*>(op1);
            auto const_op2 = dynamic_cast<ConstantInt*>(op2);
            if (const_op1) {
                if (const_op1->get_value() == 0) {
                    instr->replace_all_use_with(ConstantInt::get(0, module));
                    unused_instr_list.push_back(instr);
                }
            } else if (const_op2) {
                if (const_op2->get_value()%32 == 0) {
                    instr->replace_all_use_with(op1);
                    unused_instr_list.push_back(instr);
                }
            }
        }
            break;
        default:
            break;
        }
    }
    for (auto unused_instr : unused_instr_list) {
        bb->delete_instr(unused_instr);
    }
}
