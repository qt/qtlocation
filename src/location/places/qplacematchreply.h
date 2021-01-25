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

#ifndef QPLACEMATCHREPLY_H
#define QPLACEMATCHREPLY_H

#include <QtLocation/QPlaceReply>
#include <QtLocation/QPlaceMatchRequest>

QT_BEGIN_NAMESPACE

class QPlaceMatchReplyPrivate;
class Q_LOCATION_EXPORT QPlaceMatchReply : public QPlaceReply
{
    Q_OBJECT
public:
    explicit QPlaceMatchReply(QObject *parent = nullptr);
    ~QPlaceMatchReply();

    QPlaceReply::Type type() const;

    QList<QPlace> places() const;
    QPlaceMatchRequest request() const;

protected:
    void setPlaces(const QList<QPlace> &results);
    void setRequest(const QPlaceMatchRequest &request);
private:
    Q_DISABLE_COPY(QPlaceMatchReply)
    Q_DECLARE_PRIVATE(QPlaceMatchReply)
};

QT_END_NAMESPACE

#endif
