#ifndef ENGINECOMBOOPTION_H
#define ENGINECOMBOOPTION_H

#include "engineoption.h"
#include <QStringList>

class LIB_EXPORT EngineComboOption : public EngineOption
{
	public:
		EngineComboOption(const QString& name,
		                  const QVariant& value = QVariant(),
		                  const QVariant& defaultValue = QVariant(),
		                  const QStringList& choices = QStringList(),
		                  const QString& alias = QString());

		// Inherited from EngineOption
		virtual EngineOption* copy() const;
		virtual bool isValid(const QVariant& value) const;
		virtual QVariant toVariant() const;

		QStringList choices() const;
		void setChoices(const QStringList& choices);

	private:
		QStringList m_choices;
};

#endif // ENGINECOMBOOPTION_H
