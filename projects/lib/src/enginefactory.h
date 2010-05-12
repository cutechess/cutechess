#ifndef ENGINEFACTORY_H
#define ENGINEFACTORY_H

#include <QStringList>
#include "classregistry.h"
#include "chessengine.h"


/*! \brief A factory for creating ChessEngine objects. */
class LIB_EXPORT EngineFactory
{
	public:
		/*! Returns the class registry for concrete ChessEngine subclasses. */
		static ClassRegistry<ChessEngine>* registry();
		/*!
		 * Creates and returns a new engine that uses protocol \a protocol.
		 * Returns 0 if no engine class is associated with \a protocol.
		 */
		static ChessEngine* create(const QString& protocol,
					   QObject* parent = 0);
		/*! Returns a list of supported chess protocols. */
		static QStringList protocols();

	private:
		EngineFactory();
};

/*!
 * Registers engine class \a TYPE with protocol name \a PROTOCOL.
 *
 * This macro must be called once for every concrete ChessEngine class.
 */
#define REGISTER_ENGINE_CLASS(TYPE, PROTOCOL) \
	REGISTER_CLASS(ChessEngine, TYPE, PROTOCOL, EngineFactory::registry());

#endif // ENGINEFACTORY_H
