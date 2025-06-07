/**
 * @name Use of QMap.values or QHash.values
 * @description Calls to QMap.values and QHash.values functions should be reviewed in detail
 * @kind problem
 * @problem.severity recommendation
 * @id cpp/qt-map-values
 * @precision high
 * @tags efficiency
 */
import cpp

from FunctionCall fc, Function fn
where
  fc.getTarget() = fn and
  (fn.getDeclaringType().getSimpleName() = "QMap" or fn.getDeclaringType().getSimpleName() = "QHash")  and
  fn.hasName("values") and
  not fn.isStatic()
select fc, "Review the use of QMap.values / QHash.values"
