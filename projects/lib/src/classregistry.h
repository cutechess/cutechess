/*
    This file is part of Cute Chess.

    Cute Chess is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Cute Chess is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Cute Chess.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CLASSREGISTRY_H
#define CLASSREGISTRY_H

#include <QString>
#include <QMap>
class QObject;

/*!
 * Registers class \a subclass (derived from \a baseclass) with key \a key.
 *
 * The call to REGISTER_CLASS should be in \a subclass's header file after
 * the class definition.
 */
#define REGISTER_CLASS(baseclass, subclass, key) \
	static ClassRegistration<baseclass> _class_registration_ ## subclass(&classFactory<baseclass, subclass>, key);

/*!
 * A singleton class for creating objects based on
 * the class' runtime name or key (a string).
 *
 * The created objects of a registry must have the same base
 * class, and they must be derived from QObject.
 */
template<class T>
class ClassRegistry
{
	public:
		/*! Typedef to the factory function. */
		typedef T* (*Factory)(void);

		/*! Returns the instance of the registry. */
		static ClassRegistry<T>& instance()
		{
			static ClassRegistry<T> instance;
			return instance;
		}

		/*! Returns a list of factory functions. */
		const QMap<QString, Factory>& items() const
		{
			return m_items;
		}
		/*! Adds a new factory associated with \a key. */
		void add(Factory factory, const QString& key)
		{
			m_items[key] = factory;
		}
		/*!
		 * Creates and returns an object whose type is associated with \a key.
		 *
		 * Returns 0 if there is no type that matches \a key.
		 * Sets the created object's parent to \a parent.
		 */
		static T* create(const QString& key, QObject* parent = 0)
		{
			const QMap<QString, Factory>& items = instance().m_items;
			if (!items.contains(key))
				return 0;

			T* ptr = items[key]();
			ptr->setParent(parent);
			return ptr;
		}

	private:
		QMap<QString, Factory> m_items;
};

/*!
 * Factory function for creating an object of type \a T,
 * which must be a subclass of \a Base.
 */
template<class Base, class T>
Base* classFactory()
{
	return new T;
}

/*! A class for registering a new subclass of the templated class. */
template<class T>
class ClassRegistration
{
	public:
		/*!
		 * Creates a new registration object and adds a factory
		 * function associated with \a key to the class registry.
		 */
		ClassRegistration(typename ClassRegistry<T>::Factory factory,
				  const QString& key)
		{
			ClassRegistry<T>::instance().add(factory, key);
		}
};

#endif // CLASSREGISTRY_H
