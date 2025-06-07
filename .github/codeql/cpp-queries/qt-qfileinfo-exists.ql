/**
 * @name Use of QFileInfo.exists instead of QFileInfo::exists
 * @description Using QFileInfo::exists is faster than using QFileInfo.exists
 * @kind problem
 * @problem.severity recommendation
 * @id cpp/qt-qfileinfo-exists
 * @precision high
 * @tags efficiency
 */
import cpp

from FunctionCall fc, Function fn
where
  fc.getTarget() = fn and
  fn.getDeclaringType().getSimpleName() = "QFileInfo" and
  fn.hasName("exists") and
  not fn.isStatic()
select fc, "Prefer use of QFileInfo::exists instead of QFileInfo.exists"
