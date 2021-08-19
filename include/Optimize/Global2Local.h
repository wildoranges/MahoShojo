#ifndef MHSJ_GLOBAL2LOCAL_H
#define MHSJ_GLOBAL2LOCAL_H

#include "Pass.h"
#include "SideEffectAnalysis.h"
#include <memory>

class Global2Local : public Pass
{
private:
    //global_var, use_func, use_insts(load and store)
    std::map<std::pair<GlobalVariable *, Function *>, std::vector<Instruction *>> localize_info;

    std::string name = "Global2Local";
public:
    explicit Global2Local(Module* module): Pass(module){}
    void execute() override;
    const std::string get_name() const override {return name;}
    void analyse();
    void localize(GlobalVariable *, Function *);



};

#endif
