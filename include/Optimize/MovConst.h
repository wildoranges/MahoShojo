//
// Created by cjb on 8/14/21.
//

#ifndef MHSJ_MOVCONST_H
#define MHSJ_MOVCONST_H

#include "Module.h"
#include "Pass.h"


class MovConst : public Pass{
public:
    explicit MovConst(Module* module):Pass(module){}
    void execute()override;
    const std::string get_name() const override{return name;}

private:
    static void mov_const(BasicBlock* bb);
    const std::string name = "MovConst";
};

#endif //MHSJ_MOVCONST_H
