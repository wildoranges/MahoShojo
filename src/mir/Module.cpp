#include "Module.h"

Module::Module(std::string name) 
    : module_name_(name)
{
    void_ty_ = new Type(Type::VoidTyID, this);
    label_ty_ = new Type(Type::LabelTyID, this);
    int1_ty_ = new IntegerType(1, this);
    int32_ty_ = new IntegerType(32, this);
    // init instr_id2string
    instr_id2string_.insert({ Instruction::ret, "ret" }); 
    instr_id2string_.insert({ Instruction::br, "br" }); 
    
    instr_id2string_.insert({ Instruction::add, "add" });
    instr_id2string_.insert({ Instruction::sub, "sub" });
    instr_id2string_.insert({ Instruction::mul, "mul" });
    instr_id2string_.insert({ Instruction::sdiv, "sdiv" });
    instr_id2string_.insert({ Instruction::srem, "srem" });

    instr_id2string_.insert({ Instruction::smul_lo, "smul_lo" });
    instr_id2string_.insert({ Instruction::smul_hi, "smul_hi" });
    instr_id2string_.insert({ Instruction::smmul, "smmul" });

    instr_id2string_.insert({ Instruction::muladd, "muladd" });
    instr_id2string_.insert({ Instruction::mulsub, "mulsub" });

    instr_id2string_.insert({ Instruction::cmpbr, "cmpbr" });

    instr_id2string_.insert({ Instruction::land, "and" });
    instr_id2string_.insert({ Instruction::lor, "or" });
    instr_id2string_.insert({ Instruction::lxor, "xor" });

    instr_id2string_.insert({ Instruction::asr, "asr" });
    instr_id2string_.insert({ Instruction::lsl, "lsl" });
    instr_id2string_.insert({ Instruction::lsr, "lsr" });

    instr_id2string_.insert({ Instruction::asradd, "asradd" });
    instr_id2string_.insert({ Instruction::lsladd, "lsladd" });
    instr_id2string_.insert({ Instruction::lsradd, "lsradd" });
    instr_id2string_.insert({ Instruction::asrsub, "asrsub" });
    instr_id2string_.insert({ Instruction::lslsub, "lslsub" });
    instr_id2string_.insert({ Instruction::lsrsub, "lsrsub" });

    instr_id2string_.insert({ Instruction::alloca, "alloca" });
    instr_id2string_.insert({ Instruction::load, "load" });
    instr_id2string_.insert({ Instruction::store, "store" });
    instr_id2string_.insert({ Instruction::cmp, "icmp" });
    instr_id2string_.insert({ Instruction::phi, "phi" });
    instr_id2string_.insert({ Instruction::call, "call" });
    instr_id2string_.insert({ Instruction::getelementptr, "getelementptr" });
    instr_id2string_.insert({ Instruction::zext, "zext" });
    instr_id2string_.insert({ Instruction::load_offset, "load_offset" });
    instr_id2string_.insert({ Instruction::store_offset, "store_offset" });
    instr_id2string_.insert({ Instruction::mov_const, "mov_const" });
}

Module::~Module()
{
    delete void_ty_;
    delete label_ty_;
    delete int1_ty_;
    delete int32_ty_;
}

Type *Module::get_void_type()
{
    return void_ty_;
}

Type *Module::get_label_type()
{
    return label_ty_;
}

IntegerType *Module::get_int1_type()
{
    return int1_ty_;
}

IntegerType *Module::get_int32_type()
{
    return int32_ty_;
}

PointerType *Module::get_pointer_type(Type *contained)
{
    if( pointer_map_.find(contained) == pointer_map_.end() )
    {
        pointer_map_[contained] = new PointerType(contained);
    }
    return pointer_map_[contained];
}

ArrayType *Module::get_array_type(Type *contained, unsigned num_elements)
{
    if( array_map_.find({contained, num_elements}) == array_map_.end() )
    {
        array_map_[{contained, num_elements}] = new ArrayType(contained, num_elements);
    }
    return array_map_[{contained, num_elements}];
}

PointerType *Module::get_int32_ptr_type()
{
    return get_pointer_type(int32_ty_);
}

void Module::add_function(Function *f)
{
    function_list_.push_back(f);
}
std::list<Function* > &Module::get_functions(){
    return function_list_;
}
void Module::add_global_variable(GlobalVariable* g)
{
    global_list_.push_back(g);
}
std::list<GlobalVariable *> &Module::get_global_variable(){
    return global_list_;
}

void Module::set_print_name()
{
    for (auto func : this->get_functions())
    {
        func->set_instr_name();
    }
    return ;
}

std::string Module::print()
{
    std::string module_ir;
    for ( auto global_val : this->global_list_)
    {
        module_ir += global_val->print();
        module_ir += "\n";
    }
    for ( auto func : this->function_list_)
    {
        module_ir += func->print();
        module_ir += "\n";
    }
    return module_ir;
}
