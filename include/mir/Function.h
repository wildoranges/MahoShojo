#ifndef SYSY_FUNCTION_H
#define SYSY_FUNCTION_H

#include <iterator>
#include <list>
#include <cstddef>
#include <map>
#ifdef DEBUG
#include <cassert>
#endif
#include <set>

#include "User.h"
#include "Module.h"
#include "BasicBlock.h"
#include "Type.h"

class Module;
class Argument;
class BasicBlock;
class Type;
class FunctionType;

class Function : public Value
{
public:
    Function(FunctionType *ty, const std::string &name, Module *parent);
    ~Function();
    static Function *create(FunctionType *ty, const std::string &name, Module *parent);

    FunctionType *get_function_type() const;

    Type *get_return_type() const;

    void add_basic_block(BasicBlock *bb);

    unsigned get_num_of_args() const;
    unsigned get_num_basic_blocks() const;
    
    Module *get_parent() const;

    std::list<Argument *>::iterator arg_begin() { return arguments_.begin(); }
    std::list<Argument *>::iterator arg_end() { return arguments_.end(); }
    
    void remove(BasicBlock* bb);
    BasicBlock *get_entry_block() { return *basic_blocks_.begin(); }

    std::list<BasicBlock *> &get_basic_blocks() { return basic_blocks_; }
    std::list<Argument *> &get_args() { return arguments_; }
    std::vector<std::set<Value *>> &get_vreg_set(){ return vreg_set_;}
    
    bool is_declaration() { return basic_blocks_.empty(); }
    void set_unused_reg_num(std::set<int>& set){unused_reg_num_ = set;}
    std::set<int>& get_unused_reg_num(){return unused_reg_num_;}

    void set_instr_name();
    std::string print();

    void set_callee_set(std::set<Function*> callee_set){callee_set_ = callee_set;}
    void set_args_side_effect_store(std::set<int> args_has_side_effect){args_has_side_effect_ = args_has_side_effect;}
    void set_local_array_side_effect_store(std::set<Value*> local_array_has_side_effect){local_array_has_side_effect_ = local_array_has_side_effect;}
    void set_global_var_side_effect_store(std::set<Value*> global_var_has_side_effect){global_var_has_side_effect_ = global_var_has_side_effect;}
    void set_global_array_side_effect_store(std::set<Value*> global_array_has_side_effect){global_array_has_side_effect_ = global_array_has_side_effect;}
    void set_args_side_effect_load(std::set<int> args_effected_by_side_effect){args_effected_by_side_effect_ = args_effected_by_side_effect;}
    void set_local_array_side_effect_load(std::set<Value*> local_array_effected_by_side_effect){local_array_effected_by_side_effect_ = local_array_effected_by_side_effect;}
    void set_global_var_side_effect_load(std::set<Value*> global_var_effected_by_side_effect){global_var_effected_by_side_effect_ = global_var_effected_by_side_effect;}
    void set_global_array_side_effect_load(std::set<Value*> global_array_effected_by_side_effect){global_array_effected_by_side_effect_ = global_array_effected_by_side_effect;}

    std::set<Function*>& get_callee_set(){return callee_set_;}
    std::set<int>& get_args_side_effect_store(){return args_has_side_effect_;}
    std::set<Value*>& get_local_array_side_effect_store(){return local_array_has_side_effect_;}
    std::set<Value*>& get_global_var_side_effect_store(){return global_var_has_side_effect_;}
    std::set<Value*>& get_global_array_side_effect_store(){return global_array_has_side_effect_;}
    std::set<int>& get_args_side_effect_load(){return args_effected_by_side_effect_;}
    std::set<Value*>& get_local_array_side_effect_load(){return local_array_effected_by_side_effect_;}
    std::set<Value*>& get_global_var_side_effect_load(){return global_var_effected_by_side_effect_;}
    std::set<Value*>& get_global_array_side_effect_load(){return global_array_effected_by_side_effect_;}

private:
    void build_args();

private:
    std::list<BasicBlock *> basic_blocks_;    // basic blocks
    std::list<Argument *> arguments_;         // arguments
    std::vector<std::set<Value *>> vreg_set_;
    Module *parent_;
    std::set<int> unused_reg_num_;
    unsigned seq_cnt_;
    std::set<Function *> callee_set_;
    std::set<int> args_has_side_effect_, args_effected_by_side_effect_;
    std::set<Value *> local_array_has_side_effect_, global_var_has_side_effect_, global_array_has_side_effect_;
    std::set<Value *> local_array_effected_by_side_effect_, global_var_effected_by_side_effect_, global_array_effected_by_side_effect_;
    // unsigned num_args_;
    // We don't need this, all value inside function should be unnamed
    // std::map<std::string, Value*> sym_table_;   // Symbol table of args/instructions
};

// Argument of Function, does not contain actual value
class Argument : public Value
{
public:
    /// Argument constructor.
    explicit Argument(Type *ty, const std::string &name = "", Function *f = nullptr,
                    unsigned arg_no = 0)
        : Value(ty, name), parent_(f), arg_no_(arg_no) {}
    ~Argument() {}

    inline const Function *get_parent() const { return parent_; }
    inline       Function *get_parent()       { return parent_; }

    /// For example in "void foo(int a, float b)" a is 0 and b is 1.
    unsigned get_arg_no() const {
#ifdef DEBUG
        assert(parent_ && "can't get number of unparented arg");
#endif
        return arg_no_;
    }

    virtual std::string print() override ;
private:
    Function *parent_;
    unsigned arg_no_;  // argument No.
};

#endif // SYSY_FUNCTION_H
