// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QGEOROUTEREPLYOHOSMAPKIT_H
#define QGEOROUTEREPLYOHOSMAPKIT_H

#include <QtNetwork/qnetworkreply.h>
#include <QtLocation/qgeoroutereply.h>

QT_BEGIN_NAMESPACE

QGeoRouteReply *makeQGeoRouteReplyOhosMapKit(
    QNetworkReply *reply, const QGeoRouteRequest &request, QObject *parent = nullptr);

QT_END_NAMESPACE

#endif // QGEOROUTEREPLYOHOSMAPKIT_H
