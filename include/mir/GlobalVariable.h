#ifndef SYSY_GLOBALVARIABLE_H
#define SYSY_GLOBALVARIABLE_H

#include "Module.h"
#include "User.h"
#include "Constant.h"

class GlobalVariable : public User
{
private:
    bool is_const_ ;
    Constant* init_val_;
    GlobalVariable(std::string name, Module *m, Type* ty, bool is_const, Constant* init = nullptr);
public:
    static GlobalVariable *create(std::string name, Module *m, Type* ty, bool is_const, 
                                Constant* init );

    Constant *get_init() { return init_val_; }
    bool is_const() { return is_const_; }
    std::string print();
};
#endif //SYSY_GLOBALVARIABLE_H
