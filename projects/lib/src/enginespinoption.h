#ifndef ENGINESPINOPTION_H
#define ENGINESPINOPTION_H

#include "engineoption.h"

class LIB_EXPORT EngineSpinOption : public EngineOption
{
	public:
		EngineSpinOption(const QString& name,
		                 const QVariant& value = QVariant(),
		                 const QVariant& defaultValue = QVariant(),
		                 int min = 0,
		                 int max = 0,
		                 const QString& alias = QString());

		EngineOption* copy() const;

		bool isValid(const QVariant& value) const;

		int min() const;
		int max() const;

		void setMin(int min);
		void setMax(int max);

		QVariant toVariant() const;

	private:
		int m_min;
		int m_max;
};

#endif // ENGINESPINOPTION_H
