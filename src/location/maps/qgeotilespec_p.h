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

#ifndef QGEOTILESPEC_H
#define QGEOTILESPEC_H

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

#include <QtLocation/private/qlocationglobal_p.h>
#include <QtCore/QMetaType>
#include <QString>

#include <QSharedDataPointer>

QT_BEGIN_NAMESPACE

class QGeoTileSpecPrivate;

class Q_LOCATION_PRIVATE_EXPORT QGeoTileSpec
{
public:
    QGeoTileSpec();
    QGeoTileSpec(const QGeoTileSpec &other);
    QGeoTileSpec(const QString &plugin, int mapId, int zoom, int x, int y, int version = -1);
    ~QGeoTileSpec();

    QGeoTileSpec &operator = (const QGeoTileSpec &other);

    QString plugin() const;

    void setZoom(int zoom);
    int zoom() const;

    void setX(int x);
    int x() const;

    void setY(int y);
    int y() const;

    void setMapId(int mapId);
    int mapId() const;

    void setVersion(int version);
    int version() const;

    bool operator == (const QGeoTileSpec &rhs) const;
    bool operator < (const QGeoTileSpec &rhs) const;

private:
    QSharedDataPointer<QGeoTileSpecPrivate> d;
};

Q_LOCATION_PRIVATE_EXPORT unsigned int qHash(const QGeoTileSpec &spec);

Q_LOCATION_PRIVATE_EXPORT QDebug operator<<(QDebug, const QGeoTileSpec &);

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QGeoTileSpec)

#endif // QGEOTILESPEC_H
