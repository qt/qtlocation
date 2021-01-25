/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtPositioning module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#ifndef QGEOPATH_H
#define QGEOPATH_H

#include <QtPositioning/QGeoShape>
#include <QtCore/QVariantList>

QT_BEGIN_NAMESPACE

class QGeoCoordinate;
class QGeoPathPrivate;

class Q_POSITIONING_EXPORT QGeoPath : public QGeoShape
{
    Q_GADGET
    Q_PROPERTY(QVariantList path READ variantPath WRITE setVariantPath)
    Q_PROPERTY(qreal width READ width WRITE setWidth)

public:
    QGeoPath();
    QGeoPath(const QList<QGeoCoordinate> &path, const qreal &width = 0.0);
    QGeoPath(const QGeoPath &other);
    QGeoPath(const QGeoShape &other);

    ~QGeoPath();

    QGeoPath &operator=(const QGeoPath &other);

    using QGeoShape::operator==;
    bool operator==(const QGeoPath &other) const;

    using QGeoShape::operator!=;
    bool operator!=(const QGeoPath &other) const;

    void setPath(const QList<QGeoCoordinate> &path);
    const QList<QGeoCoordinate> &path() const;
    void clearPath();
    void setVariantPath(const QVariantList &path);
    QVariantList variantPath() const;

    void setWidth(const qreal &width);
    qreal width() const;

    Q_INVOKABLE void translate(double degreesLatitude, double degreesLongitude);
    Q_INVOKABLE QGeoPath translated(double degreesLatitude, double degreesLongitude) const;
    Q_INVOKABLE double length(int indexFrom = 0, int indexTo = -1) const;
    Q_INVOKABLE int size() const;
    Q_INVOKABLE void addCoordinate(const QGeoCoordinate &coordinate);
    Q_INVOKABLE void insertCoordinate(int index, const QGeoCoordinate &coordinate);
    Q_INVOKABLE void replaceCoordinate(int index, const QGeoCoordinate &coordinate);
    Q_INVOKABLE QGeoCoordinate coordinateAt(int index) const;
    Q_INVOKABLE bool containsCoordinate(const QGeoCoordinate &coordinate) const;
    Q_INVOKABLE void removeCoordinate(const QGeoCoordinate &coordinate);
    Q_INVOKABLE void removeCoordinate(int index);

    Q_INVOKABLE QString toString() const;

private:
    inline QGeoPathPrivate *d_func();
    inline const QGeoPathPrivate *d_func() const;
};

Q_DECLARE_TYPEINFO(QGeoPath, Q_MOVABLE_TYPE);

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QGeoPath)

#endif // QGEOPATH_H
