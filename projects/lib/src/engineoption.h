#ifndef ENGINEOPTION_H
#define ENGINEOPTION_H

#include <QString>
#include <QVariant>

class LIB_EXPORT EngineOption
{
	public:
		explicit EngineOption(const QString& name,
				      QVariant::Type valueType,
		                      const QVariant& value = QVariant(),
		                      const QVariant& defaultValue = QVariant(),
		                      const QString& alias = QString());
		virtual ~EngineOption();

		/*! Creates and returns a deep copy of this option. */
		virtual EngineOption* copy() const = 0;

		QVariant::Type valueType() const;
		bool isValid() const;
		virtual bool isValid(const QVariant& value) const = 0;

		QString name() const;
		QVariant value() const;
		QVariant defaultValue() const;
		QString alias() const;

		void setName(const QString& name);
		void setValue(const QVariant& value);
		void setDefaultValue(const QVariant& value);
		void setAlias(const QString& alias);

		virtual QVariant toVariant() const = 0;

	private:
		QVariant::Type m_valueType;
		QString m_name;
		QVariant m_value;
		QVariant m_defaultValue;
		QString m_alias;
};

#endif // ENGINEOPTION_H
