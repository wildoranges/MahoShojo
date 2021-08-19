
#include "SyntaxTree.h"

using namespace SyntaxTree;

void Assembly::accept(Visitor &visitor) { visitor.visit(*this); }
void FuncDef::accept(Visitor &visitor) { visitor.visit(*this); }
void BinaryExpr::accept(Visitor &visitor) { visitor.visit(*this); }
void UnaryExpr::accept(Visitor &visitor) { visitor.visit(*this); }
void LVal::accept(Visitor &visitor) { visitor.visit(*this); }
void Literal::accept(Visitor &visitor) { visitor.visit(*this); }
void ReturnStmt::accept(Visitor &visitor) { visitor.visit(*this); }
void VarDef::accept(Visitor &visitor) { visitor.visit(*this); }
void AssignStmt::accept(Visitor &visitor) { visitor.visit(*this); }
void FuncCallStmt::accept(Visitor &visitor) { visitor.visit(*this); }
void BlockStmt::accept(Visitor &visitor) { visitor.visit(*this); }
void EmptyStmt::accept(Visitor &visitor) { visitor.visit(*this); }
void ExprStmt::accept(Visitor &visitor) { visitor.visit(*this); }
void FuncParam::accept(Visitor &visitor) { visitor.visit(*this); }
void FuncFParamList::accept(Visitor &visitor) { visitor.visit(*this); }
void IfStmt::accept(Visitor &visitor) { visitor.visit(*this); }
void WhileStmt::accept(Visitor &visitor) { visitor.visit(*this); }
void BreakStmt::accept(Visitor &visitor) { visitor.visit(*this); }
void ContinueStmt::accept(Visitor &visitor) { visitor.visit(*this); }
void UnaryCondExpr::accept(Visitor &visitor) { visitor.visit(*this); }
void BinaryCondExpr::accept(Visitor &visitor) { visitor.visit(*this); }
void InitVal::accept(Visitor &visitor) { visitor.visit(*this); }
