// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QPLACESEARCHSUGGESTIONREPLYOHOSMAPKIT_H
#define QPLACESEARCHSUGGESTIONREPLYOHOSMAPKIT_H

#include <QtLocation/qplacemanagerengine.h>
#include <QtLocation/qplacesearchsuggestionreply.h>
#include <QtNetwork/qnetworkreply.h>

QT_BEGIN_NAMESPACE

QPlaceSearchSuggestionReply *makeQPlaceSearchSuggestionReplyOhosMapKit(
    QNetworkReply *reply, QPlaceManagerEngine *parent);

QT_END_NAMESPACE

#endif // QPLACESEARCHSUGGESTIONREPLYOHOSMAPKIT_H
