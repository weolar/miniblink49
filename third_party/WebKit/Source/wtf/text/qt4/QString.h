
#ifndef QSTRING_H
#define QSTRING_H

#include "QtGlobal.h"
#include "QChar.h"

class Q_CORE_EXPORT QString
{
public:
    inline QString();

    //QChar toQCharLower() const { return QChar(*this).toLower(); }
    //QChar toQCharUpper() const { return QChar(*this).toUpper(); }

    QString toLower() const Q_REQUIRED_RESULT;
    QString toUpper() const Q_REQUIRED_RESULT;

};


#endif // QSTRING_H