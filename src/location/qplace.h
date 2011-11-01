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
****************************************************************************/

#ifndef QPLACE_H
#define QPLACE_H

#include <QtCore/QSharedDataPointer>
#include <QtLocation/qtlocation.h>

#include "qgeoaddress.h"
#include "qgeoboundingbox.h"
#include "qgeocoordinate.h"
#include "qgeolocation.h"
#include "qplacecategory.h"
#include "qplacecontent.h"
#include "qplacerating.h"
#include "qplacereview.h"
#include "qplaceattribute.h"
#include "qplacecontactdetail.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE



class QString;
class QPlaceIcon;
class QPlacePrivate;

class Q_LOCATION_EXPORT QPlace
{
public:
    QPlace();
    QPlace(const QPlace &other);
    ~QPlace();

    QPlace& operator=(const QPlace &other);

    bool operator==(const QPlace &other) const;
    bool operator!=(const QPlace &other) const;

    QList<QPlaceCategory> categories() const;
    void setCategories(const QList<QPlaceCategory> &categories);
    QGeoLocation location() const;
    void setLocation(const QGeoLocation &location);
    QPlaceRating rating() const;
    void setRating(const QPlaceRating &rating);
    QPlaceSupplier supplier() const;
    void setSupplier(const QPlaceSupplier &supplier);

    QString attribution() const;
    void setAttribution(const QString &attribution);

    QPlaceIcon icon() const;
    void setIcon(const QPlaceIcon &icon);

    QPlaceContent::Collection content(QPlaceContent::Type type) const;
    void setContent(QPlaceContent::Type type, const QPlaceContent::Collection &content);
    void insertContent(QPlaceContent::Type type, const QPlaceContent::Collection &content);

    int totalContentCount(QPlaceContent::Type type) const;
    void setTotalContentCount(QPlaceContent::Type type, int total);

    QString name() const;
    void setName(const QString &name);
    QString placeId() const;
    void setPlaceId(const QString &placeId);

    QString primaryPhone() const;
    QString primaryFax() const;
    QString primaryEmail() const;
    QUrl primaryWebsite() const;

    bool detailsFetched() const;
    void setDetailsFetched(bool fetched);

    QStringList extendedAttributeTypes() const;
    QPlaceAttribute extendedAttribute(const QString &attributeType) const;
    void setExtendedAttribute(const QString &attributeType, const QPlaceAttribute &attribute);
    void removeExtendedAttribute(const QString &attributeType);

    QStringList contactTypes() const;
    QList<QPlaceContactDetail> contactDetails(const QString &contactType);
    void setContactDetails(const QString &contactType, QList<QPlaceContactDetail> details);
    void appendContactDetail(const QString &contactType, const QPlaceContactDetail &detail);

    QtLocation::Visibility visibility() const;
    void setVisibility(QtLocation::Visibility visibility);

private:
    QSharedDataPointer<QPlacePrivate> d_ptr;

    inline QPlacePrivate *d_func();
    inline const QPlacePrivate *d_func() const;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
