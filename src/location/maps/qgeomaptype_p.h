/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
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
****************************************************************************/

#ifndef QGEOMAPTYPE_H
#define QGEOMAPTYPE_H

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

#include <QtCore/QString>
#include <QtCore/QSharedDataPointer>
#include <QtLocation/private/qlocationglobal_p.h>
#include <QtLocation/private/qgeocameracapabilities_p.h>
#include <QVariantMap>

QT_BEGIN_NAMESPACE

class QGeoMapTypePrivate;

class Q_LOCATION_PRIVATE_EXPORT QGeoMapType
{
public:
    enum MapStyle { // ### Qt6: change this to be a QFlags instead, or remove.
        NoMap = 0,
        StreetMap,
        SatelliteMapDay,
        SatelliteMapNight,
        TerrainMap,
        HybridMap,
        TransitMap,
        GrayStreetMap,
        PedestrianMap,
        CarNavigationMap,
        CycleMap,
        CustomMap = 100
    };

    QGeoMapType();
    QGeoMapType(const QGeoMapType &other);
    QGeoMapType(MapStyle style, const QString &name, const QString &description, bool mobile,
                bool night, int mapId, const QByteArray &pluginName,
                const QGeoCameraCapabilities &cameraCapabilities,
                const QVariantMap &metadata = QVariantMap());
    ~QGeoMapType();

    QGeoMapType &operator = (const QGeoMapType &other);

    bool operator == (const QGeoMapType &other) const;
    bool operator != (const QGeoMapType &other) const;

    MapStyle style() const;
    QString name() const;
    QString description() const;
    bool mobile() const;
    bool night() const;
    int mapId() const;
    QByteArray pluginName() const;
    QGeoCameraCapabilities cameraCapabilities() const;
    QVariantMap metadata() const;

private:
    QSharedDataPointer<QGeoMapTypePrivate> d_ptr;
};

QT_END_NAMESPACE

#endif // QGEOMAPTYPE_H
