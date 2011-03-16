#ifndef ENGINECHECKOPTION_H
#define ENGINECHECKOPTION_H

#include "engineoption.h"

class LIB_EXPORT EngineCheckOption : public EngineOption
{
	public:
		EngineCheckOption(const QString& name,
				  const QVariant& value = QVariant(),
				  const QVariant& defaultValue = QVariant());

		EngineOption* copy() const;

		bool isValid(const QVariant& value) const;

		QVariant toVariant() const;
};

#endif // ENGINECHECKOPTION_H
