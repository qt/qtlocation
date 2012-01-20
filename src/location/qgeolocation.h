/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
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
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGEOLOCATION_H
#define QGEOLOCATION_H

#include <QSharedDataPointer>
#include <QString>
#include <QVariant>
#include <QList>
#include "qgeoaddress.h"
#include "qgeocoordinate.h"
#include "qgeoboundingbox.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE



class QGeoLocationPrivate;

class Q_LOCATION_EXPORT QGeoLocation
{
public:
    QGeoLocation();
    QGeoLocation(const QGeoLocation &other);

    ~QGeoLocation();

    QGeoLocation &operator=(const QGeoLocation &other);

    bool operator==(const QGeoLocation &other) const;
    bool operator!=(const QGeoLocation &other) const {
        return !(other == *this);
    }

    QGeoAddress address() const;
    void setAddress(const QGeoAddress &address);
    QGeoCoordinate coordinate() const;
    void setCoordinate(const QGeoCoordinate &position);
    QGeoBoundingBox boundingBox() const;
    void setBoundingBox(const QGeoBoundingBox &box);

private:
    QSharedDataPointer<QGeoLocationPrivate> d;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QGeoLocation)

QT_END_HEADER

#endif
