/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the test suite of the Qt Toolkit.
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
****************************************************************************/

#ifndef PLACEMANAGER_UTILS_H
#define PLACEMANAGER_UTILS_H

#include <QtCore/QString>
#include <QtLocation/QPlaceReply>
#include <QtLocation/qtlocation.h>

#ifndef WAIT_UNTIL
#define WAIT_UNTIL(__expr) \
        do { \
        const int __step = 50; \
        const int __timeout = 10000; \
        if (!(__expr)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && !(__expr); __i+=__step) { \
            QTest::qWait(__step); \
        } \
    } while (0)
#endif

QT_BEGIN_NAMESPACE

class QPlaceManager;
class QPlace;
class QPlaceSearchResult;
class QPlaceSearchRequest;
class QPlaceCategory;
class QPlaceMatchRequest;

QT_END_NAMESPACE

class PlaceManagerUtils : public QObject
{
    Q_OBJECT
public:
    PlaceManagerUtils(QObject *parent = 0);

    static bool doSavePlace(QPlaceManager *manager,
                     const QPlace &place,
                     QPlaceReply::Error expectedError = QPlaceReply::NoError,
                     QString *placeId = 0);

    static void doSavePlaces(QPlaceManager *manager, QList<QPlace> &places);

    //sets the id for saved places
    static void doSavePlaces(QPlaceManager *manager, const QList<QPlace *> &places);

    static bool doSearch(QPlaceManager *manager, const QPlaceSearchRequest &request,
                  QList<QPlaceSearchResult> *results,
             QPlaceReply::Error expectedError = QPlaceReply::NoError);

    static bool doSearch(QPlaceManager *manager, const QPlaceSearchRequest &request,
                  QList<QPlace> *results,
             QPlaceReply::Error expectedError = QPlaceReply::NoError);

    static bool doRemovePlace(QPlaceManager *manager, const QPlace &place,
                       QPlaceReply::Error expectedError = QPlaceReply::NoError);

    static bool doFetchDetails(QPlaceManager *manager,
                        QString placeId,
                        QPlace *place,
                        QPlaceReply::Error expectedError = QPlaceReply::NoError);

    static bool doInitializeCategories(QPlaceManager *manager,
                                QPlaceReply::Error expectedError = QPlaceReply::NoError);

    static bool doSaveCategory(QPlaceManager *manager,
                        const QPlaceCategory &category,
                        const QString &parentId,
                        QPlaceReply::Error expectedError = QPlaceReply::NoError,
                        QString *categoryId = 0);

    static bool doRemoveCategory(QPlaceManager *manager, const QPlaceCategory &category,
                          QPlaceReply::Error expectedError = QPlaceReply::NoError);

    static bool doFetchCategory(QPlaceManager *manager,
                         const QString &categoryId,
                         QPlaceCategory *category,
                         QPlaceReply::Error expectedError = QPlaceReply::NoError);

    static bool doMatch(QPlaceManager *manager,
                 const QPlaceMatchRequest &request,
                 QList<QPlace> *places,
                 QPlaceReply::Error expectedError = QPlaceReply::NoError);

    static bool checkSignals(QPlaceReply *reply, QPlaceReply::Error expectedError,
                      QPlaceManager *manager);

    static bool compare(const QList<QPlace> &actualResults,
                        const QList<QPlace> &expectedResults);

    static void setVisibility(QList<QPlace *>places, QtLocation::Visibility visibility);

    static const int Timeout;

protected:
    bool doSavePlace(const QPlace &place,
                QPlaceReply::Error expectedError = QPlaceReply::NoError,
                QString *placeId = 0) {
        return doSavePlace(placeManager, place, expectedError, placeId);
    }

    void doSavePlaces(QList<QPlace> &places) {
        return doSavePlaces(placeManager, places);
    }

    void doSavePlaces(const QList<QPlace *> &places) {
        return doSavePlaces(placeManager, places);
    }

    bool doRemovePlace(const QPlace &place,
                       QPlaceReply::Error expectedError = QPlaceReply::NoError)
    {
        return doRemovePlace(placeManager, place, expectedError);
    }

    bool doSearch(const QPlaceSearchRequest &request,
                  QList<QPlace> *results,
                  QPlaceReply::Error expectedError = QPlaceReply::NoError) {
        return doSearch(placeManager, request, results,expectedError);
    }

    bool doFetchDetails(QString placeId,
                        QPlace *place,
                        QPlaceReply::Error expectedError = QPlaceReply::NoError) {
        return doFetchDetails(placeManager, placeId, place, expectedError);
    }

    bool doInitializeCategories(QPlaceReply::Error expectedError = QPlaceReply::NoError) {
        return doInitializeCategories(placeManager, expectedError);
    }

    bool doSaveCategory(const QPlaceCategory &category,
                        QPlaceReply::Error expectedError = QPlaceReply::NoError,
                        QString *categoryId = 0) {
        return doSaveCategory(placeManager, category, QString(),
                                            expectedError,categoryId);
    }

    bool doSaveCategory(const QPlaceCategory &category,
                        const QString &parentId,
                        QPlaceReply::Error expectedError = QPlaceReply::NoError,
                        QString *categoryId = 0) {
        return doSaveCategory(placeManager, category, parentId,
                                            expectedError, categoryId);
    }

    bool doRemoveCategory(const QPlaceCategory &category,
                          QPlaceReply::Error expectedError = QPlaceReply::NoError)
    {
        return doRemoveCategory(placeManager, category, expectedError);
    }

    bool doFetchCategory(const QString &categoryId,
                         QPlaceCategory *category,
                         QPlaceReply::Error expectedError = QPlaceReply::NoError) {
        return doFetchCategory(placeManager, categoryId,
                                             category, expectedError);
    }

    bool doMatch(const QPlaceMatchRequest &request,
                 QList<QPlace> *places,
                 QPlaceReply::Error expectedError = QPlaceReply::NoError) {
        return doMatch(placeManager, request,
                                     places, expectedError);
    }

    bool checkSignals(QPlaceReply *reply, QPlaceReply::Error expectedError) {
        return checkSignals(reply, expectedError, placeManager);
    }

    QPlaceManager *placeManager;
};

#endif

