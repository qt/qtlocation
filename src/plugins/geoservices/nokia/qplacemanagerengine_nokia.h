/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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

#include <qplacecontent.h>
#include <qplacemanagerengine.h>
#include <qgeoserviceprovider.h>

QT_BEGIN_NAMESPACE
class QPlaceContentReply;

class QPlaceManagerEngineNokia : public QPlaceManagerEngine
{
    Q_OBJECT
public:

    QPlaceManagerEngineNokia(const QMap<QString, QVariant> &parameters,
                             QGeoServiceProvider::Error *error,
                             QString *errorString);
    ~QPlaceManagerEngineNokia();

    QPlaceDetailsReply *getPlaceDetails(const QString &placeId);

    QPlaceContentReply *getContent(const QPlace &place, const QPlaceContentRequest &request);

    QPlaceSearchReply *search(const QPlaceSearchRequest &query);

    QPlaceSearchReply *recommendations(const QPlace &place, const QPlaceSearchRequest &query);
    QPlaceTextPredictionReply *textPredictions(const QPlaceSearchRequest &query);

    QPlaceIdReply *savePlace(const QPlace &place);
    QPlaceIdReply *removePlace(const QPlace &place);

    QPlaceIdReply *saveCategory(const QPlaceCategory &category, const QString &parentId);
    QPlaceIdReply *removeCategory(const QString &categoryId);

    QPlaceReply *initializeCategories();
    QString parentCategoryId(const QString &/*categoryId*/) const {return QString();}
    virtual QStringList childrenCategoryIds(const QString &/*categoryId*/) const {return QStringList();}
    virtual QPlaceCategory category(const QString &/*categoryId*/) const {return QPlaceCategory();}
    QList<QPlaceCategory> childCategories(const QString &parentId) const;

    QLocale locale() const;
    void setLocale(const QLocale &locale);

    QUrl constructIconUrl(const QPlaceIcon &icon, const QSize &size, QPlaceIcon::IconFlags flags);
    QPlaceManager::ManagerFeatures supportedFeatures() const;

private slots:
    void processingError(QPlaceReply *reply, const QPlaceReply::Error &error, const QString &errorMessage);
    void processingFinished(QPlaceReply *reply);
};

QT_END_NAMESPACE

#endif // QPLACEMANAGERENGINEIMPL_H
