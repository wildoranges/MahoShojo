#ifndef MHSJ_MEM2REG_H
#define MHSJ_MEM2REG_H

#include "BasicBlock.h"
#include "Function.h"
#include "IRBuilder.h"
#include "Instruction.h"
#include "Module.h"
#include "Pass.h"

class Mem2Reg : public Pass{
private:
	Function *func_;
	IRBuilder *builder;
	std::map<BasicBlock *, std::vector<Value *>> define_var;
    std::string name = "mem2reg";
	std::map<Value*, Value*> lvalue_connection;
	std::set<Value*> no_union_set;

public:
	explicit Mem2Reg(Module *m) : Pass(m) {}
	~Mem2Reg(){};
	void execute() final;
	void genPhi();
	void insideBlockForwarding();
	void valueDefineCounting();
	void valueForwarding(BasicBlock *bb);
	void removeAlloc();
	void phiStatistic();
    const std::string get_name() const override {return name;}

	bool isLocalVarOp(Instruction *inst){
		if (inst->get_instr_type() == Instruction::OpID::store){
			StoreInst *sinst = static_cast<StoreInst *>(inst);
			auto lvalue = sinst->get_lval();
			auto glob = dynamic_cast<GlobalVariable *>(lvalue);
			auto array_element_ptr = dynamic_cast<GetElementPtrInst *>(lvalue);
			return !glob && !array_element_ptr;
		}
		else if (inst->get_instr_type() == Instruction::OpID::load){
			LoadInst *linst = static_cast<LoadInst *>(inst);
			auto lvalue = linst->get_lval();
			auto glob = dynamic_cast<GlobalVariable *>(lvalue);
			auto array_element_ptr = dynamic_cast<GetElementPtrInst *>(lvalue);
			return !glob && !array_element_ptr;
		}
		else
			return false;
	}
	// void DeleteLS();
	//可加一个遍历删除空phi节点
};

#endif