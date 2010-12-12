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

#ifndef SQUAREINFO_H
#define SQUAREINFO_H

#include <QMetaType>
#include <QString>


/*!
 * \brief A class for providing information about a chess square
 * and its contents.
 *
 * SquareInfo objects are used by ChessboardModel to inform
 * ChessboardView about the type, color and contents of a square.
 * QVariant objects are used to carry the information.
 */
class SquareInfo
{
	public:
		/*! The type of the square. */
		enum TypeFlag
		{
			NormalSquare = 0,	//!< Normal square (default)
			SourceSquare = 1,	//!< Source square
			TargetSquare = 2,	//!< Target square
			HighlightedSquare = 4	//!< Highlighted square
		};

		/*! The color of the square. */
		enum Color
		{
			BackgroundColor, //!< Background color (default)
			HoldingsColor,	 //!< Piece holdings slot color
			LightColor,	 //!< Light color
			DarkColor	 //!< Dark color
		};

		/*! Creates a new SquareInfo object. */
		SquareInfo();
		/*! Creates a new SquareInfo object. */
		SquareInfo(int flags,
			   Color color,
			   int pieceCount,
			   const QString& pieceSymbol);

		/*! Returns the type flags of the square. */
		int flags() const;
		/*! Returns the color of the square. */
		Color color() const;
		/*!
		 * Returns the number of pieces on the square/slot.
		 * \note All pieces on the square are of the same type.
		 */
		int pieceCount() const;
		/*! Returns the symbol of the piece(s). */
		QString pieceSymbol() const;

		/*! Sets the square's type flags to \a flags. */
		void setFlags(int flags);
		/*! Sets the square's color to \a color. */
		void setColor(Color color);
		/*!
		 * Sets the piece count to \a pieceCount.
		 * \note must be a non-negative value.
		 */
		void setPieceCount(int count);
		/*! Sets the piece symbol to \a symbol. */
		void setPieceSymbol(const QString& symbol);

	private:
		int m_flags;
		Color m_color;
		int m_pieceCount;
		QString m_pieceSymbol;
};
Q_DECLARE_METATYPE(SquareInfo)

#endif // SQUAREINFO_H
