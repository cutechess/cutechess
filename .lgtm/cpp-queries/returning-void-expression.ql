/**
 * @name Returning void expression
 * @description Returning void expression
 * @kind problem
 * @problem.severity warning
 * @id cpp/returning-void-expression
 * @precision medium
 */

import cpp

predicate voidExpr(Expr e) {
  e.getType() instanceof VoidType
}

class VoidReturnStmt extends ReturnStmt {
  VoidReturnStmt() { voidExpr(this.getExpr()) }
}

from VoidReturnStmt r
  where not (r.getExpr() instanceof ConstructorCall)
select r, "Returning void expression"
