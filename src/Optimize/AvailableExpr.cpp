//
// Created by cjb on 7/28/21.
//

#include "AvailableExpr.h"
#include <set>
#include <algorithm>

void AvailableExpr::execute() {
    for(auto func:module->get_functions()){
        if(func->get_basic_blocks().empty()){
            continue;
        }
        initial_map(func);
        compute_local_gen(func);
        //compute_local_kill(func);
        compute_global_in_out(func);
        compute_global_common_expr(func);
    }
}

bool AvailableExpr::is_valid_expr(Instruction *inst) {
    // TODO: 绑定成一条汇编指令的ir指令集合(如smul_lo和smul_hi绑定为smul)要么全部传播,要么都不传播
    return !(inst->is_void()||inst->is_call()||inst->is_phi()||inst->is_alloca()||inst->is_load()||inst->is_load_offset()||inst->is_cmp());//TODO:CHECK VALID INST
}

void AvailableExpr::compute_local_gen(Function *f) {
    //std::cerr << "local expr\n";
    auto all_bbs = f->get_basic_blocks();
    for(auto bb:all_bbs){
        //std::cerr<<bb->get_name()<<std::endl;
        std::vector<Instruction*> delete_list = {};
        auto instrs = bb->get_instructions();
        for(auto instr : instrs){
            if(is_valid_expr(instr)){
                auto res = bb_gen[bb].insert(instr);
                if(!res.second){
                    auto old_instr = bb_gen[bb].find(instr);
                    //std::cerr << "local replace " <<instr->print() << " with "<<(*old_instr)->print() << std::endl;
                    instr->replace_all_use_with(*old_instr);
                    delete_list.push_back(instr);
//                    instr_iter = instrs.erase(instr_iter);
//                    instr_iter --;
//                    instr->remove_use_of_ops();
                }else{
                    //std::cerr << "insert " << instr->print() <<" into bb_gen\n";
                    auto u_res = U.insert(instr);
//                    if(u_res.second){
//                        std::cerr << "insert " << instr->print() <<" into U\n";
//                    }
                }
                //remove_relevant_instr(instr,bb_gen[bb]);
            }
        }
        for(auto inst : delete_list){
            bb->delete_instr(inst);
        }
    }
}

//void AvailableExpr::compute_local_kill(Function *f) {
//    auto all_bbs = f->get_basic_blocks();
//    for(auto bb:all_bbs){
//        auto instrs = bb->get_instructions();
//        for(auto instr:instrs){
//            if(is_valid_expr(instr)){
//                bb_kill[bb].erase(instr);//TODO:CHECK ACC
//                insert_relevant_instr(instr,bb_kill[bb]);
//            }
//        }
//    }
//}

void AvailableExpr::compute_global_in_out(Function *f) {
    auto all_bbs = f->get_basic_blocks();
    auto entry = f->get_entry_block();
    for(auto bb:all_bbs){
        if(bb!=entry){
            bb_out[bb] = U;
        }
    }
    bb_out[entry] = std::set<Instruction*,cmp_expr>();
    bool change = true;
    int iter_cnt = 1;
    while (change){
        //std::cerr << "\n\nloop" << iter_cnt << std::endl<<std::endl;
        iter_cnt++;
        change = false;
        for(auto bb:all_bbs){
            if(bb!=entry){
                //std::cerr << "cur bb:" <<bb->get_name() <<std::endl;
                std::set<Instruction*,cmp_expr> last_tmp;
                bool is_first = true;
                for(auto pred:bb->get_pre_basic_blocks()){
                    //std::cerr << "pred bb:"<< pred->get_name()<<std::endl;
                    if(!is_first){
                        std::set<Instruction*,cmp_expr> this_tmp= {};
                        std::insert_iterator<std::set<Instruction*,cmp_expr>> it(this_tmp,this_tmp.begin());
                        std::set_intersection(last_tmp.begin(),last_tmp.end(),bb_out[pred].begin(),bb_out[pred].end(),it);
                        last_tmp = this_tmp;
                    }else{
                        is_first = false;
                        last_tmp = bb_out[pred];
                    }
                }
                //std::cerr << "intersect:\n";
//                for(auto inst:last_tmp){
//                    std::cerr << inst->print() << std::endl;
//                }
                bb_in[bb] = last_tmp;
                auto old_out_size = bb_out[bb].size();
                std::set<Instruction*,cmp_expr> tmp2 = {};
                std::insert_iterator<std::set<Instruction*,cmp_expr>> it(tmp2,tmp2.begin());
                //std::set_difference(bb_in[bb].begin(),bb_in[bb].end(),bb_kill[bb].begin(),bb_kill[bb].end(),it);
                //std::set<Instruction*,cmp_expr> tmp3 = {};
                //std::insert_iterator<std::set<Instruction*,cmp_expr>> it2(tmp3,tmp3.begin());
                std::set_union(bb_in[bb].begin(),bb_in[bb].end(),bb_gen[bb].begin(),bb_gen[bb].end(),it);
                //std::cerr << "union:\n";
//                for(auto inst:tmp2){
//                    std::cerr << inst->print() << std::endl;
//                }
                bb_out[bb] = tmp2;
                auto new_out_size = tmp2.size();
                if(old_out_size!=new_out_size){
                    change = true;
                }
            }
        }
    }
}

