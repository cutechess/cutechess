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

#ifndef ECONODE_H
#define ECONODE_H

#include <QString>
#include <QMap>
#include "pgngame.h"
class QDataStream;
class PgnStream;

/*!
 * \brief A node in the ECO tree (Encyclopaedia of Chess Openings)
 *
 * The EcoNode class can be used to generate and query a database (a tree) of
 * known chess openings that belong to the Encyclopaedia of Chess Openings.
 * More about ECO: http://en.wikipedia.org/wiki/Encyclopaedia_of_Chess_Openings
 *
 * The ECO tree can be generated from a PGN collection or from a binary file
 * that's part of the cutechess library (the default). A node corresponding
 * to a PgnGame can be found by traversing the ECO tree as new moves are added
 * to the game, or by passing all the moves at once to the find() function.
 *
 * \note The Encyclopaedia of Chess Openings only applies to games of standard
 * chess that start from the default starting position.
 */
class LIB_EXPORT EcoNode
{
	public:
		/*! Destroys the node and its subtree. */
		~EcoNode();

		/*!
		 * Returns true if the node is a leaf node; otherwise returns false.
		 * A leaf node is a node that counts as an opening and has an ECO
		 * code and a name.
		 */
		bool isLeaf() const;
		/*!
		 * Returns the node's child node corresponding to \a sanMove, or 0
		 * if no match is found.
		 */
		EcoNode* child(const QString& sanMove) const;
		/*!
		 * Returns the node's ECO code, or an empty string if the node is
		 * an inner node.
		 */
		QString ecoCode() const;
		/*!
		 * Returns the node's opening name, or an empty string if the node
		 * is an inner node.
		 */
		QString opening() const;
		/*!
		 * Returns the node's variation name, or an empty string if the node
		 * is an inner node or doesn't have a variation name.
		 */
		QString variation() const;

		/*! Initializes the ECO tree from the internal opening database. */
		static void initialize();
		/*! Initializes the ECO tree by parsing the PGN games in \a in. */
		static void initialize(PgnStream& in);
		/*!
		 * Returns the root node of the ECO tree.
		 * initialize() is called first if the tree is uninitialized.
		 */
		static const EcoNode* root();
		/*!
		 * Returns the deepest node (closest to the leaves) that matches the
		 * opening sequence in \a moves.
		 */
		static const EcoNode* find(const QVector<PgnGame::MoveData>& moves);
		/*! Writes the ECO tree in binary format to \a fileName. */
		static void write(const QString& fileName);

	private:
		friend LIB_EXPORT QDataStream& operator<<(QDataStream& out, const EcoNode* node);
		friend LIB_EXPORT QDataStream& operator>>(QDataStream& in, EcoNode*& node);

		EcoNode();
		void addChild(const QString& sanMove, EcoNode* child);

		qint16 m_ecoCode;
		qint32 m_opening;
		QString m_variation;

		QMap<QString, EcoNode*> m_children;
};

/*! Writes the node \a node to stream \a out. */
extern LIB_EXPORT QDataStream& operator<<(QDataStream& out, const EcoNode* node);
/*!
 * Reads a node from stream \a in into \a node.
 * \a node should be a null or uninitialized pointer.
 */
extern LIB_EXPORT QDataStream& operator>>(QDataStream& in, EcoNode*& node);

#endif // ECONODE_H
