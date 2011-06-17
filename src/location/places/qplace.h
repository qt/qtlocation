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

#ifndef QPLACE_H
#define QPLACE_H

#include <QSharedDataPointer>
#include <QString>
#include <QVariant>
#include <QList>
#include "qmobilityglobal.h"
#include "qplacealternativevalue.h"
#include "qplacebusinessinformation.h"
#include "qplacecategory.h"
#include "qplacecontact.h"
#include "qplacedescription.h"
#include "qplacelocation.h"
#include "qplacerating.h"
#include "qplacepaginationlist.h"
#include "qplacemediaobject.h"
#include "qplacereview.h"

QTM_BEGIN_NAMESPACE

class QPlacePrivate;

class Q_LOCATION_EXPORT QPlace
{
public:
    QPlace();
    QPlace(const QPlace &other);

    virtual ~QPlace();

    QPlace &operator=(const QPlace &other);

    bool operator==(const QPlace &other) const;
    bool operator!=(const QPlace &other) const {
        return !(other == *this);
    }

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
    QPlaceLocation location() const;
    void setLocation(const QPlaceLocation &location);
    QList<QPlaceLocation> alternativeLocations() const;
    void setAlternativeLocations(const QList<QPlaceLocation> &locations);
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

private:
    QSharedDataPointer<QPlacePrivate> d;
};

QTM_END_NAMESPACE

#endif // QPLACE_H
