/**
 * @name Avoid using slow key retrieval methods on Qt dictionary types
 * @description Calls to key retrieval functions should be reviewed in detail
 * @kind problem
 * @problem.severity recommendation
 * @id cpp/qt-map-keys
 * @precision high
 * @tags efficiency
 */
import cpp

from FunctionCall fc, Function fn
where
  fc.getTarget() = fn and
  (fn.getDeclaringType().getSimpleName() = "QMap" or fn.getDeclaringType().getSimpleName() = "QHash")  and
  (fn.hasName("key") or (fn.hasName("keys") and fn.getNumberOfParameters() != 0)) and
  not fn.isStatic()
select fc, "Review the retrieval of keys from dictionary types"
