#ifndef ENGINECHECKOPTION_H
#define ENGINECHECKOPTION_H

#include "engineoption.h"

class LIB_EXPORT EngineCheckOption : public EngineOption
{
	public:
		EngineCheckOption(const QString& name,
		                  const QVariant& value = QVariant(),
		                  const QVariant& defaultValue = QVariant(),
		                  const QString& alias = QString());

		// Inherited from EngineOption
		virtual EngineOption* copy() const;
		virtual bool isValid(const QVariant& value) const;
		virtual QVariant toVariant() const;
};

#endif // ENGINECHECKOPTION_H
