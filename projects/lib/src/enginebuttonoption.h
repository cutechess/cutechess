#ifndef ENGINEBUTTONOPTION_H
#define ENGINEBUTTONOPTION_H

#include "engineoption.h"

class LIB_EXPORT EngineButtonOption : public EngineOption
{
	public:
		EngineButtonOption(const QString& name);

		EngineOption* copy() const;

		bool isValid(const QVariant& value) const;
		QVariant toVariant() const;
};

#endif // ENGINEBUTTONOPTION_H
