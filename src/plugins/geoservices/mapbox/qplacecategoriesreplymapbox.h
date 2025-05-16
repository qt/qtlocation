// Copyright (C) 2017 Mapbox, Inc.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#ifndef QPLACECATEGORIESREPLYMAPBOX_H
#define QPLACECATEGORIESREPLYMAPBOX_H

#include <QtLocation/QPlaceReply>

QT_BEGIN_NAMESPACE

class QPlaceCategoriesReplyMapbox : public QPlaceReply
{
    Q_OBJECT

public:
    explicit QPlaceCategoriesReplyMapbox(QObject *parent = nullptr);
    ~QPlaceCategoriesReplyMapbox();

public slots:
    void finish();
    void setError(QPlaceReply::Error errorCode, const QString &errorString);
};

QT_END_NAMESPACE

#endif // QPLACECATEGORIESREPLYMAPBOX_H
