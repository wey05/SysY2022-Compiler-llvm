#include "Ast.h"

void CompUnit::accept(Visitor& v) { v.visit(this); }
void Decl::accept(Visitor& v) { v.visit(this); }
void VarDef::accept(Visitor& v) { v.visit(this); }
void FuncDef::accept(Visitor& v) { v.visit(this); }
void FuncFParam::accept(Visitor& v) { v.visit(this); }
void BlockStmt::accept(Visitor& v) { v.visit(this); }
void ExpStmt::accept(Visitor& v) { v.visit(this); }
void AssignStmt::accept(Visitor& v) { v.visit(this); }
void IfStmt::accept(Visitor& v) { v.visit(this); }
void WhileStmt::accept(Visitor& v) { v.visit(this); }
void BreakStmt::accept(Visitor& v) { v.visit(this); }
void ContinueStmt::accept(Visitor& v) { v.visit(this); }
void ReturnStmt::accept(Visitor& v) { v.visit(this); }
void BinaryExp::accept(Visitor& v) { v.visit(this); }
void UnaryExp::accept(Visitor& v) { v.visit(this); }
void LVal::accept(Visitor& v) { v.visit(this); }
void IntConst::accept(Visitor& v) { v.visit(this); }
void FloatConst::accept(Visitor& v) { v.visit(this); }
void CallExp::accept(Visitor& v) { v.visit(this); }