#include "enginefactory.h"
#include "xboardengine.h"
#include "uciengine.h"


REGISTER_ENGINE_CLASS(XboardEngine, "xboard", "winboard,cecp")
REGISTER_ENGINE_CLASS(UciEngine, "uci", "")

ClassRegistry<ChessEngine>* EngineFactory::registry()
{
	static ClassRegistry<ChessEngine>* registry = new ClassRegistry<ChessEngine>;
	return registry;
}

ChessEngine* EngineFactory::create(const QString& protocol)
{
	return registry()->create(protocol);
}

QStringList EngineFactory::protocols()
{
	return registry()->items().keys();
}
