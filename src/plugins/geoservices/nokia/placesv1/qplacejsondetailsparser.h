/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtLocation module of the Qt Toolkit.
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
**
** $QT_END_LICENSE$
**
** This file is part of the Nokia services plugin for the Maps and
** Navigation API.  The use of these services, whether by use of the
** plugin or by other means, is governed by the terms and conditions
** described by the file NOKIA_TERMS_AND_CONDITIONS.txt in
** this package, located in the directory containing the Nokia services
** plugin source code.
**
****************************************************************************/

#ifndef QPLACEJSONDETAILSPARSER_H
#define QPLACEJSONDETAILSPARSER_H

#include <QObject>
#include <QList>

#include <qplace.h>
#include <qplaceimage.h>
#include "qplacejsonparser_p.h"

QT_BEGIN_NAMESPACE

class QJSEngine;
class QJSValue;
class QPlaceEditorial;

class QPlaceJSonDetailsParser : public QPlaceJSonParser
{
    Q_OBJECT
public:
    explicit QPlaceJSonDetailsParser(QPlaceManager *manager, QObject *parent = 0);
    virtual ~QPlaceJSonDetailsParser();

    static QPlace buildPlace(const QJSValue &place, QPlaceManager *manager);
    QPlace result();

private:
    void processJSonData(const QJSValue &sv);
    static void buildPlace(const QJSValue &place, QPlace *targetPlace);
    static void processMainProvider(const QJSValue &place, QPlace *targetPlace);
    static void processContacts(const QJSValue &contacts, QPlace *targetPlace);
    static void processCategories(const QJSValue &categories, QPlace *targetPlace);
    static QPlaceCategory processCategory(const QJSValue &category);
    static void processRatings(const QJSValue &ratings, QPlace *targetPlace);
    static QPlaceRatings *processRating(const QJSValue &rating);
    static void processAddress(const QJSValue &address, QGeoLocation *location);
    static void processLocation(const QJSValue &location, QPlace *targetPlace);
    static void processTags(const QJSValue &locations, QPlace *targetPlace);
    static void processNames(const QJSValue &locations, QPlace *targetPlace);
    static QString processName(const QJSValue &nameValue);
    static void processPremiumContents(const QJSValue &content, QPlace *targetPlace);
    static void processPremiumVersion(const QJSValue &content, QPlace *targetPlace);
    static void processPremiumContent(const QJSValue &content, QPlace *targetPlace);
    static void processPremiumContentDescription(const QJSValue &content,
                                                 const QPlaceSupplier &supplier,
                                                 QPlace *targetPlace);
    static void processPremiumContentMediaObjects(const QJSValue &content,
                                                  const QPlaceSupplier &supplier,
                                                  QPlace *targetPlace);
    static QPlaceImage *processPremiumContentMediaObject(const QJSValue &content);
    static void processAdContent(const QJSValue &content, QPlace *targetPlace);
    static void processAdContentDescriptions(const QJSValue &content, QPlace *targetPlace);
    static QPlaceEditorial *processAdContentDescription(const QJSValue &content);
    static void processAdContentMediaObjects(const QJSValue &content, QPlace *targetPlace);
    static QPlaceImage *processAdContentMediaObject(const QJSValue &content);
    static void processAdContentPaymentMethods(const QJSValue &content, QPlace *targetPlace);
    static QString processAdContentPaymentMethod(const QJSValue &content);
    static void processAdContentBusinessHours(const QJSValue &content, QPlace *targetPlace);
    static void processAdContentClosingsNotes(const QJSValue &content, QPlace *targetPlace);
    static QString processAdContentClosingsNote(const QJSValue &content);
    static void processAdContentOpeningHours(const QJSValue &content, QPlace *targetPlace);
    static void processAdContentOpeningHoursElement(const QJSValue &content);
    static void processAdContentOpeningNotes(const QJSValue &content, QPlace *targetPlace);
    static QString processAdContentOpeningNote(const QJSValue &content);

private:
    QPlace place;
    QPlaceManager *m_manager;
};

QT_END_NAMESPACE

#endif // QPLACEJSONDETAILSPARSER_H
