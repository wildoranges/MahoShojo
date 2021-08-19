#include <iostream>
#include "MHSJBuilder.hpp"
#include "MHSJDriver.h"
#include "SyntaxTreePrinter.h"
#include "SyntaxTreeChecker.h"

#include "Pass.h"
#include "DominateTree.h"
#include "mem2reg.h"
#include "LIR.h"
#include "ActiveVar.h"
#include "ConstPropagation.h"
#include "DeadCodeElimination.h"
#include "CFGSimplifier.h"
#include "CFG_analyse.h"
#include "CodeGen.h"

#include "LoopInvariant.h"
#include "AvailableExpr.h"
#include "FuncInline.h"
#include "LoopExpansion.h"
#include "Global2Local.h"
#include "MovConst.h"
#include "SCCP.h"

void print_help(const std::string& exe_name) {
  std::cout << "Usage: " << exe_name
            << " [ -h | --help ] [ -p | --trace_parsing ] [ -s | --trace_scanning ] [ -emit-mir ] [ -emit-lir ] [ -emit-ast ] [-check] [-o <output-file> ] [ -O2 ] [ -S ]"
            << " [ -no-const-prop ] [ -no-ava-expr ] [ -no-cfg-simply ] [ -no-dead-code-eli ] [ -no-func-inline ] [ -no-loop-expand ] [ -no-loop-invar ] [ -no-sccp ]"
            << " <input-file>"
            << std::endl;
}

