/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGEOCIRCLE_H
#define QGEOCIRCLE_H

#include <QtLocation/QGeoShape>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QGeoCoordinate;
class QGeoCirclePrivate;

class Q_LOCATION_EXPORT QGeoCircle : public QGeoShape
{
public:
    QGeoCircle();
    QGeoCircle(const QGeoCoordinate &center, qreal radius = -1.0);
    QGeoCircle(const QGeoCircle &other);
    QGeoCircle(const QGeoShape &other);

    ~QGeoCircle();

    QGeoCircle &operator=(const QGeoCircle &other);

#ifdef Q_NO_USING_KEYWORD
    bool operator==(const QGeoShape &other) const
    {
        return QGeoShape::operator==(other);
    }
#else
    using QGeoShape::operator==;
#endif
    bool operator==(const QGeoCircle &other) const;

#ifdef Q_NO_USING_KEYWORD
    bool operator!=(const QGeoShape &other) const
    {
        return QGeoShape::operator!=(other);
    }
#else
    using QGeoShape::operator!=;
#endif
    bool operator!=(const QGeoCircle &other) const;

    void setCenter(const QGeoCoordinate &center);
    QGeoCoordinate center() const;

    void setRadius(qreal radius);
    qreal radius() const;

    void translate(double degreesLatitude, double degreesLongitude);
    QGeoCircle translated(double degreesLatitude, double degreesLongitude) const;

private:
    inline QGeoCirclePrivate *d_func();
    inline const QGeoCirclePrivate *d_func() const;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QGeoCircle)

QT_END_HEADER

#endif

