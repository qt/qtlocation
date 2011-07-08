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

#ifndef QGEOPLACE_H
#define QGEOPLACE_H

#include "qmobilityglobal.h"
#include <QSharedDataPointer>
#include <QVariant>

#include "qgeoaddress.h"
#include "qgeoboundingbox.h"
#include "qgeocoordinate.h"
#include "qgeolocation.h"
#include "qplacealternativevalue.h"
#include "qplacebusinessinformation.h"
#include "qplacecategory.h"
#include "qplacecontact.h"
#include "qplacedescription.h"
#include "qplacerating.h"
#include "qplacepaginationlist.h"
#include "qplacemediaobject.h"
#include "qplacereview.h"

class QString;

QTM_BEGIN_NAMESPACE

class QLandmark;

class QGeoPlacePrivate;

class Q_LOCATION_EXPORT QGeoPlace
{
public:
    QGeoPlace();
    QGeoPlace(const QGeoPlace &other);
    virtual ~QGeoPlace();

    QGeoPlace& operator=(const QGeoPlace &other);

    bool operator==(const QGeoPlace &other) const;
    bool operator!=(const QGeoPlace &other) const;

    bool isLandmark() const;

    QGeoBoundingBox viewport() const;
    void setViewport(const QGeoBoundingBox &viewport);

    QGeoCoordinate coordinate() const;
    void setCoordinate(const QGeoCoordinate &coordinate);

    QGeoAddress address() const;
    void setAddress(const QGeoAddress &address);

    QVariantHash additionalData() const;
    void setAdditionalData(const QVariantHash &data);
    QList<QPlaceAlternativeValue> alternativeNames() const;
    void setAlternativeNames(const QList<QPlaceAlternativeValue> &alternativeNames);
    double placeScore() const;
    void setPlaceScore(const double &data);
    QPlaceBusinessInformation businessInformation() const;
    void setBusinessInformation(const QPlaceBusinessInformation &business);
    QList<QPlaceCategory> categories() const;
    void setCategories(const QList<QPlaceCategory> &categories);
    QList<QPlaceContact> contacts() const;
    void setContacts(const QList<QPlaceContact> &contacts);
    QList<QPlaceDescription> descriptions() const;
    void setDescriptions(const QList<QPlaceDescription> &descriptions);
    QGeoLocation location() const;
    void setLocation(const QGeoLocation &location);
    QList<QGeoLocation> alternativeLocations() const;
    void setAlternativeLocations(const QList<QGeoLocation> &locations);
    QPlaceRating rating() const;
    void setRating(const QPlaceRating &rating);
    QList<QPlaceSupplier> suppliers() const;
    void setSuppliers(const QList<QPlaceSupplier> &data);
    QStringList feeds() const;
    void setFeeds(const QStringList &feeds);
    QPlacePaginationList<QPlaceMediaObject> media() const;
    void setMedia(const QPlacePaginationList<QPlaceMediaObject> &media);
    int mediaCount() const;
    void setMediaCount(const int &count);
    QString name() const;
    void setName(const QString &name);
    QString placeId() const;
    void setPlaceId(const QString &placeId);
    QPlacePaginationList<QPlaceReview> reviews() const;
    void setReviews(const QPlacePaginationList<QPlaceReview> &reviews);
    int reviewCount() const;
    void setReviewCount(const int &count);
    QString shortDescription() const;
    void setShortDescription(const QString &description);
    QStringList tags() const;
    void setTags(const QStringList &tags);

protected:
    QGeoPlace(QGeoPlacePrivate *dd);
    QSharedDataPointer<QGeoPlacePrivate> d_ptr;

private:
    QGeoPlacePrivate* d_func();
    const QGeoPlacePrivate* d_func() const;
    friend class QLandmark;
};

QTM_END_NAMESPACE

#endif
