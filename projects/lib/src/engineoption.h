#ifndef ENGINEOPTION_H
#define ENGINEOPTION_H

#include <QString>
#include <QVariant>

class LIB_EXPORT EngineOption
{
	public:
		explicit EngineOption(const QString& name,
				      const QVariant& value = QVariant(),
				      const QVariant& defaultValue = QVariant());
		virtual ~EngineOption() {}

		/*! Creates and returns a deep copy of this option. */
		virtual EngineOption* copy() const = 0;

		bool isValid() const;
		virtual bool isValid(const QVariant& value) const = 0;

		QString name() const;
		QVariant value() const;
		QVariant defaultValue() const;

		void setName(const QString& name);
		void setValue(const QVariant& value);
		void setDefaultValue(const QVariant& value);

		virtual QVariant toVariant() const = 0;

	protected:
		QString m_name;
		QVariant m_value;
		QVariant m_defaultValue;
};

#endif // ENGINEOPTION_H
