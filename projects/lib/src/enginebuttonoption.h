#ifndef ENGINEBUTTONOPTION_H
#define ENGINEBUTTONOPTION_H

#include "engineoption.h"

class LIB_EXPORT EngineButtonOption : public EngineOption
{
	public:
		EngineButtonOption(const QString& name);

		bool isValid(const QVariant& value) const;
};

#endif // ENGINEBUTTONOPTION_H
