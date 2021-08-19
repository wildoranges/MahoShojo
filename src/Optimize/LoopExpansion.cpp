#include "LoopExpansion.h"

void LoopExpansion::execute(){
    CFG_analyser = std::make_unique<CFG_analyse>(module);
    CFG_analyser->execute();
    find_try();
}

void LoopExpansion::find_try(){
    auto loops = CFG_analyser->get_loops();
    for (auto loop : *loops){
        if (loop->size() > 2){
            continue;
        }
        auto expand_time = loop_check(loop);
        if (expand_time == 0 || expand_time > 20){
            continue;
        }
        expand(expand_time, loop);
    }
}

int LoopExpansion::loop_check(std::vector<BasicBlock*>* loop){
    auto entry_BB = *(*loop).rbegin();
    auto body_BB = *(*loop).begin();
    if (entry_BB->get_pre_basic_blocks().size()>2){
        return 0;
    }
    if (body_BB->get_succ_basic_blocks().size()>1){
        return 0;
    }
    
    auto terminator = entry_BB->get_terminator();
    if (terminator->is_br() && terminator->get_num_operand() == 3){
        auto cond = dynamic_cast<Instruction *>(terminator->get_operand(0));
        if (cond == nullptr){
            return 0;
        }
        if (cond->is_cmp() == false){
            return 0;
        }
        else {
            auto cond_cmp = dynamic_cast<CmpInst*>(cond);
            auto cmp_op = cond_cmp->get_cmp_op();
            int init;
            int limit;
            int change = 0;
            int tmp = const_val_check(cond_cmp);
            if (tmp == 0){
                return 0;
            }
            limit = dynamic_cast<ConstantInt *>(cond_cmp->get_operand(tmp-1))->get_value();
            auto key = dynamic_cast<Instruction *>(cond_cmp->get_operand(2-tmp));
            if (tmp == 1){
                //default : cond_cmp is key op limit
                switch (cmp_op){
                case CmpInst::GE :
                    cmp_op = CmpInst::LE;
                    break;
                case CmpInst::LE :
                    cmp_op = CmpInst::GE;
                    break;
                case CmpInst::GT :
                    cmp_op = CmpInst::LT;
                    break;
                case CmpInst::LT :
                    cmp_op = CmpInst::GT;
                    break;
                default:
                    break;
                }
            }
            auto inst = key;
            if (inst == nullptr){
                return 0;
            }
            if (inst->is_phi() == false || inst->get_num_operand() > 4){
                return 0;
            }
            auto from_val1 = inst->get_operand(0);
            auto from_BB1 = dynamic_cast<BasicBlock*>(inst->get_operand(1));
            auto from_val2 = inst->get_operand(2);
            auto from_BB2 = dynamic_cast<BasicBlock*>(inst->get_operand(3));
            
            if (CFG_analyser->find_bb_loop(from_BB1) == loop){
                inst = dynamic_cast<Instruction *>(from_val1);
                auto const_init_val = dynamic_cast<ConstantInt *>(from_val2);
                if (const_init_val == nullptr){
                    return 0;
                }
                init = const_init_val->get_value();
            }
            else{
                inst = dynamic_cast<Instruction *>(from_val2);
                auto const_init_val = dynamic_cast<ConstantInt *>(from_val1);
                if (const_init_val == nullptr){
                    return 0;
                }
                init = const_init_val->get_value();
            }
            while (inst != key){
                if (inst == nullptr){
                    return 0;
                }
                if (inst->is_add()){
                    tmp = const_val_check(inst);
                    if (tmp == 0){
                        return 0;
                    }
                    auto const_val = dynamic_cast<ConstantInt*>(inst->get_operand(tmp-1));
                    change += const_val->get_value();
                    inst = dynamic_cast<Instruction *>(inst->get_operand(2-tmp));
                }
                else if (inst->is_sub()){
                    tmp = const_val_check(inst);
                    if (tmp != 2){
                        return 0;
                    }
                    auto const_val = dynamic_cast<ConstantInt*>(inst->get_operand(1));
                    change -= const_val->get_value();
                    inst = dynamic_cast<Instruction *>(inst->get_operand(0));
                }
                else {
                    return 0;
                }
            }

            int gap = init - limit;
            int time = 0;
            switch (cmp_op) {
            case CmpInst::GE :
                if (gap < 0){
                    return -1;
                }
                else{
                    if (change >= 0){
                        return 0;
                    }
                    while (gap >= 0){
                        gap += change;
                        time ++;
                    }
                    return time;
                }
                break;
            case CmpInst::LE :
                if (gap > 0){
                    return -1;
                }
                else{
                    if (change <= 0){
                        return 0;
                    }
                    while (gap <= 0){
                        gap += change;
                        time ++;
                    }
                    return time;
                }
                break;
            case CmpInst::GT :
                if (gap <= 0){
                    return -1;
                }
                else{
                    if (change >= 0){
                        return 0;
                    }
                    while (gap > 0){
                        gap += change;
                        time ++;
                    }
                    return time;
                }
                break;
            case CmpInst::LT :
                if (gap >= 0){
                    return -1;
                }
                else{
                    if (change <= 0){
                        return 0;
                    }
                    while (gap < 0){
                        gap += change;
                        time ++;
                    }
                    return time;
                }
                break;
            case CmpInst::NE :
                if (gap == 0){
                    return -1;
                }
                else{
                    if (gap%change != 0){
                        return 0;
                    }
                    return gap/change;
                }
                break;
            default:
                return 0;
                break;
            }
        }
    }
    else{
        return 0;
    }
}

