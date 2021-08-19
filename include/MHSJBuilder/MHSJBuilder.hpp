#ifndef _MHSJ_BUILDER_HPP_
#define _MHSJ_BUILDER_HPP_
#include "BasicBlock.h"
#include "Constant.h"
#include "Function.h"
#include "IRBuilder.h"
#include "Module.h"
#include "Type.h"
#include <map>
#include "SyntaxTree.h"


class Scope {
public:
    // enter a new scope
    void enter() {
        inner.push_back({});
        inner_func.push_back({});
        inner_array_size.push_back({});
        inner_array_const.push_back({});
    }

    // exit a scope
    void exit() {
        inner.pop_back();
        inner_func.pop_back();
        inner_array_size.pop_back();
        inner_array_const.pop_back();
    }

    bool in_global() {
        return inner.size() == 1;
    }

    // push a name to scope
    // return true if successful
    // return false if this name already exits
    bool push(std::string name, Value *val) {
        auto result = inner[inner.size() - 1].insert({name, val});
        return result.second;
    }

    Value* find(std::string name) {
        for (auto s = inner.rbegin(); s!= inner.rend();s++) {
            auto iter = s->find(name);
            if (iter != s->end()) {
                return iter->second;
            }
        }

        return nullptr;
    }

    bool push_func(std::string name, Value *val) {
        auto result = inner_func[inner_func.size() - 1].insert({name, val});
        return result.second;
    }

    Value* find_func(std::string name) {
        for (auto s = inner_func.rbegin(); s!= inner_func.rend();s++) {
            auto iter = s->find(name);
            if (iter != s->end()) {
                return iter->second;
            }
        }

        return nullptr;
    }

    bool push_size(std::string name, std::vector<int> size){
        auto result = inner_array_size[inner_array_size.size() - 1].insert({name,size});
        return result.second;
    }

    std::vector<int> find_size(std::string name) {
        for (auto s = inner_array_size.rbegin(); s!=inner_array_size.rend(); s++){
            auto iter = s->find(name);
            if (iter != s->end()) {
                return iter->second;
            }
        }

        return {};
    }

    bool push_const(std::string name, ConstantArray* size){
        auto result = inner_array_const[inner_array_const.size() - 1].insert({name,size});
        return result.second;
    }

    ConstantArray* find_const(std::string name) {
        for (auto s = inner_array_const.rbegin(); s!=inner_array_const.rend(); s++){
            auto iter = s->find(name);
            if (iter != s->end()) {
                return iter->second;
            }
        }

        return nullptr;
    }

private:
    std::vector<std::map<std::string, Value *>> inner;
    std::vector<std::map<std::string, Value *>> inner_func;
    std::vector<std::map<std::string, std::vector<int>>> inner_array_size;
    std::vector<std::map<std::string, ConstantArray *>> inner_array_const;
};

class MHSJBuilder: public SyntaxTree::Visitor
{
private:
    virtual void visit(SyntaxTree::InitVal &) override final;
    virtual void visit(SyntaxTree::Assembly &) override final;
    virtual void visit(SyntaxTree::FuncDef &) override final;
    virtual void visit(SyntaxTree::VarDef &) override final;
    virtual void visit(SyntaxTree::AssignStmt &) override final;
    virtual void visit(SyntaxTree::ReturnStmt &) override final;
    virtual void visit(SyntaxTree::BlockStmt &) override final;
    virtual void visit(SyntaxTree::EmptyStmt &) override final;
    virtual void visit(SyntaxTree::ExprStmt &) override final;
    virtual void visit(SyntaxTree::UnaryCondExpr &) override final;
    virtual void visit(SyntaxTree::BinaryCondExpr &) override final;
    virtual void visit(SyntaxTree::BinaryExpr &) override final;
    virtual void visit(SyntaxTree::UnaryExpr &) override final;
    virtual void visit(SyntaxTree::LVal &) override final;
    virtual void visit(SyntaxTree::Literal &) override final;
    virtual void visit(SyntaxTree::FuncCallStmt &) override final;
    virtual void visit(SyntaxTree::FuncParam &) override final;
    virtual void visit(SyntaxTree::FuncFParamList &) override final;
    virtual void visit(SyntaxTree::IfStmt &) override final;
    virtual void visit(SyntaxTree::WhileStmt &) override final;
    virtual void visit(SyntaxTree::BreakStmt &) override final;
    virtual void visit(SyntaxTree::ContinueStmt &) override final;

    IRBuilder *builder;
    Scope scope;
    std::unique_ptr<Module> module;
public:
    MHSJBuilder(){
        module = std::unique_ptr<Module>(new Module("SysY code"));
        builder = new IRBuilder(nullptr, module.get());
        auto TyVoid = Type::get_void_type(module.get());
        auto TyInt32 = Type::get_int32_type(module.get());
        auto TyInt32Ptr = Type::get_int32_ptr_type(module.get());

        auto input_type = FunctionType::get(TyInt32, {});
        auto get_int =
            Function::create(
                    input_type,
                    "getint",
                    module.get());

        input_type = FunctionType::get(TyInt32, {});
        auto get_char =
            Function::create(
                    input_type,
                    "getch",
                    module.get());

        std::vector<Type *> input_params;
        std::vector<Type *>().swap(input_params);
        input_params.push_back(TyInt32Ptr);
        input_type = FunctionType::get(TyInt32, input_params);
        auto get_array =
            Function::create(
                    input_type,
                    "getarray",
                    module.get());

        std::vector<Type *> output_params;
        std::vector<Type *>().swap(output_params);
        output_params.push_back(TyInt32);
        auto output_type = FunctionType::get(TyVoid, output_params);
        auto put_int =
            Function::create(
                    output_type,
                    "putint",
                    module.get());

        std::vector<Type *>().swap(output_params);
        output_params.push_back(TyInt32);
        output_type = FunctionType::get(TyVoid, output_params);
        auto put_char =
            Function::create(
                    output_type,
                    "putch",
                    module.get());

        std::vector<Type *>().swap(output_params);
        output_params.push_back(TyInt32);
        output_params.push_back(TyInt32Ptr);
        output_type = FunctionType::get(TyVoid, output_params);
        auto put_array =
            Function::create(
                    output_type,
                    "putarray",
                    module.get());

        std::vector<Type *>().swap(input_params);
        input_params.push_back(TyInt32);
        auto time_type = FunctionType::get(TyVoid, input_params);
        auto start_time =
            Function::create(
                    time_type,
                    "_sysy_starttime",
                    module.get());

        std::vector<Type *>().swap(input_params);
        input_params.push_back(TyInt32);
        time_type = FunctionType::get(TyVoid, input_params);
        auto stop_time =
            Function::create(
                    time_type,
                    "_sysy_stoptime",
                    module.get());

        scope.enter();
        scope.push_func("getint", get_int);
        scope.push_func("getch", get_char);
        scope.push_func("getarray", get_array);
        scope.push_func("putint", put_int);
        scope.push_func("putch", put_char);
        scope.push_func("putarray", put_array);
        scope.push_func("starttime", start_time);
        scope.push_func("stoptime", stop_time);
    }
    std::unique_ptr<Module> getModule() {
        return std::move(module);
    }
};


#endif
