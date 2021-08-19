#include "SCCP.h"
#include <queue>

void SCCP::execute() {
    for (auto func : module->get_functions()) {
        if (func->get_num_basic_blocks() == 0){
            continue;
        }
        CFGWorkList.clear();
        SSAWorkList.clear();
        CFGWorkList_mark.clear();
        Inst_mark.clear();
        lattice_value_map.clear();
        real_value.clear();
        ActiveVar active_var(module);
        active_var.execute();
        for (auto bb : func->get_basic_blocks()) {
            for (auto def : bb->get_def_var()) {
                lattice_value_map.insert({def, top});
            }
            for (auto use : bb->get_use_var()) {
                lattice_value_map.insert({use, top});
            }
        }
        auto entry_bb = func->get_entry_block();
        CFGWorkList.push_back({nullptr, entry_bb});
        CFGWorkList_mark[{nullptr, entry_bb}] = unexecuted;
        while (CFGWorkList.empty() == false || SSAWorkList.empty() == false) {
            if (CFGWorkList.empty() == false) {
                auto edge = CFGWorkList.back();
                CFGWorkList.pop_back();
                if (CFGWorkList_mark[edge] == unexecuted) {
                    CFGWorkList_mark[edge] = executed;
                    evaluate_all_phis_in_block(edge);
                    auto succ_bb = edge.second;
                    auto executed_flag = false;
                    for (auto pre_bb : succ_bb->get_pre_basic_blocks()) {
                        if (pre_bb == edge.first) continue;
                        if (CFGWorkList_mark[{pre_bb, succ_bb}] == executed) {
                            executed_flag = true;
                            break;
                        }
                    }
                    if (executed_flag == false){
                        for (auto inst : succ_bb->get_instructions()){
                            Inst_mark[inst] = 1;
                            if (inst->is_phi()){
                                continue;
                            }
                            else if (inst->is_br()){
                                if (inst->get_num_operand() == 1){
                                    CFGWorkList.push_back({succ_bb, dynamic_cast<BasicBlock *>(inst->get_operand(0))});
                                    continue;
                                }
                                evaluate_conditional(inst);
                            }
                            else {
                                evaluate_assign(inst);
                            }
                        }
                    }
                }
            }
            if (SSAWorkList.empty() == false){
                auto edge = SSAWorkList.back();
                SSAWorkList.pop_back();
                auto d = edge.first;
                auto c = edge.second;
                bool c_marked = false;
                if (Inst_mark[c] == 1){
                    c_marked = true;
                }
                else{
                    auto c_parent = c->get_parent();
                    auto d_parent = d->get_parent();
                    if (c_parent == d_parent){
                        for (auto iter_inst = d_parent->get_instructions().begin(); iter_inst != d_parent->get_instructions().end(); iter_inst++){
                            auto inst = *iter_inst;
                            if (inst == d){
                                break;
                            }
                            if (inst == c){
                                c_marked = true;
                                break;
                            }
                        }
                    }
                    else {
                        for (auto c_p_pre : c_parent->get_pre_basic_blocks()){
                            if (CFGWorkList_mark[{c_p_pre, c_parent}] == executed){
                                c_marked = true;
                                break;
                            }
                        }
                    }
                }
                if (c_marked){
                    if (c->is_phi()){
                        evaluate_phi(d, c);
                    }
                    else if (c->is_br()){
                        evaluate_conditional(c);
                    }
                    else{
                        evaluate_assign(c);
                    }
                }
            }
        }
        for (auto pair : real_value){
            auto op = pair.first;
            auto value = pair.second;
            auto inst = dynamic_cast<Instruction *>(op);
            if (inst != nullptr){
                if (lattice_value_map[inst] != bottom){
                    inst->replace_all_use_with(ConstantInt::get(value,module));
                    inst->get_parent()->delete_instr(inst);
                }
            }
        }
    }
}