int main(int argc, char *argv[])
{
    MHSJBuilder builder;
    MHSJDriver driver;
    SyntaxTreePrinter printer;
    ErrorReporter reporter(std::cerr);
    SyntaxTreeChecker checker(reporter);

    bool print_mir = false;
    bool print_ast = false;
    bool print_LIR = false;
    bool check = false;
    bool codegen=false;
    bool optimize = false;
    bool no_const_prop = false;
    bool no_ava_expr = false;
    bool no_cfg_simply = false;
    bool no_dead_code_eli = false;
    bool no_func_inline = false;
    bool no_loop_expand = false;
    bool no_loop_invar = false;
    bool no_sccp = false;
    std::string out_asm_file = "testcase.s";
    std::string out_ll_file = "testcase.ll";
    std::string filename = "testcase.sy";
    for (int i = 1; i < argc; ++i) {
        if (argv[i] == std::string("-h") || argv[i] == std::string("--help")) {
            print_help(argv[0]);
            return 0;
        }
        else if (argv[i] == std::string("-p") || argv[i] == std::string("--trace_parsing"))
            driver.trace_parsing = true;
        else if (argv[i] == std::string("-s") || argv[i] == std::string("--trace_scanning"))
            driver.trace_scanning = true;
        else if (argv[i] == std::string("-emit-mir"))
            print_mir = true;
        else if (argv[i] == std::string("-emit-lir"))
            print_LIR = true;
        else if (argv[i] == std::string("-emit-ast"))
            print_ast = true;
        else if (argv[i] == std::string("-check"))
            check = true;
        else if (argv[i] == std::string("-o")){
            out_asm_file = argv[++i];
            out_ll_file = out_asm_file;
        }
        else if (argv[i] == std::string("-O2")) {
            optimize = true;
        }
        else if (argv[i] == std::string("-S")){
            codegen = true;
            //print_IR = true;
        }
        else if (argv[i] == std::string("-no-const-prop")){
            no_const_prop = true;
        }
        else if (argv[i] == std::string("-no-ava-expr")){
            no_ava_expr = true;
        }
        else if (argv[i] == std::string("-no-cfg-simply")){
            no_cfg_simply = true;
        }
        else if (argv[i] == std::string("-no-dead-code-eli")){
            no_dead_code_eli = true;
        }
        else if (argv[i] == std::string("-no-func-inline")){
            no_func_inline = true;
        }
        else if (argv[i] == std::string("-no-loop-expand")){
            no_loop_expand = true;
        }
        else if (argv[i] == std::string("-no-loop-invar")){
            no_loop_invar = true;
        }
        else if (argv[i] == std::string("-no-sccp")){
            no_sccp = true;
        }
        else {
            filename = argv[i];
        }
    }
    auto root = driver.parse(filename);
    if(print_ast)
        root->accept(printer);
    if(check){
        root->accept(checker);
        if(checker.is_err())
        {
            std::cerr<<"The file has semantic errors\n";
            exit(-1);
        }
    }
    if (print_LIR||codegen||print_mir) {
        root->accept(builder);
        auto m = builder.getModule();
        m->set_file_name(filename);
        if(!optimize){
            PassMgr passmgr(m.get());
            //passmgr.addPass<CFGSimplifier>();
            passmgr.addPass<DominateTree>();
            passmgr.addPass<Mem2Reg>();
            passmgr.addPass<Global2Local>();
            if(!print_mir){
                //passmgr.addPass<ConstPropagation>();
                passmgr.addPass<LIR>();
                passmgr.addPass<CFGSimplifier>();
                passmgr.addPass<MovConst>();
                passmgr.addPass<ActiveVar>();
                passmgr.addPass<CFG_analyse>();
            }
            m->set_print_name();
            passmgr.execute();
        }
        else{
            PassMgr passmgr(m.get());

//            if(!no_cfg_simply)
//                passmgr.addPass<CFGSimplifier>();
//
//            if(!no_dead_code_eli)
//                passmgr.addPass<DeadCodeElimination>();

//            if(!no_dead_code_eli)
//                passmgr.addPass<DeadCodeElimination>();

//            if(!no_cfg_simply)
//                passmgr.addPass<CFGSimplifier>();

            passmgr.addPass<DominateTree>();
            passmgr.addPass<Mem2Reg>();

            if(!no_dead_code_eli)
                passmgr.addPass<DeadCodeElimination>();

            passmgr.addPass<Global2Local>();

            if(!no_dead_code_eli)
                passmgr.addPass<DeadCodeElimination>();

            if(!no_const_prop)
                passmgr.addPass<ConstPropagation>();

            if(!no_cfg_simply)
                passmgr.addPass<CFGSimplifier>();

            if(!no_dead_code_eli)
                passmgr.addPass<DeadCodeElimination>();

            if(!no_cfg_simply)
                passmgr.addPass<CFGSimplifier>();

            if(!no_ava_expr)
                passmgr.addPass<AvailableExpr>();

            if(!no_dead_code_eli)
                passmgr.addPass<DeadCodeElimination>();

            if(!no_cfg_simply)
                passmgr.addPass<CFGSimplifier>();

//            if(!no_cfg_simply)
//                passmgr.addPass<CFGSimplifier>();

            if(!no_const_prop)
                passmgr.addPass<ConstPropagation>();

            if(!no_cfg_simply)
                passmgr.addPass<CFGSimplifier>();

            if(!no_dead_code_eli)
                passmgr.addPass<DeadCodeElimination>();

            if(!no_cfg_simply)
                passmgr.addPass<CFGSimplifier>();

            if(!no_dead_code_eli)
                passmgr.addPass<DeadCodeElimination>();

            if(!no_cfg_simply)
                passmgr.addPass<CFGSimplifier>();

            if(!no_loop_invar)
                passmgr.addPass<LoopInvariant>();

            if(!no_cfg_simply)
                passmgr.addPass<CFGSimplifier>();

            if(!no_ava_expr)
                passmgr.addPass<AvailableExpr>();

            if(!no_dead_code_eli)
                passmgr.addPass<DeadCodeElimination>();

            if(!no_cfg_simply)
                passmgr.addPass<CFGSimplifier>();

            if(!no_loop_expand)
                passmgr.addPass<LoopExpansion>();

            if(!no_func_inline)
                passmgr.addPass<FuncInline>();

            if(!no_sccp)
                passmgr.addPass<SCCP>();

            if(!no_dead_code_eli)
                passmgr.addPass<DeadCodeElimination>();

            if(!no_cfg_simply)
                passmgr.addPass<CFGSimplifier>();

            if(!no_dead_code_eli)
                passmgr.addPass<DeadCodeElimination>();

            if(!no_const_prop)
                passmgr.addPass<ConstPropagation>();

            if(!no_cfg_simply)
                passmgr.addPass<CFGSimplifier>();

            if(!no_dead_code_eli)
                passmgr.addPass<DeadCodeElimination>();

            if(!no_cfg_simply)
                passmgr.addPass<CFGSimplifier>();

            if(!no_ava_expr)
                passmgr.addPass<AvailableExpr>();

            if(!no_dead_code_eli)
                passmgr.addPass<DeadCodeElimination>();

            if(!print_mir){
                passmgr.addPass<LIR>();
                // passmgr.addPass<MovConst>();
            }

            if(!no_const_prop)
                passmgr.addPass<ConstPropagation>();

            if(!no_cfg_simply)
                passmgr.addPass<CFGSimplifier>();

            if(!no_dead_code_eli)
                passmgr.addPass<DeadCodeElimination>();

            if(!no_ava_expr)
                passmgr.addPass<AvailableExpr>();

            if(!no_dead_code_eli)
                passmgr.addPass<DeadCodeElimination>();

            if(!no_cfg_simply)
                passmgr.addPass<CFGSimplifier>();

            if(!print_mir){
                passmgr.addPass<MovConst>();
            }

            if(!no_ava_expr)
                passmgr.addPass<AvailableExpr>();

            if(!no_dead_code_eli)
                passmgr.addPass<DeadCodeElimination>();

            passmgr.addPass<ActiveVar>();
            passmgr.addPass<CFG_analyse>();

            m->set_print_name();
            passmgr.execute();
        }
        m->set_print_name();
        if(codegen&&!(print_LIR||print_mir)){
            CodeGen coder = CodeGen();
            auto asmcode = coder.module_gen(m.get());
            std::ofstream output_stream;
            output_stream.open(out_asm_file, std::ios::out);
            output_stream << asmcode;
            output_stream.close();
        }
        else if(print_LIR||print_mir){
            auto IR = m->print();
            std::ofstream output_stream;
            output_stream.open(out_ll_file, std::ios::out);
            output_stream << IR;
            //std::cout << "outputir\n";
            output_stream.close();
        }
    }
    return 0;
}
