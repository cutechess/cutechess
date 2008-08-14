/*
    This file is part of SloppyGUI.

    SloppyGUI is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SloppyGUI is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SloppyGUI.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MANAGER_H
#define MANAGER_H

class LogManager;

/**
 * ManagerBase is the template base class of each Manager (Singleton) type
 * class.
 *
 * ManagerBase provides the base operations for singleton classes namely
 * getting the instance and releasing it.
*/
template <typename T> class ManagerBase
{
	public:
		/**
		 * Returns an instance of the template class T.
		 * @return Instance of T
		*/
		static T* get()
		{
			if (!instance)
			{
				instance = new T();
			}

			return instance;
		}

		/**
		 * Releases the instance of the class.
		*/
		static void release()
		{
			delete instance;
			instance = 0;
		}

	protected:
		ManagerBase() { }
		virtual ~ManagerBase() { }

		/** Instance of the template class. */
		static T* instance;
};

/*!
 * \brief The Manager class is the base of operations.
 *
 * The %Manager has two main tasks:
 *
 * - Provide access to other managers
 * - Provide methods which are required across the application
*/
class Manager : public ManagerBase<Manager>
{
	// 'friend' is required here so that ManagerBase can
	// create an instance of this class
	friend class ManagerBase<Manager>;
	
	public:
		/*!
		 * Returns an instance of the LogManager class.
		 * \sa LogManager
		*/
		LogManager* getLogManager();

		/*!
		 * Releases all managers.
		*/
		static void quit();

	protected:
		Manager() { }
};

#endif // MANAGER_H

