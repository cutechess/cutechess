#ifndef ENGINESPINOPTION_H
#define ENGINESPINOPTION_H

#include "engineoption.h"

class LIB_EXPORT EngineSpinOption : public EngineOption
{
	public:
		EngineSpinOption();
		EngineSpinOption(const QString& name,
				 const QVariant& value = QVariant(),
				 const QVariant& defaultValue = QVariant(),
				 int min = 0,
				 int max = 0);

		bool isValid(const QVariant& value) const;

		int min() const;
		int max() const;

		void setMin(int min);
		void setMax(int max);

	private:
		int m_min;
		int m_max;
};

Q_DECLARE_METATYPE(EngineSpinOption)

#endif // ENGINESPINOPTION_H
