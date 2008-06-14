#ifndef NOTATION_H
#define NOTATION_H

#include <QChar>
#include <QString>

typedef enum _MoveType
{
	MT_NORMAL, MT_CHECK, MT_MATE, MT_ERROR
} MoveType;

/* Returns true if <word> is a move string (in coordinate notation).
   It doesn't have to be a legal or even a pseudo-legal move though.  */
extern bool isMoveString(const QString& word);

#endif // NOTATION_H

