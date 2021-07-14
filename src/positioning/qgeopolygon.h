/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtPositioning module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGEOPOLYGON_H
#define QGEOPOLYGON_H

#include <QtPositioning/QGeoShape>
#include <QtCore/QVariantList>

QT_BEGIN_NAMESPACE

class QGeoCoordinate;
class QGeoPolygonPrivate;

class Q_POSITIONING_EXPORT QGeoPolygon : public QGeoShape
{
    Q_GADGET
    Q_PROPERTY(QList<QGeoCoordinate> perimeter READ perimeter WRITE setPerimeter REVISION(5, 12))

public:
    QGeoPolygon();
    QGeoPolygon(const QList<QGeoCoordinate> &path);
    QGeoPolygon(const QGeoPolygon &other);
    QGeoPolygon(const QGeoShape &other);

    ~QGeoPolygon();

    QGeoPolygon &operator=(const QGeoPolygon &other);

    void setPerimeter(const QList<QGeoCoordinate> &path);
    const QList<QGeoCoordinate> &perimeter() const;

    Q_INVOKABLE void addHole(const QVariant &holePath);
                void addHole(const QList<QGeoCoordinate> &holePath);
    Q_INVOKABLE const QVariantList hole(qsizetype index) const;
                const QList<QGeoCoordinate> holePath(qsizetype index) const;
    Q_INVOKABLE void removeHole(qsizetype index);
    Q_INVOKABLE qsizetype holesCount() const;
    Q_INVOKABLE void translate(double degreesLatitude, double degreesLongitude);
    Q_INVOKABLE QGeoPolygon translated(double degreesLatitude, double degreesLongitude) const;
    Q_INVOKABLE double length(qsizetype indexFrom = 0, qsizetype indexTo = -1) const;
    Q_INVOKABLE qsizetype size() const;
    Q_INVOKABLE void addCoordinate(const QGeoCoordinate &coordinate);
    Q_INVOKABLE void insertCoordinate(qsizetype index, const QGeoCoordinate &coordinate);
    Q_INVOKABLE void replaceCoordinate(qsizetype index, const QGeoCoordinate &coordinate);
    Q_INVOKABLE QGeoCoordinate coordinateAt(qsizetype index) const;
    Q_INVOKABLE bool containsCoordinate(const QGeoCoordinate &coordinate) const;
    Q_INVOKABLE void removeCoordinate(const QGeoCoordinate &coordinate);
    Q_INVOKABLE void removeCoordinate(qsizetype index);

    Q_INVOKABLE QString toString() const;

private:
    inline QGeoPolygonPrivate *d_func();
    inline const QGeoPolygonPrivate *d_func() const;
};

Q_DECLARE_TYPEINFO(QGeoPolygon, Q_RELOCATABLE_TYPE);

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QGeoPolygon)

#endif // QGEOPOLYGON_H
