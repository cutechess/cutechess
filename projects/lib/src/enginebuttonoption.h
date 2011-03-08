#ifndef ENGINEBUTTONOPTION_H
#define ENGINEBUTTONOPTION_H

#include "engineoption.h"

class LIB_EXPORT EngineButtonOption : public EngineOption
{
	public:
		EngineButtonOption();
		EngineButtonOption(const QString& name);

		bool isValid(const QVariant& value) const;
};

Q_DECLARE_METATYPE(EngineButtonOption)

#endif // ENGINEBUTTONOPTION_H
