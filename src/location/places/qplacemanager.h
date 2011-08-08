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

#ifndef QPLACEMANAGER_H
#define QPLACEMANAGER_H

#include "qmobilityglobal.h"
#include "qplacemediaobject.h"
#include "qplacemediareply.h"
#include "qplacequery.h"
#include "qplacesavereply.h"
#include "qplacereply.h"
#include "qplacesearchreply.h"
#include "qplacedetailsreply.h"
#include "qplacereviewreply.h"
#include "qplacetextpredictionreply.h"

#include <QLocale>
#include <QVector>
#include <QString>
#include <QObject>

QT_BEGIN_NAMESPACE

class QPlaceManagerEngine;
class QPlaceSearchQuery;

class QPlaceManagerPrivate;
class Q_LOCATION_EXPORT QPlaceManager : public QObject
{
    Q_OBJECT
public:
    enum ConnectivityMode {
        OfflineMode = 0x0001,
        OnlineMode = 0x0002,
        HybridMode = 0x0003
    };

    Q_DECLARE_FLAGS(ConnectivityModes, ConnectivityMode)

    enum VisibilityScope {
        NoScope = 0x0000,
        PublicScope = 0x0001,
        PrivateScope = 0x0002,
        PublicAndPrivateScope = 0x0007
    };

    Q_DECLARE_FLAGS(VisibilityScopes, VisibilityScope)

    enum ManagerFeature {
        ImportFeature,
        ExportFeature,
        CheckInFeature,
        PostMediaFeature,
        PostRatingFeature,
        SuggestionFeature,
        ReportPlaceFeature,
        AuthenticationFeature,
        CreatePlaceFeature,
        UpdatePlaceFeature
    };

    enum Error {
        NoError,
        NotSupportedError
    };

    ~QPlaceManager();

    QString managerName() const;
    int managerVersion() const;

    QPlaceDetailsReply *getPlaceDetails(const QString &placeId) const;
    QPlaceReply *postRating(const QString &placeId, qreal value);

    QPlaceReviewReply *getReviews(const QGeoPlace &place, const QPlaceQuery &query) const;

    QPlaceMediaReply *getMedia(const QGeoPlace &place, const QPlaceQuery &query) const;

    QPlaceSearchReply *searchForPlaces(const QPlaceSearchQuery &query) const;

    QPlaceSearchReply *recommendations(const QGeoPlace &place, const QPlaceSearchQuery &query) const;

    QPlaceTextPredictionReply *textPredictions(const QPlaceSearchQuery &query) const;

    ConnectivityModes connectivityMode() const;
    void setConnectivityMode(ConnectivityModes connectivityMode);
    ConnectivityModes supportedConnectivityModes() const;

    QPlaceSaveReply *savePlace(const QGeoPlace &place, VisibilityScope scope = QPlaceManager::NoScope);
    VisibilityScopes supportedSaveVisibilityScopes();

    QPlaceReply *removePlace(const QGeoPlace &place);

    QPlaceReply *initializeCategories();
    QList<QPlaceCategory> categories() const;

    static QStringList availableManagers();

    QLocale locale() const;
    void setLocale(const QLocale &locale);

Q_SIGNALS:
    void finished(QPlaceReply *reply);
    void error(QPlaceReply *, QPlaceReply::Error error, const QString &errorString = QString());

private:
    QPlaceManager(QPlaceManagerEngine *engine, QObject *parent = 0);

    Q_DISABLE_COPY(QPlaceManager)
    QPlaceManagerPrivate* d;
    Q_DECLARE_PRIVATE(QPlaceManager)

    friend class QGeoServiceProvider;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QPlaceManager::VisibilityScopes);
Q_DECLARE_OPERATORS_FOR_FLAGS(QPlaceManager::ConnectivityModes);

QT_END_NAMESPACE

#endif // QPLACEMANAGER_H
