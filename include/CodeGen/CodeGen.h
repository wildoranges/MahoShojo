#ifndef MHSJ_CODEGEN_H
#define MHSJ_CODEGEN_H

#include<Module.h>
#include<IR2asm.h>
#include<Value.h>
#include<Instruction.h>
#include<RegAlloc.h>

class CodeGen{
    const int int_align = 4;
    const int int_size = 4;
    const int int_p2align = 2;
    const int reg_size = 4;
    const int max_func_reg = 3;

    std::map<Value*, IR2asm::Regbase*> stack_map;
    std::vector<IR2asm::Regbase*> arg_on_stack;
    std::map<GlobalVariable *, IR2asm::label *> global_variable_table;
    std::map<Function*, std::set<GlobalVariable *>> global_variable_use;
    std::pair<std::set<int>, std::set<int>> used_reg;
    // std::map<int, std::vector<Value*>> reg2value;
    std::map<Value*, Interval*> reg_map;
    int func_no = 0;
    int bb_no = 0;
    int label_no = 0;
    int max_arg_size = 0;
    int pool_number = 0;
    int accumulate_line_num = 0;
    int temp_reg_store_num = 4;
    const std::set<int> all_free_tmp_pos = {0,1,2,3};//TODO:TOGETHER WITH temp_reg_store_num;
    int caller_saved_reg_num = 5;
    int literal_pool_threshold = 950;
    std::vector<BasicBlock*> linear_bb;
    std::map<BasicBlock*, IR2asm::label *> bb_label;
    bool have_func_call = true;
    bool long_func = false;
    bool have_temp_reg = true;
    std::map<int, std::vector<Value*>> reg2val;
    std::vector<int> to_save_reg;
    int sp_extra_ofst = 0;
    int func_param_extra_offset = 0;
    std::map<int,int> caller_saved_pos;
    std::vector<int> store_list;
    std::set<Value*> to_store_set;
    std::set<Interval*> interval_set;
    std::set<int> free_tmp_pos = {0,1,2,3};//FIXME:4 TMP REG
    std::set<int> cur_tmp_regs;
    std::map<int, IR2asm::Regbase*> tmp_regs_loc;
public:
    std::string tmp_reg_restore(Instruction* inst);
    std::string ld_tmp_regs(Instruction* inst);
    std::string push_tmp_instr_regs(Instruction* inst);
    std::string pop_tmp_instr_regs(Instruction* inst);
    void make_linear_bb(Function* fun, RegAllocDriver* driver = nullptr);
    void func_call_check(Function* fun);
    std::string make_lit_pool(bool have_br = false);
    std::string push_regs(std::vector<int> &reg_list, std::string cond = "");
    std::string pop_regs(std::vector<int> &reg_list, std::string cond = "");
    std::string global(std::string name);
    bool iszeroinit(Constant * init);
    std::string module_gen(Module* module);
    std::string global_def_gen(Module* module);
    void make_global_table(Module* module);
    std::string function_gen(Function* function, RegAllocDriver* driver = nullptr);
    int stack_space_allocation(Function* fun);
    std::string callee_reg_store(Function* fun);
    std::string callee_stack_operation_in(Function* fun, int stack_size);
    std::string arg_move(CallInst* call);
    std::string callee_arg_move(Function* fun);
    std::string callee_reg_restore(Function* fun);
    std::string callee_stack_operation_out(Function* fun, int stack_size);
    std::string caller_reg_store(Function* fun,CallInst* call);
    std::string caller_reg_restore(Function* fun, CallInst* call);
    void global_label_gen(Function* fun);
    std::string print_global_table();
    std::string bb_gen(BasicBlock* bb);
    std::string instr_gen(Instruction * inst);
    std::string phi_union(BasicBlock* bb, Instruction* br_inst);
    std::string single_data_move(IR2asm::Location* src_loc, IR2asm::Location* target_loc, IR2asm::Reg *reg_tmp, std::string cmpop = "");
    std::string data_move(std::vector<IR2asm::Location*> &src, std::vector<IR2asm::Location*> &dst, std::string cmpop = "");
    IR2asm::Reg *get_asm_reg(Value *val){
        if ((reg_map).find(val) != reg_map.end())
            return new IR2asm::Reg((reg_map).find(val)->second->reg_num);
        else exit(7);
    }
    IR2asm::constant *get_asm_const(Constant *val){if (dynamic_cast<ConstantZero*>(val)) return new IR2asm::constant(0);
                                                    else {
                                                        auto const_val = dynamic_cast<ConstantInt*>(val);
                                                        if (const_val) return new IR2asm::constant(const_val->get_value());
                                                    }}
    std::string ret_mov(CallInst* call);
    bool instr_may_need_push_stack(Instruction *instr) { return !(instr->is_ret() || instr->is_phi());}
};

#endif