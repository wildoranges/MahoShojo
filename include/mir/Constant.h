#ifndef SYSY_CONSTANT_H
#define SYSY_CONSTANT_H
#include "User.h"
#include "Value.h"
#include "Type.h"

class Constant : public User
{
private:
    // int value;
public:
    Constant(Type *ty, const std::string &name = "", unsigned num_ops = 0)
        : User(ty, name, num_ops) {}
    ~Constant() = default;
};

class ConstantInt : public Constant
{
private:
    int value_;
    ConstantInt(Type* ty,int val) 
        : Constant(ty,"",0),value_(val) {}
public:
    
    static int get_value(ConstantInt *const_val) { return const_val->value_; }
    int get_value() { return value_; }
    static ConstantInt *get(int val, Module *m);
    static ConstantInt *get(bool val, Module *m);
    virtual std::string print() override;
};

class ConstantArray : public Constant
{
private:
    std::vector<Constant*> const_array;

    ConstantArray(ArrayType *ty, const std::vector<Constant*> &val);
public:
    
    ~ConstantArray()=default;

    Constant* get_element_value(int index);

    unsigned get_size_of_array() { return const_array.size(); } 

    static ConstantArray *get(ArrayType *ty, const std::vector<Constant*> &val);

    virtual std::string print() override;
};

class ConstantZero : public Constant 
{
private:
    ConstantZero(Type *ty)
        : Constant(ty,"",0) {}
public:
    static ConstantZero *get(Type *ty, Module *m);
    virtual std::string print() override;
};

#endif //SYSY_CONSTANT_H
