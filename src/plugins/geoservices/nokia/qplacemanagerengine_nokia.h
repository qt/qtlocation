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

#ifndef QPLACEMANAGERENGINE_NOKIA_H
#define QPLACEMANAGERENGINE_NOKIA_H

#include <qplacemanagerengine.h>
#include <qgeoserviceprovider.h>

QT_BEGIN_NAMESPACE

class QPlaceManagerEngineNokia : public QPlaceManagerEngine
{
    Q_OBJECT
public:

    QPlaceManagerEngineNokia(const QMap<QString, QVariant> &parameters,
                             QGeoServiceProvider::Error *error,
                             QString *errorString);
    ~QPlaceManagerEngineNokia();

    QPlaceDetailsReply *getPlaceDetails(const QString &placeId);

    QPlaceMediaReply *getMedia(const QGeoPlace &place, const QPlaceQuery &query);

    QPlaceReply *postRating(const QString &placeId, qreal value);

    QPlaceReviewReply *getReviews(const QGeoPlace &place, const QPlaceQuery &query);

    QPlaceSearchReply *searchForPlaces(const QPlaceSearchQuery &query, QPlaceManager::VisibilityScope scope);
    QPlaceManager::VisibilityScopes supportedSearchVisibilityScopes() const;

    QPlaceSearchReply *recommendations(const QGeoPlace &place, const QPlaceSearchQuery &query);
    QPlaceTextPredictionReply *textPredictions(const QPlaceSearchQuery &query);

    QPlaceManager::ConnectivityModes connectivityMode() const;
    void setConnectivityMode(QPlaceManager::ConnectivityModes connectivityMode);
    QPlaceManager::ConnectivityModes supportedConnectivityModes() const;

    QPlaceReply *savePlace(QGeoPlace *place, QPlaceManager::VisibilityScope scope);
    QPlaceManager::VisibilityScopes supportedSaveVisibilityScopes() const;

    QPlaceReply *removePlace(const QGeoPlace &place);

    QPlaceReply *initializeCategories();
    QList<QPlaceCategory> categories() const;

private slots:
    void processingError(QPlaceReply *reply, const QPlaceReply::Error &error, const QString &errorMessage);
    void processingFinished(QPlaceReply *reply);
};

QT_END_NAMESPACE

#endif // QPLACEMANAGERENGINEIMPL_H
