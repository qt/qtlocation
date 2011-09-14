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

#include <QSharedDataPointer>
#include <QVariant>

#include "qgeoaddress.h"
#include "qgeoboundingbox.h"
#include "qgeocoordinate.h"
#include "qgeolocation.h"
#include "qplacecategory.h"
#include "qplacecontent.h"
#include "qplacerating.h"
#include "qplacereview.h"
#include "qplaceattribute.h"

QT_BEGIN_NAMESPACE

class QString;
class QPlacePrivate;

class Q_LOCATION_EXPORT QPlace
{
public:
    typedef QMap<QString, QPlaceAttribute> ExtendedAttributes;

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
    QList<QPlaceSupplier> suppliers() const;
    void setSuppliers(const QList<QPlaceSupplier> &data);

    QPlaceContent::Collection content(QPlaceContent::Type type) const;
    void setContent(QPlaceContent::Type type, const QPlaceContent::Collection &content);
    void addContent(QPlaceContent::Type type, const QPlaceContent::Collection &content);
    int contentCount(QPlaceContent::Type type) const;
    void setContentCount(QPlaceContent::Type type, int);

    QString name() const;
    void setName(const QString &name);
    QString placeId() const;
    void setPlaceId(const QString &placeId);

    QString primaryPhone() const;
    void setPrimaryPhone(const QString &phone);

    QString primaryFax() const;
    void setPrimaryFax(const QString fax);

    QString primaryEmail() const;
    void setPrimaryEmail(const QString &email);

    QUrl primaryUrl() const;
    void setPrimaryUrl(const QUrl &url);

    bool detailsFetched() const;
    void setDetailsFetched(bool fetched);

    QPlace::ExtendedAttributes extendedAttributes() const;
    void setExtendedAttributes(const QPlace::ExtendedAttributes &attributes);
    void insertExtendedAttribute(const QString &key, const QPlaceAttribute &);

protected:
    QPlace(QPlacePrivate *dd);
    QSharedDataPointer<QPlacePrivate> d_ptr;

private:
    QPlacePrivate* d_func();
    const QPlacePrivate* d_func() const;
};

QT_END_NAMESPACE

#endif
