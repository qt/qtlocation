/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qlocationtestutils_p.h"

#if defined(Q_OS_SYMBIAN)
    #include <e32std.h>
#endif

void QLocationTestUtils::uheap_mark()
{
#if defined(Q_OS_SYMBIAN)
    __UHEAP_MARK;
#endif
}

void QLocationTestUtils::uheap_mark_end()
{
#if defined(Q_OS_SYMBIAN)
    __UHEAP_MARKEND;
#endif
}

bool QLocationTestUtils::hasDefaultSource()
{
#if defined(Q_OS_SYMBIAN)
    return true;
#elif defined (Q_OS_WINCE)
    return true;
#else 
    return false;    
#endif
}

bool QLocationTestUtils::hasDefaultMonitor()
{
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5)
    return true;
#else
    return false;
#endif
}

QString QLocationTestUtils::addNmeaChecksumAndBreaks(const QString &sentence)
{
    Q_ASSERT(sentence[0] == '$' && sentence[sentence.length()-1] == '*');

    // XOR byte value of all characters between '$' and '*'
    int result = 0;
    for (int i=1; i<sentence.length()-1; i++)
        result ^= sentence[i].toAscii();
    QString sum;
    sum.sprintf("%02x", result);
    return sentence + sum + "\r\n";
}

QString QLocationTestUtils::createRmcSentence(const QDateTime &dt)
{
    QString time = dt.toString("hhmmss.zzz");
    QString date = dt.toString("ddMMyy");
    QString nmea = QString("$GPRMC,%1,A,2730.83609,S,15301.87844,E,0.7,9.0,%2,11.2,W,A*")
        .arg(time).arg(date);
    return addNmeaChecksumAndBreaks(nmea);
}

QString QLocationTestUtils::createGgaSentence(const QTime &time)
{
    QString nmea = QString("$GPGGA,%1,2734.76859,S,15305.99361,E,1,04,3.5,49.4,M,39.2,M,,*")
            .arg(time.toString("hhmmss.zzz"));
    return addNmeaChecksumAndBreaks(nmea);
}

QString QLocationTestUtils::createGgaSentence(int lat, int lng, const QTime &time) {
    QString nmea = QString("$GPGGA,%1,%200.00000,S,%300.,E,1,04,3.5,49.4,M,39.2,M,,*")
            .arg(time.toString("hhmmss.zzz")).arg(lat).arg(lng);
    return addNmeaChecksumAndBreaks(nmea);
}

QString QLocationTestUtils::createZdaSentence(const QDateTime &dt)
{
    QString time = dt.toString("hhmmss.zzz");
    QString nmea = QString("$GPZDA,%1,%2,%3,%4,,*")
        .arg(time).arg(dt.toString("dd")).arg(dt.toString("MM")).arg(dt.toString("yyyy"));
    return addNmeaChecksumAndBreaks(nmea);
}
