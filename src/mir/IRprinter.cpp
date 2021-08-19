#include "IRprinter.h"

std::string print_as_op( Value *v, bool print_ty )
{
    std::string op_ir;
    if( print_ty )
    {
        if (dynamic_cast<CmpInst*>(v)) {
            op_ir += "i1 ";
        } else {
            op_ir += v->get_type()->print(); 
            op_ir += " ";
        }
    }

    if (dynamic_cast<GlobalVariable *>(v))
    {
        op_ir += "@"+v->get_name();
    }
    else if ( dynamic_cast<Function *>(v) )
    {
        op_ir += "@"+v->get_name();
    }
    else if ( dynamic_cast<Constant *>(v))
    {
        op_ir += v->print();
    }
    else
    {
        op_ir += "%"+v->get_name();
    }

    return op_ir;
}

std::string print_cmp_type( CmpInst::CmpOp op )
{
    switch (op)
    {
    case CmpInst::GE:
        return "sge";
        break;
    case CmpInst::GT:
        return "sgt";
        break;
    case CmpInst::LE:
        return "sle";
        break;
    case CmpInst::LT:
        return "slt";
        break;
    case CmpInst::EQ:
        return "eq";
        break;
    case CmpInst::NE:
        return "ne";
        break;
    default:
        break;
    }
    return "wrong cmpop";
}
