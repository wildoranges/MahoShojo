#ifndef SYSY_BASICBLOCK_H
#define SYSY_BASICBLOCK_H

#include "Value.h"
#include "Instruction.h"
#include "Module.h"
#include "Function.h"

#include <list>
#include <set>
#include <string>

class Function;
class Instruction;
class Module;

class BasicBlock : public Value
{
public:
    static BasicBlock *create(Module *m, const std::string &name ,
                            Function *parent ) {
        auto prefix = name.empty() ? "" : "label_";
        return new BasicBlock(m, prefix + name, parent);
    }

    // return parent, or null if none.
    Function *get_parent() { return parent_; }
    
    Module *get_module();

    /****************api about cfg****************/

    std::list<BasicBlock *> &get_pre_basic_blocks() { return pre_bbs_; }
    std::list<BasicBlock *> &get_succ_basic_blocks() { return succ_bbs_; }
    void add_pre_basic_block(BasicBlock *bb) { pre_bbs_.push_back(bb); }
    void add_succ_basic_block(BasicBlock *bb) { succ_bbs_.push_back(bb); }

    void remove_pre_basic_block(BasicBlock *bb) { pre_bbs_.remove(bb); }
    void remove_succ_basic_block(BasicBlock *bb) { succ_bbs_.remove(bb); }

    /****************api about cfg****************/

    /// Returns the terminator instruction if the block is well formed or null
    /// if the block is not well formed.
    const Instruction *get_terminator() const;
    Instruction *get_terminator() {
        return const_cast<Instruction *>(
            static_cast<const BasicBlock *>(this)->get_terminator());
    }
    
    void add_instruction(Instruction *instr);
    void add_instruction(std::list<Instruction *>::iterator instr_pos, Instruction *instr);
    void add_instr_begin(Instruction *instr);

    std::list<Instruction *>::iterator find_instruction(Instruction *instr);

    void delete_instr(Instruction *instr);

    bool empty() { return instr_list_.empty(); }

    int get_num_of_instr() { return instr_list_.size(); }
    std::list<Instruction *> &get_instructions() { return instr_list_; }
    
    void erase_from_parent();
    
    virtual std::string print() override;
    void set_idom(BasicBlock* bb){idom_ = bb;}
    BasicBlock* get_idom(){return idom_;}
    void add_dom_frontier(BasicBlock* bb){dom_frontier_.insert(bb);}
    void add_rdom_frontier(BasicBlock* bb){rdom_frontier_.insert(bb);}
    void clear_rdom_frontier(){rdom_frontier_.clear();}
    auto add_rdom(BasicBlock* bb){return rdoms_.insert(bb);}
    void clear_rdom(){rdoms_.clear();}
    std::set<BasicBlock *> &get_dom_frontier(){return dom_frontier_;}
    std::set<BasicBlock *> &get_rdom_frontier(){return rdom_frontier_;}
    std::set<BasicBlock *> &get_rdoms(){return rdoms_;}
    std::set<Value*>& get_use_var(){return use_var;}
    std::set<Value*>& get_def_var(){return def_var;}
    std::set<Value*>& get_live_in(){return live_in;}
    std::set<Value*>& get_live_out(){return live_out;}
    void set_use_var(std::set<Value*> use){use_var = use;}
    void set_def_var(std::set<Value*> def){def_var = def;}
    void set_live_in(std::set<Value*> in){live_in = in;}
    void set_live_out(std::set<Value*> out){live_out = out;}
    void insert_live_in(Value* in){live_in.insert(in);}
    void insert_live_out(Value* out){live_out.insert(out);}

    /*******CFG_analyse*******/
    int get_incoming_branch(){return incoming_branch;}
    void incoming_add(){incoming_branch++;}
    void incoming_decrement(){incoming_branch--;}
    bool is_incoming_zero(){return incoming_branch==0;}
    void incoming_reset(){incoming_branch = 0;}
    int get_loop_depth(){return loop_depth;}
    void loop_depth_add(){loop_depth++;}
    void loop_depth_reset(){loop_depth = 0;}
    /*******CFG_analyse*******/
private:
    explicit BasicBlock(Module *m, const std::string &name ,
                        Function *parent );
    std::list<BasicBlock *> pre_bbs_;
    std::list<BasicBlock *> succ_bbs_;
    std::list<Instruction *> instr_list_;
    Function *parent_;
    BasicBlock* idom_ = nullptr;
    std::set<BasicBlock*> dom_frontier_;
    std::set<BasicBlock*> rdom_frontier_;
    std::set<BasicBlock*> rdoms_;
    std::set<Value*> use_var;
    std::set<Value*> def_var;
    std::set<Value*> live_in;
    std::set<Value*> live_out;

    int incoming_branch = 0;
    int loop_depth = 0;
};

#endif // SYSY_BASICBLOCK_H