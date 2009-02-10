#ifndef VARIANT_H
#define VARIANT_H

class QString;

namespace Chess {

/*!
 * \brief Chess variant
 */
class LIB_EXPORT Variant
{
	public:
		/*! Variant code. */
		enum Code
		{
			//! No chess variant.
			NoVariant,
			//! Standard Chess.
			Standard,
			//! Capablanca Chess.
			Capablanca,
			//! Gothic Chess.
			Gothic,
			//! Ficher Random Chess.
			Fischerandom,
			//! Capablanca Random Chess.
			Caparandom
		};
		
		Variant(Code code = NoVariant);
		Variant(const QString& str);
		
		/*! Returns true if \a other is the same as this variant. */
		bool operator==(const Variant& other) const;
		/*! Returns true if \a otherCode is the same as this variant's code. */
		bool operator==(Code otherCode) const;
		/*! Returns true if \a other different from this variant. */
		bool operator!=(const Variant& other) const;
		/*! Returns true if \a otherCode is different from this variant's code. */
		bool operator!=(Code otherCode) const;
		
		Code code() const;
		/*! Returns true if the variant is NoVariant. */
		bool isNone() const;
		bool isRandom() const;
		bool isStandard() const;
		bool isCapablanca() const;
		int boardWidth() const;
		int boardHeight() const;
		QString toString() const;
		QString startingFen() const;

	private:
		Code m_code;
};

} // namespace Chess
#endif // VARIANT_H
