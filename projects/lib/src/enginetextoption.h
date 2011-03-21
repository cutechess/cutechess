#ifndef ENGINETEXTOPTION_H
#define ENGINETEXTOPTION_H

#include "engineoption.h"

class LIB_EXPORT EngineTextOption : public EngineOption
{
	public:
		EngineTextOption(const QString& name,
		                 const QVariant& value = QVariant(),
		                 const QVariant& defaultValue = QVariant(),
		                 const QString& alias = QString());

		EngineOption* copy() const;

		bool isValid(const QVariant& value) const;

		QVariant toVariant() const;
};

#endif // ENGINETEXTOPTION_H