void AvailableExpr::initial_map(Function *f) {
    auto all_bbs = f->get_basic_blocks();
    for(auto bb:all_bbs){
        bb_in[bb] = std::set<Instruction*,cmp_expr>();
        bb_out[bb] = std::set<Instruction*,cmp_expr>();
        bb_gen[bb] = std::set<Instruction*,cmp_expr>();
        //bb_kill[bb] = std::set<Instruction*,cmp_expr>();
    }
}

//void AvailableExpr::remove_relevant_instr(Value *val, std::set<Instruction *, cmp_expr> &bb_set) {
//    for(auto instr_iter = bb_set.begin();instr_iter!=bb_set.end();instr_iter++){
//        auto cur_instr = *instr_iter;
//        for(auto opr:cur_instr->get_operands()){
//            if(opr==val){
//                instr_iter = bb_set.erase(instr_iter);
//                instr_iter --;
//                break;
//            }
//        }
//    }
//}

//void AvailableExpr::insert_relevant_instr(Value *val, std::set<Instruction *, cmp_expr> &bb_set) {
//    for(auto & map_iter : bb_gen){
//        auto cur_gen = map_iter.second;
//        for(auto instr:cur_gen){
//            for(auto opr:instr->get_operands()){
//                if(opr==val){
//                    bb_set.insert(instr);
//                    break;
//                }
//            }
//        }
//    }
//}

void AvailableExpr::compute_global_common_expr(Function *f) {
    //std::cerr << "global\n";
    std::set<Instruction*> delete_list = {};
    std::map<Instruction*,Instruction*> replace_map;
    auto all_bbs = f->get_basic_blocks();
    for(auto bb:all_bbs){
        //std::cerr << "cur bb:"<< bb->get_name() <<std::endl;

        auto instrs = bb->get_instructions();
        for(auto instr : instrs) {
            if (is_valid_expr(instr)) {
                auto common_exp = bb_in[bb].find(instr);
                if (common_exp != bb_in[bb].end()) {
                    if(*common_exp!=instr){
                        //std::cerr << "global replace " <<instr->print() << " with "<<(*common_exp)->print() << std::endl;
                        //instr->replace_all_use_with(*common_exp);
                        replace_map[instr] = *common_exp;
                        delete_list.insert(instr);
                    }
//                instr_iter = instrs.erase(instr_iter);
//                instr_iter --;
//                instr->remove_use_of_ops();
                }
            }
        }
    }
    for(auto inst : delete_list){
        auto common_exp = replace_map[inst];
        while(replace_map.find(common_exp)!=replace_map.end()){
            common_exp = replace_map[common_exp];
        }
        inst->replace_all_use_with(common_exp);
        inst->get_parent()->delete_instr(inst);
    }
}

