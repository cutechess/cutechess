/**
 * @name Use of a comma expression inside an if statement
 * @description Use of a comma expression inside an if statement is widely discouraged and may indicate an error in the controlling expression
 * @kind problem
 * @problem.severity recommendation
 * @id cpp/commaexpr-in-ifstmt
 * @precision high
 */

import cpp

predicate commaExpr(Expr e) {
  e instanceof CommaExpr
}

class CommaExprIfStmt extends IfStmt {
  CommaExprIfStmt() { commaExpr(this.getControllingExpr()) }
}

from CommaExprIfStmt ci
select ci, "Comma expression used inside an 'if' statement"
