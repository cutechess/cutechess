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

#include "epdrecord.h"
#include <QTextStream>

EpdRecord::EpdRecord()
{
}

bool EpdRecord::parse(QTextStream& stream)
{
	enum TokenType
	{
		SeparatorToken,
		OpcodeToken,
		OperandToken
	};

	m_fen.clear();
	m_operations.clear();

	// Parse FEN
	for (int i = 0; i < 4; i++)
	{
		QString tmp;
		stream >> tmp;
		if (stream.status() != QTextStream::Ok)
			return false;

		m_fen.append(tmp);
		if (i < 3)
			m_fen.append(" ");
	}

	// Parse the operations
	QString opcode;
	QString operand;
	TokenType type = SeparatorToken;
	bool inQuotes = false;
	bool ok = true;
	while (stream.status() == QTextStream::Ok)
	{
		QChar c;
		stream >> c;

		switch (c.toLatin1())
		{
		case '\n':
			return true;
		case ' ':
		case '\t':
			if (inQuotes && type == OperandToken)
				operand.append(c);
			else if (type != SeparatorToken)
			{
				if (type == OpcodeToken)
					type = OperandToken;
				else
				{
					m_operations[opcode].append(operand);
					operand.clear();
				}
			}
			break;
		case '\"':
			if (type == OpcodeToken)
			{
				ok = false;
				break;
			}
			else if (type == SeparatorToken)
			{
				if (opcode.isEmpty())
				{
					ok = false;
					break;
				}
				type = OperandToken;
			}
			else if (type == OperandToken
			     &&  !inQuotes
			     &&  !operand.isEmpty())
			{
				ok = false;
				break;
			}
			inQuotes = !inQuotes;
			break;
		case ';':
			if (!inQuotes)
			{
				if (opcode.isEmpty())
				{
					ok = false;
					break;
				}
				m_operations[opcode].append(operand);
				opcode.clear();
				operand.clear();
				type = SeparatorToken;
			}
			break;
		default:
			if (type == SeparatorToken)
				type = OpcodeToken;
			if (type == OpcodeToken)
				opcode.append(c);
			else
				operand.append(c);
			break;
		}

		if (!ok)
		{
			while (!stream.atEnd() && c != '\n')
				stream >> c;
			return false;
		}
	}

	return true;
}

bool EpdRecord::hasOpcode(const QString& opcode) const
{
	return m_operations.contains(opcode);
}

QString EpdRecord::fen() const
{
	return m_fen;
}

QStringList EpdRecord::operands(const QString& opcode) const
{
	return m_operations.value(opcode);
}
