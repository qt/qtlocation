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

#ifndef QGEOCOORDINATEOBJECT_P_H
#define QGEOCOORDINATEOBJECT_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtPositioning/private/qpositioningglobal_p.h>
#include <QObject>
#include <QGeoCoordinate>
#include <QVariantMap>

QT_BEGIN_NAMESPACE

class Q_POSITIONING_PRIVATE_EXPORT QGeoCoordinateObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QGeoCoordinate coordinate READ coordinate WRITE setCoordinate NOTIFY coordinateChanged)

public:
    QGeoCoordinateObject(QObject *parent = 0);
    QGeoCoordinateObject(const QGeoCoordinate &c, QObject *parent = 0);
    virtual ~QGeoCoordinateObject();

    bool operator==(const QGeoCoordinate &other) const;
    bool operator==(const QGeoCoordinateObject &other) const;
    inline bool operator!=(const QGeoCoordinate &other) const {
        return !operator==(other);
    }
    inline bool operator!=(const QGeoCoordinateObject &other) const {
        return !operator==(other);
    }

    QGeoCoordinate coordinate() const;
    void setCoordinate(const QGeoCoordinate &c);

Q_SIGNALS:
    void coordinateChanged();

protected:
    QGeoCoordinate m_coordinate;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QGeoCoordinateObject*)

#endif // QGEOCOORDINATEOBJECT_P_H
