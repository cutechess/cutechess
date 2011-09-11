#ifndef ENGINE_OPTION_FACTORY_H
#define ENGINE_OPTION_FACTORY_H

#include <QVariant>

class EngineOption;

class LIB_EXPORT EngineOptionFactory
{
	public:
		static EngineOption* create(const QVariantMap& map);

	private:
		EngineOptionFactory();
};

#endif // ENGINE_OPTION_FACTORY_H
