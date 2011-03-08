#ifndef ENGINETEXTOPTION_H
#define ENGINETEXTOPTION_H

#include "engineoption.h"

class LIB_EXPORT EngineTextOption : public EngineOption
{
	public:
		EngineTextOption(const QString& name,
				 const QVariant& value = QVariant(),
				 const QVariant& defaultValue = QVariant());

		bool isValid(const QVariant& value) const;

		QVariant toQVariant() const;
};

#endif // ENGINETEXTOPTION_H
