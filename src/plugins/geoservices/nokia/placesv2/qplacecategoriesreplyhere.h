// Copyright (C) 2015 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#ifndef QPLACECATEGORIESREPLYHERE_H
#define QPLACECATEGORIESREPLYHERE_H

#include <QtLocation/QPlaceReply>

QT_BEGIN_NAMESPACE

class QPlaceCategoriesReplyHere : public QPlaceReply
{
    Q_OBJECT

public:
    explicit QPlaceCategoriesReplyHere(QObject *parent = nullptr);
    ~QPlaceCategoriesReplyHere();

    void emitFinished();

private slots:
    void setError(QPlaceReply::Error error_, const QString &errorString);
};

QT_END_NAMESPACE

#endif // QPLACECATEGORIESREPLYHERE_H
