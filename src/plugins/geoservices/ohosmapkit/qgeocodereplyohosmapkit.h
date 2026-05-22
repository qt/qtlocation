// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QGEOCODEREPLYOHOSMAPKIT_H
#define QGEOCODEREPLYOHOSMAPKIT_H

#include <QtNetwork/qnetworkreply.h>
#include <QtLocation/qgeocodereply.h>

QT_BEGIN_NAMESPACE

QGeoCodeReply *makeQGeoCodeReplyOhosMapKit(QNetworkReply *reply, QObject *parent = nullptr);

QT_END_NAMESPACE

#endif // QGEOCODEREPLYOHOSMAPKIT_H
