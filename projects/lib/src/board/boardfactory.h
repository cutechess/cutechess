#ifndef BOARDFACTORY_H
#define BOARDFACTORY_H

#include <QStringList>
#include <classregistry.h>
#include "board.h"


namespace Chess {

/*! \brief A factory for creating Board objects. */
class LIB_EXPORT BoardFactory
{
	public:
		/*! Returns the class registry for concrete Board subclasses. */
		static ClassRegistry<Board>* registry();
		/*!
		 * Creates and returns a new Board of variant \a variant.
		 * Returns 0 if \a variant is not supported.
		 */
		static Board* create(const QString& variant, QObject* parent = 0);
		/*! Returns a list of supported chess variants. */
		static QStringList variants();

	private:
		BoardFactory();
};

/*!
 * Registers board class \a TYPE with variant name \a VARIANT.
 *
 * This macro must be called once for every concrete Board class.
 */
#define REGISTER_BOARD(TYPE, VARIANT) \
	REGISTER_CLASS(Board, TYPE, VARIANT, BoardFactory::registry());

} // namespace Chess

#endif // BOARDFACTORY_H
