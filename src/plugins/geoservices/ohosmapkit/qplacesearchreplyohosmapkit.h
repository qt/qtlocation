// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QPLACESEARCHREPLYOHOSMAPKIT_H
#define QPLACESEARCHREPLYOHOSMAPKIT_H

#include <QtLocation/qplacemanagerengine.h>
#include <QtLocation/qplacesearchreply.h>
#include <QtLocation/qplacesearchrequest.h>
#include <QtNetwork/qnetworkreply.h>

QT_BEGIN_NAMESPACE

QPlaceSearchReply *makeQPlaceSearchReplyOhosMapKit(
    const QPlaceSearchRequest &request, QNetworkReply *reply, QPlaceManagerEngine *parent);

QT_END_NAMESPACE

#endif // QPLACESEARCHREPLYOHOSMAPKIT_H
