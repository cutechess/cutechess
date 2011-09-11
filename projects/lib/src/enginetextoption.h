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

		// Inherited from EngineOption
		virtual EngineOption* copy() const;
		virtual bool isValid(const QVariant& value) const;
		virtual QVariant toVariant() const;
};

#endif // ENGINETEXTOPTION_H
