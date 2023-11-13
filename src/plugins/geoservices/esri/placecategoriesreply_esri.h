// Copyright (C) 2013-2018 Esri <contracts@esri.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#ifndef PLACECATEGORIESREPLYESRI_H
#define PLACECATEGORIESREPLYESRI_H

#include <QtLocation/QPlaceReply>

QT_BEGIN_NAMESPACE

class PlaceCategoriesReplyEsri : public QPlaceReply
{
    Q_OBJECT

public:
    explicit PlaceCategoriesReplyEsri(QObject *parent = nullptr);
    ~PlaceCategoriesReplyEsri();

    void emitFinished();
    void setError(QPlaceReply::Error errorCode, const QString &errorString);
};

QT_END_NAMESPACE

#endif // PLACECATEGORIESREPLYESRI_H
