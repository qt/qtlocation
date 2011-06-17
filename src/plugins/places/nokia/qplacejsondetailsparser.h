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
** This file is part of the Ovi services plugin for the Maps and
** Navigation API.  The use of these services, whether by use of the
** plugin or by other means, is governed by the terms and conditions
** described by the file OVI_SERVICES_TERMS_AND_CONDITIONS.txt in
** this package, located in the directory containing the Ovi services
** plugin source code.
**
****************************************************************************/

#ifndef QPLACEJSONDETAILSPARSER_H
#define QPLACEJSONDETAILSPARSER_H

#include <QObject>
#include <QList>

#include <qplace.h>

class QScriptEngine;
class QScriptValue;

QTM_BEGIN_NAMESPACE

class QPlaceJSonDetailsParser : public QObject
{
    Q_OBJECT
public:
    enum Error {
        NoError,
        ParsingError
    };

    explicit QPlaceJSonDetailsParser(QObject *parent = 0);
    virtual ~QPlaceJSonDetailsParser();

    static QPlace buildPlace(const QScriptValue &place);
    QPlace result();

signals:
    void finished(const QPlaceJSonDetailsParser::Error &error, const QString &errorMessage);

public slots:
    void processData(const QString &data);

private:
    static void buildPlace(const QScriptValue &place, QPlace *targetPlace);
    static void processMainProvider(const QScriptValue &place, QPlace *targetPlace);
    static void processContacts(const QScriptValue &contacts, QPlace *targetPlace);
    static void processCategories(const QScriptValue &categories, QPlace *targetPlace);
    static QPlaceCategory processCategory(const QScriptValue &category);
    static void processRatings(const QScriptValue &ratings, QPlace *targetPlace);
    static QPlaceRating *processRating(const QScriptValue &rating);
    static void processAddress(const QScriptValue &address, QPlaceLocation *location);
    static void processLocations(const QScriptValue &locations, QPlace *targetPlace);
    static QPlaceLocation processLocation(const QScriptValue &location);
    static void processTags(const QScriptValue &locations, QPlace *targetPlace);
    static void processNames(const QScriptValue &locations, QPlace *targetPlace);
    static QPlaceAlternativeValue *processName(const QScriptValue &nameValue);
    static void processPremiumContents(const QScriptValue &content, QPlace *targetPlace);
    static void processPremiumVersion(const QScriptValue &content, QPlace *targetPlace);
    static void processPremiumContent(const QScriptValue &content, QPlace *targetPlace);
    static void processPremiumContentDescription(const QScriptValue &content,
                                                 const QPlaceSupplier &supplier,
                                                 QPlace *targetPlace);
    static void processPremiumContentMediaObjects(const QScriptValue &content,
                                                  const QPlaceSupplier &supplier,
                                                  QPlace *targetPlace);
    static QPlaceMediaObject *processPremiumContentMediaObject(const QScriptValue &content);
    static void processAdContent(const QScriptValue &content, QPlace *targetPlace);
    static void processAdContentPackages(const QScriptValue &content, QPlace *targetPlace);
    static void processAdContentDescriptions(const QScriptValue &content, QPlace *targetPlace);
    static QPlaceDescription *processAdContentDescription(const QScriptValue &content);
    static void processAdContentMediaObjects(const QScriptValue &content, QPlace *targetPlace);
    static QPlaceMediaObject *processAdContentMediaObject(const QScriptValue &content);
    static void processAdContentPaymentMethods(const QScriptValue &content, QPlace *targetPlace);
    static QString processAdContentPaymentMethod(const QScriptValue &content);
    static void processAdContentBusinessHours(const QScriptValue &content, QPlace *targetPlace);
    static void processAdContentClosingsNotes(const QScriptValue &content, QPlace *targetPlace);
    static QString processAdContentClosingsNote(const QScriptValue &content);
    static void processAdContentOpeningHours(const QScriptValue &content, QPlace *targetPlace);
    static QPlaceWeekdayHours *processAdContentOpeningHoursElement(const QScriptValue &content);
    static void processAdContentOpeningNotes(const QScriptValue &content, QPlace *targetPlace);
    static QString processAdContentOpeningNote(const QScriptValue &content);

private:
    QScriptEngine *engine;
    QPlace place;
};

QTM_END_NAMESPACE

#endif // QPLACEJSONDETAILSPARSER_H
