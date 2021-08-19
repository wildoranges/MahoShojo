#include "SyntaxTreePrinter.h"
#include <map>
#include <string>
#include <iostream>

using namespace SyntaxTree;

std::map<Type, std::string> type2str = {
    {Type::INT, "int"},
    {Type::VOID, "void"}
};

std::map<BinaryCondOp,std::string> bincondop2str = {
        {BinaryCondOp::EQ,"=="},
        {BinaryCondOp::NEQ,"!="},
        {BinaryCondOp::GT,">"},
        {BinaryCondOp::GTE,">="},
        {BinaryCondOp::LT,"<"},
        {BinaryCondOp::LTE,"<="},
        {BinaryCondOp::LAND,"&&"},
        {BinaryCondOp::LOR,"||"}
};

std::map<BinOp, std::string> binop2str = {
    {BinOp::PLUS, "+"},
    {BinOp::MINUS, "-"},
    {BinOp::MULTIPLY, "*"},
    {BinOp::DIVIDE, "/"},
    {BinOp::MODULO, "%"}
};

std::map<UnaryCondOp,std::string> unarycondop2str = {
        {UnaryCondOp::NOT,"!"}
};

std::map<UnaryOp, std::string> unaryop2str = {
    {UnaryOp::PLUS, "+"},
    {UnaryOp::MINUS, "-"}
};

void SyntaxTreePrinter::print_indent()
{
    for (int i = 0; i < indent; i++)
        std::cout << " ";
}

void SyntaxTreePrinter::visit(Assembly &node)
{
    indent = 0;
    for (auto def : node.global_defs) {
        def->accept(*this);
    }
}

void SyntaxTreePrinter::visit(FuncDef &node)
{
    std::cout << type2str[node.ret_type] << " " 
        << node.name << "(";
    node.param_list->accept(*this);
    std::cout << ")" << std::endl;
    node.body->accept(*this);
    indent = 0;
}

void SyntaxTreePrinter::visit(BlockStmt &node)
{
    print_indent();
    std::cout << "{" << std::endl;
    indent += 4;
    for (auto stmt : node.body) {
        stmt->accept(*this);
    }
    indent -= 4;
    print_indent();
    std::cout << "}" << std::endl;
}

void SyntaxTreePrinter::visit(VarDef &node)
{
    print_indent();
    bool is_array = false;
    if (node.is_constant)
        std::cout << "const ";
    std::cout << type2str[node.btype] << " " << node.name;
    for (auto length : node.array_length) {
        std::cout << "[";
        length->accept(*this);
        std::cout << "]";
        is_array = true;
    }
    if (node.is_inited) {
        int tmp = indent;
        indent = 0;
        std::cout << " = ";
        // if (is_array)
        //     std::cout << "{";
        // size_t num = 0;
        // for (auto init : node.initializers) {
        //     init->accept(*this);
        //     if (num < node.initializers.size() - 1)
        //         std::cout << ", ";
        //     num++;
        // }
        // if (is_array)
        //     std::cout << "}";
        node.initializers->accept(*this);
        indent = tmp;
    }
    std::cout << ";" << std::endl;
}

void SyntaxTreePrinter::visit(InitVal &node){
    if(node.isExp){
        node.expr->accept(*this);
    }
    else{
        std::cout << "{";
        int i = 0;
        int num = node.elementList.size();
        for(auto item: node.elementList){
            item->accept(*this);
            i++;
            if(i < num) std::cout << ", ";
        }
        std::cout << "}";
    }
    return;
}

void SyntaxTreePrinter::visit(BinaryExpr &node)
{
    std::cout << "(";
    node.lhs->accept(*this);
    std::cout << binop2str[node.op];
    node.rhs->accept(*this);
    std::cout << ")";
}

void SyntaxTreePrinter::visit(UnaryExpr &node)
{
    std::cout << "(";
    std::cout << unaryop2str[node.op];
    node.rhs->accept(*this);
    std::cout << ")";
}

