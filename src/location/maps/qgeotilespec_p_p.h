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
#ifndef QGEOTILESPEC_P_H
#define QGEOTILESPEC_P_H

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

#include <QString>
#include <QSharedData>

QT_BEGIN_NAMESPACE

class QGeoTileSpecPrivate : public QSharedData
{
public:
    QGeoTileSpecPrivate();
    QGeoTileSpecPrivate(const QGeoTileSpecPrivate &other);
    QGeoTileSpecPrivate(const QString &plugin, int mapId, int zoom, int x, int y, int version);
    ~QGeoTileSpecPrivate();

    QGeoTileSpecPrivate &operator = (const QGeoTileSpecPrivate &other);

    bool operator == (const QGeoTileSpecPrivate &rhs) const;
    bool operator < (const QGeoTileSpecPrivate &rhs) const;

    QString plugin_;
    int mapId_;
    int zoom_;
    int x_;
    int y_;
    int version_;
};

QT_END_NAMESPACE

#endif // QGEOTILESPEC_P_H
