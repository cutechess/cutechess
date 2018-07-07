/*
    This file is part of Cute Chess.
    Copyright (C) 2008-2018 Cute Chess authors

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

/*!
 * Registers a new class.
 *
 * \param BASE The base class.
 * \param TYPE The subclass to register.
 * \param KEY The key associated with class \a TYPE.
 * \param REGISTRY A pointer to a ClassRegistry<BASE> object.
 *
 * \note The call to this macro should be in class \a TYPE's
 * implementation file (.cpp).
 */
#define REGISTER_CLASS(BASE, TYPE, KEY, REGISTRY) \
	static ClassRegistration<BASE> _class_registration_ ## TYPE(REGISTRY, &ClassRegistry<BASE>::factory<TYPE>, KEY);

/*!
 * \brief A class for creating objects based on the class'
 * runtime name or key (a string).
 *
 * The created objects of a registry must have the same base class.
 */
template<class T>
class ClassRegistry
{
	public:
		/*! Typedef to the factory function. */
		typedef T* (*Factory)(void);

		/*!
		 * Factory function for creating an object of type \a Subclass,
		 * which must be a subclass of \a T.
		 */
		template<class Subclass>
		static T* factory()
		{
			return new Subclass;
		}

		/*! Returns a list of factory functions. */
		const QMap<QString, Factory>& items() const
		{
			return m_items;
		}
		/*! Adds a new factory associated with \a key. */
		void add(Factory f, const QString& key)
		{
			m_items[key] = f;
		}
		/*!
		 * Creates and returns an object whose type is associated with \a key.
		 *
		 * Returns 0 if there is no type that matches \a key.
		 */
		T* create(const QString& key)
		{
			if (!m_items.contains(key))
				return nullptr;

			return m_items[key]();
		}

	private:
		QMap<QString, Factory> m_items;
};

/*! \brief A class for registering a new subclass of the templated class. */
template<class T>
class ClassRegistration
{
	public:
		/*!
		 * Creates a new registration object and adds (registers)
		 * a new class to a class registry.
		 *
		 * \param registry A registry where the class is added.
		 * \param factory A factory function for creating instances
		 * of the class.
		 * \param key A key (class name) associated with the class.
		 */
		ClassRegistration(ClassRegistry<T>* registry,
				  typename ClassRegistry<T>::Factory factory,
				  const QString& key)
		{
			registry->add(factory, key);
		}
};

#endif // CLASSREGISTRY_H