void SyntaxTreePrinter::visit(LVal &node)
{
    std::cout << node.name;
    for (auto index : node.array_index) {
        std::cout << "[";
        index->accept(*this);
        std::cout << "]";
    }
}

void SyntaxTreePrinter::visit(Literal &node)
{
    if (node.is_int)
        std::cout << node.int_const;
    else
        std::cout << node.str;
}

void SyntaxTreePrinter::visit(ReturnStmt &node)
{
    print_indent();
    std::cout << "return";
    //indent = 0;
    if (node.ret.get()) {
        std::cout << " ";
        node.ret->accept(*this);
    }
    std::cout << ";" << std::endl;
}

void SyntaxTreePrinter::visit(AssignStmt &node)
{
    print_indent();
    node.target->accept(*this);
    std::cout << " = ";
    node.value->accept(*this);
    std::cout << ";" << std::endl;
}

void SyntaxTreePrinter::visit(FuncCallStmt &node)
{
    //print_indent();
    std::cout << node.name << "(";
    size_t num = 0;
    for(auto exp:node.params)
    {
        exp->accept(*this);
        if(num < node.params.size() - 1)
        {
            std::cout <<", ";
        }
        num++;
    }
    std::cout <<")";
}

void SyntaxTreePrinter::visit(EmptyStmt &)
{
    print_indent();
    std::cout << ";" << std::endl;
}

void SyntaxTreePrinter::visit(ExprStmt &node)
{
    print_indent();
    node.exp->accept(*this);
    std::cout << ";" << std::endl;
}

void SyntaxTreePrinter::visit(FuncParam &node)
{
    std::cout << type2str[node.param_type] << " " << node.name;
    for(auto exp:node.array_index)
    {
        std::cout << "[";
        if (exp != nullptr)
            exp->accept(*this);
        std::cout << "]";
    }
}

void SyntaxTreePrinter::visit(FuncFParamList &node)
{
    size_t num = 0;
    for(auto param:node.params)
    {
        param->accept(*this);
        if(num < node.params.size() - 1)
        {
            std::cout << ", ";
        }
        num++;
    }
}

void SyntaxTreePrinter::visit(BinaryCondExpr &node)
{
    std::cout << "(";
    node.lhs->accept(*this);
    std::cout << bincondop2str[node.op];
    node.rhs->accept(*this);
    std::cout << ")";
}

void SyntaxTreePrinter::visit(UnaryCondExpr &node)
{
    std::cout <<"(";
    std::cout << unarycondop2str[node.op];
    node.rhs->accept(*this);
    std::cout << ")";
}

void SyntaxTreePrinter::visit(IfStmt &node)
{
    print_indent();
    std::cout << "if";
    std::cout << " ";
    std::cout << "(";
    node.cond_exp->accept(*this);
    std::cout << ")" << std::endl;
    if(dynamic_cast<BlockStmt*>(node.if_statement.get())){
        node.if_statement->accept(*this);
    }
    else{
        indent+=4;
        node.if_statement->accept(*this);
        indent-=4;
    }
    if (node.else_statement != nullptr) {
        print_indent();
        std::cout << "else" << std::endl;
        if(dynamic_cast<BlockStmt*>(node.else_statement.get())){
            node.else_statement->accept(*this);
        }else {
            indent+=4;
            node.else_statement->accept(*this);
            indent-=4;
        }
    }
}

void SyntaxTreePrinter::visit(WhileStmt &node)
{
    print_indent();
    std::cout << "while";
    std::cout << " ";
    std::cout << "(";
    node.cond_exp->accept(*this);
    std::cout << ")"<< std::endl;
    if(dynamic_cast<BlockStmt*>(node.statement.get())){
        node.statement->accept(*this);
    }
    else{
        indent+=4;
        node.statement->accept(*this);
        indent-=4;
    }
}

void SyntaxTreePrinter::visit(BreakStmt &node)
{
    print_indent();
    std::cout << "break";
    std::cout << ";" <<std::endl;
}

void SyntaxTreePrinter::visit(ContinueStmt &node)
{
    print_indent();
    std::cout << "continue";
    std::cout << ";" <<std::endl;
}