void SCCP::evaluate_assign(Instruction *assign) {
    if (assign->is_cmp() || assign->is_add() || assign->is_sub() || assign->is_div() || assign->is_mul() || assign->is_rem()){
        bool all_other = true;
        for (auto y : assign->get_operands()){
            auto y_inst = dynamic_cast<Instruction *>(y);
            auto y_const = dynamic_cast<ConstantInt *>(y);
            if (y_inst != nullptr){
                SSA_add(y_inst);
                if (lattice_value_map[y_inst] == bottom){
                    all_other = false;
                }
            }
            else if (y_const != nullptr){
                real_value[y] = y_const->get_value();
            }
            else{
                //not instruction or const
                all_other = false;
            }
        }
        auto old_lattice = lattice_value_map[assign];
        if (old_lattice != bottom){
            if (all_other == false){
                lattice_value_map[assign] = bottom;
                for (auto use : assign->get_use_list()){
                    auto use_inst = dynamic_cast<Instruction *>(use.val_);
                    if (use_inst!=nullptr){
                        SSAWorkList.push_back({assign, use_inst});
                    }
                }
            }
            else {
                auto old_value = real_value[assign];
                switch (assign->get_instr_type())
                {
                case Instruction::OpID::add:
                    real_value[assign] = real_value[assign->get_operand(0)] + real_value[assign->get_operand(1)];
                    break;
                case Instruction::OpID::sub:
                    real_value[assign] = real_value[assign->get_operand(0)] - real_value[assign->get_operand(1)];
                    break;
                case Instruction::OpID::mul:
                    real_value[assign] = real_value[assign->get_operand(0)] * real_value[assign->get_operand(1)];
                    break;
                case Instruction::OpID::sdiv:
                    real_value[assign] = real_value[assign->get_operand(0)] / real_value[assign->get_operand(1)];
                    break;
                case Instruction::OpID::srem:
                    real_value[assign] = real_value[assign->get_operand(0)] % real_value[assign->get_operand(1)];
                    break;
                case Instruction::OpID::cmp:{
                        auto assign_cmp = dynamic_cast<CmpInst *>(assign);
                        switch (assign_cmp->get_cmp_op())
                        {
                        case CmpInst::CmpOp::EQ:
                            real_value[assign] = (real_value[assign->get_operand(0)] == real_value[assign->get_operand(1)]);
                            break;
                        case CmpInst::CmpOp::NE:
                            real_value[assign] = (real_value[assign->get_operand(0)] != real_value[assign->get_operand(1)]);
                            break;
                        case CmpInst::CmpOp::LE:
                            real_value[assign] = (real_value[assign->get_operand(0)] <= real_value[assign->get_operand(1)]);
                            break;
                        case CmpInst::CmpOp::LT:
                            real_value[assign] = (real_value[assign->get_operand(0)] < real_value[assign->get_operand(1)]);
                            break;
                        case CmpInst::CmpOp::GE:
                            real_value[assign] = (real_value[assign->get_operand(0)] >= real_value[assign->get_operand(1)]);
                            break;
                        case CmpInst::CmpOp::GT:
                            real_value[assign] = (real_value[assign->get_operand(0)] > real_value[assign->get_operand(1)]);
                            break;
                        default:
                            //no others
                            break;
                        }
                    }
                    break;
                default:
                    //no others
                    break;
                }
                if (old_lattice == top){
                    lattice_value_map[assign] = other;
                    for (auto use : assign->get_use_list()){
                        auto use_inst = dynamic_cast<Instruction *>(use.val_);
                        if (use_inst!=nullptr){
                            SSAWorkList.push_back({assign, use_inst});
                        }
                    }
                }
                else if (old_value != real_value[assign]){
                    lattice_value_map[assign] = bottom;
                    for (auto use : assign->get_use_list()){
                        auto use_inst = dynamic_cast<Instruction *>(use.val_);
                        if (use_inst!=nullptr){
                            SSAWorkList.push_back({assign, use_inst});
                        }
                    }
                }
            }
        }
    }
    else {
        lattice_value_map[assign] = bottom;
    }
}

void SCCP::evaluate_conditional(Instruction *br_inst) {
    auto cond_bb = br_inst->get_parent();
    auto true_bb = dynamic_cast<BasicBlock *>(br_inst->get_operand(1));
    auto false_bb = dynamic_cast<BasicBlock *>(br_inst->get_operand(2));
    auto cond = br_inst->get_operand(0);
    auto cond_inst = dynamic_cast<Instruction *>(cond);
    auto cond_const = dynamic_cast<ConstantInt *>(cond);
    if (cond_inst != nullptr){
        if (lattice_value_map[cond_inst] != bottom){
            SSA_add(cond_inst);
            if (lattice_value_map[cond_inst] == bottom){
                CFGWorkList.push_back({cond_bb, true_bb});
                CFGWorkList.push_back({cond_bb, false_bb});
            }
            else {
                if (real_value[cond] == 0){
                    CFGWorkList.push_back({cond_bb, false_bb});
                }
                else{
                    CFGWorkList.push_back({cond_bb, true_bb});
                }
            }
        }
        else {
            CFGWorkList.push_back({cond_bb, true_bb});
            CFGWorkList.push_back({cond_bb, false_bb});
        }
        
    }
    else if (cond_const != nullptr){
        if (cond_const->get_value() == 0){
            CFGWorkList.push_back({cond_bb, false_bb});
        }
        else{
            CFGWorkList.push_back({cond_bb, true_bb});
        }
    }
    else {
        CFGWorkList.push_back({cond_bb, true_bb});
        CFGWorkList.push_back({cond_bb, false_bb});
    }
}

void SCCP::evaluate_phi(Instruction *d, Instruction *p) {
    evaluate_operands(p);
    evaluate_result(p);
}

