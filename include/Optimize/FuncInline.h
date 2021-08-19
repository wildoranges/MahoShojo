#ifndef MHSJ_FUNCINLINE_H
#define MHSJ_FUNCINLINE_H

#include "Pass.h"

class FuncInline : public Pass
{
private:
    /* data */
    std::vector<std::pair<Function*, std::pair<Instruction *,Function *>>> calling_pair;
    std::string name = "FuncInline";
public:
    explicit FuncInline(Module* module): Pass(module){}
    void execute() override;
    const std::string get_name() const override {return name;}
    void need_inline_call_find();
    void func_inline();
};




#endif