int LoopExpansion::const_val_check(Instruction *inst){
    //return which one is const
    auto const_op1 = dynamic_cast<ConstantInt *>(inst->get_operand(0));
    auto const_op2 = dynamic_cast<ConstantInt *>(inst->get_operand(1));
    if (const_op1 && (const_op2 == nullptr)){
        return 1;
    }
    else if(const_op2 && (const_op1 == nullptr)){
        return 2;
    }
    else{
        return 0;
    }
}

void LoopExpansion::expand(int time, std::vector<BasicBlock *>* loop){
    auto entry_BB = *(*loop).rbegin();
    auto body_BB = *(*loop).begin();

    //remove terminator
    auto entry_terminator = entry_BB->get_terminator();
    auto next_BB = entry_terminator->get_operand(2);
    auto body_terminator = body_BB->get_terminator();
    entry_BB->get_instructions().pop_back();
    body_BB->delete_instr(body_terminator);

    //record phi information in entry BB
    std::map<Value*, Value*>old2new;
    std::map<Value*, Value*>phi2input;
    std::map<Value*, Value*>phi2old;
    std::map<Value*, Value*>phi2new;
    for (auto inst : entry_BB->get_instructions()){
        if (inst->is_phi()){
            auto from_val1 = inst->get_operand(0);
            auto from_BB1 = dynamic_cast<BasicBlock*>(inst->get_operand(1));
            auto from_val2 = inst->get_operand(2);
            auto from_BB2 = dynamic_cast<BasicBlock*>(inst->get_operand(3));
            if (CFG_analyser->find_bb_loop(from_BB1) == loop){
                phi2input[inst] = from_val2;
                phi2old[inst] = from_val1;
                phi2new[inst] = from_val1;
            }
            else{
                phi2input[inst] = from_val1;
                phi2old[inst] = from_val2;
                phi2new[inst] = from_val1;
            }
        }
        else{
            break;
        }
    }

    //copy insts in body BB
    for (auto i = 0; i < time; i++){
        for (auto inst : body_BB->get_instructions()){
            auto new_inst = inst->copy_inst(entry_BB);
            for (auto j = 0; j < new_inst->get_num_operand(); j++){
                auto operand = new_inst->get_operand(j);
                if (i == 0){
                    //first loop, initialize
                    if (phi2input[operand]!=nullptr){
                        //new_inst use input val
                        operand->remove_use(new_inst);
                        new_inst->set_operand(j,phi2input[operand]);
                    }
                }
                if (phi2new[operand]!=nullptr){
                    operand->remove_use(new_inst);
                    new_inst->set_operand(j,phi2new[operand]);
                }
                else if (old2new[operand]!=nullptr){
                    operand->remove_use(new_inst);
                    new_inst->set_operand(j,old2new[operand]);
                }
            }
            old2new[inst] = new_inst;
        }
        for (auto pair : phi2old){
            phi2new[pair.first] = old2new[pair.second];
        }
    }

    //remove unused phi and cmp
    for (auto iter_inst = entry_BB->get_instructions().begin(); iter_inst != entry_BB->get_instructions().end(); ){
        auto inst = *iter_inst;
        if (inst->is_phi()){
            //replace use with new loop val
            auto loop_val = phi2old[inst];
            auto new_val = old2new[loop_val];
            for (auto use : inst->get_use_list()){
                auto use_inst = dynamic_cast<Instruction *>(use.val_);
                if (use_inst == nullptr){
                    continue;
                }
                if (CFG_analyser->find_bb_loop(use_inst->get_parent()) != loop){
                    use_inst->set_operand(use.arg_no_, new_val);
                }
            }
            iter_inst++;
            entry_BB->delete_instr(inst);
        }
        else if (inst->is_cmp()){
            iter_inst++;
            entry_BB->delete_instr(inst);
        }
        else{
            break;
        }
    }

    entry_terminator->remove_operands(0,2);
    entry_terminator->add_operand(next_BB);
    entry_BB->get_instructions().push_back(entry_terminator);
    auto body_insts = body_BB->get_instructions();
    for (auto inst : body_insts){
        body_BB->delete_instr(inst);
    }
    body_BB->get_parent()->remove(body_BB);
}