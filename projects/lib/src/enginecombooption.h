#ifndef ENGINECOMBOOPTION_H
#define ENGINECOMBOOPTION_H

#include "engineoption.h"
#include <QStringList>

class LIB_EXPORT EngineComboOption : public EngineOption
{
	public:
		EngineComboOption();
		EngineComboOption(const QString& name,
				  const QVariant& value = QVariant(),
				  const QVariant& defaultValue = QVariant(),
				  const QStringList& choices = QStringList());

		EngineOption* copy() const;

		bool isValid(const QVariant& value) const;

		QStringList choices() const;
		void setChoices(const QStringList& choices);

		QVariant toQVariant() const;

	private:
		QStringList m_choices;
};

Q_DECLARE_METATYPE(EngineComboOption)

#endif // ENGINECOMBOOPTION_H
