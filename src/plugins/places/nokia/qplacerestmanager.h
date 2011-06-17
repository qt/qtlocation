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

#ifndef QPLACERESTMANAGER_H
#define QPLACERESTMANAGER_H

#include <QObject>

#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include <qmobilityglobal.h>
#include <qplacesearchquery.h>
#include "qplacerestreply.h"

QTM_BEGIN_NAMESPACE

class QPlaceRestManager : public QObject
{
    Q_OBJECT

public:
    static QPlaceRestManager *instance();

    QPlaceRestReply *sendPlaceRequest(const QString &placeId);
    QPlaceRestReply *sendPlaceImagesRequest(const QString &placeId, const QPlaceQuery &params);
    QPlaceRestReply *sendPlaceReviewRequest(const QString &placeId, const QPlaceQuery &params);
    QPlaceRestReply *sendRecommendationRequest(const QPlaceSearchQuery &query, const QString &userId);
    QPlaceRestReply *sendCategoriesTreeRequest();
    QPlaceRestReply *sendSearchRequest(const QPlaceSearchQuery &query);
    QPlaceRestReply *sendSearchByCategoryRequest(const QPlaceSearchQuery &query);
    QPlaceRestReply *sendSuggestionRequest(const QPlaceSearchQuery &query);

    QPlaceRestReply *postRatingRequest(const QString &placeId, const QString &userId, const int &value);

private:
    explicit QPlaceRestManager(QObject *parent = 0);
    QPlaceRestReply *sendGeneralRequest(const QUrl &url);
    //TODO: remove when engine is refactored out
    QPlaceRestReply *sendGeneralRequest(const QString &url) {
        return sendGeneralRequest(QUrl(url));
    }

    QString prepareSearchRequest(const QPlaceSearchQuery &query);

private:
    QNetworkAccessManager *mManager;
    static QPlaceRestManager *mInstance;
};

QTM_END_NAMESPACE

#endif // QPLACERESTMANAGER_H