void SCCP::evaluate_all_phis_in_block(std::pair<BasicBlock*, BasicBlock*> edge) {
    auto n = edge.second;
    for (auto inst : n->get_instructions()){
        if (inst->is_phi()){
            evaluate_operands(inst);
        }
        else{
            break;
        }
    }
    for (auto inst : n->get_instructions()){
        if (inst->is_phi()){
            evaluate_result(inst);
        }
        else{
            break;
        }
    }
}

void SCCP::evaluate_operands(Instruction *phi) {
    auto x = phi;
    if (lattice_value_map[x] != bottom){
        auto x_bb = x->get_parent();
        for (auto i = 0; i < x->get_num_operand(); i+=2){
            auto y_val = x->get_operand(i);
            auto y_from_bb = dynamic_cast<BasicBlock *>(x->get_operand(i+1));
            if (CFGWorkList_mark[{y_from_bb, x_bb}] == executed){
                //c is marked
                auto y_inst = dynamic_cast<Instruction *>(y_val);
                auto y_const = dynamic_cast<ConstantInt *>(y_val);
                if (y_inst != nullptr){
                    SSA_add(y_inst);
                }
                else if (y_const == nullptr){
                    lattice_value_map[y_val] = bottom;
                }
            }
        }
    }
}

void SCCP::evaluate_result(Instruction *phi) {
    auto x = phi;
    if (lattice_value_map[x] != bottom){
        auto old_lattice = lattice_value_map[x];
        for (auto i = 0; i < x->get_num_operand(); i+=2){
            lattice_add(x, x->get_operand(i));
        }
        if (old_lattice != lattice_value_map[x]){
            for (auto use : x->get_use_list()){
                auto use_inst = dynamic_cast<Instruction *>(use.val_);
                if (use_inst != nullptr){
                    SSAWorkList.push_back({x, use_inst});
                }
            }
        }
    }
}

void SCCP::SSA_add(Instruction *y){
    std::set<Instruction *> executed_phi;
    if (y->is_phi()){
        std::queue<Instruction *> y_phi;
        y_phi.push(y);
        while (y_phi.empty() == false){
            auto phi = y_phi.front();
            executed_phi.insert(phi);
            y_phi.pop();
            for (auto i = 0; i < phi->get_num_operand(); i+=2){
                if (lattice_value_map[y] == bottom){
                    return;
                }
                
                auto phi_val = phi->get_operand(i);
                auto phi_val_inst = dynamic_cast<Instruction *>(phi_val);
                auto phi_from_bb = dynamic_cast<BasicBlock *>(phi->get_operand(i+1));
                if (CFGWorkList_mark[{phi_from_bb, y->get_parent()}] == unexecuted){
                    continue;
                }
                if (phi_val_inst == nullptr){
                    //not a instruction
                    auto phi_val_const = dynamic_cast<ConstantInt *>(phi_val);
                    if (phi_val_const == nullptr){
                        //not a const
                        lattice_value_map[y] = bottom;
                        return;
                    }
                    else {
                        lattice_add(y, phi_val_const);
                    }
                }
                else {
                    //is a instruction
                    if (phi_val_inst->is_phi()){
                        if (executed_phi.find(phi_val_inst) == executed_phi.end()){
                            y_phi.push(phi_val_inst);
                        }
                    }
                    else {
                        lattice_add(y, phi_val_inst);
                    }
                }
            }
        }
    }
}

void SCCP::lattice_add(Value *v1, Value *v2){
    //add v2 to v1
    //v2 is usually a Instruction or a ConstantInt

    auto v2_const = dynamic_cast<ConstantInt *>(v2);
    if (v2_const != nullptr){
        switch (lattice_value_map[v1])
        {
        case top:
            lattice_value_map[v1] = other;
            real_value[v1] = v2_const->get_value();
            break;
        case other:
            if (real_value[v1] != v2_const->get_value()){
                lattice_value_map[v1] = bottom;
            }
            break;
        case bottom:
            break;
        default:
            break;
        }
        return;
    }

    auto inst2 = dynamic_cast<Instruction *>(v2);
    if (inst2 == nullptr){
        lattice_value_map[v1] = bottom;
        return;
    }
    
    //v2 is Instruction with a int assign
    if (inst2->is_cmp() || inst2->is_add() || inst2->is_sub() || inst2->is_div() || inst2->is_mul() || inst2->is_rem() || inst2->is_phi() || inst2->is_or() || inst2->is_and()){
        auto l1 = lattice_value_map[v1];
        auto l2 = lattice_value_map[v2];
        if (l1 == bottom || l2 == bottom){
            lattice_value_map[v1] = bottom;
            return;
        }
        if (l2 == other){
            //check
            if (l1 == top){
                lattice_value_map[v1] = other;
                real_value[v1] = real_value[v2];
            }
            else {
                //l1 == other
                if (real_value[v1] != real_value[v2]){
                    lattice_value_map[v1] = bottom;
                }
            }
        }
    }
    else {
        //inst2 is bottom(other instructions)
        lattice_value_map[v1] = bottom;
        return;
    }
}