// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QGEOMAPREPLYOHOSMAPKIT_H
#define QGEOMAPREPLYOHOSMAPKIT_H

#include <QtCore/qobject.h>
#include <QtLocation/private/qgeotiledmapreply_p.h>
#include <QtLocation/private/qgeotilespec_p.h>
#include <QtNetwork/qnetworkreply.h>

QT_BEGIN_NAMESPACE

QGeoTiledMapReply *makeGeoTiledMapReplyOhosMapKit(
    QNetworkReply *reply, const QGeoTileSpec &spec, QObject *parent);

QT_END_NAMESPACE

#endif // QGEOMAPREPLYOHOSMAPKIT_H
