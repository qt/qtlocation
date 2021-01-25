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

#ifndef QPLACERESULT_H
#define QPLACERESULT_H

#include <QtLocation/QPlace>
#include <QtLocation/QPlaceSearchResult>

QT_BEGIN_NAMESPACE

class QPlaceResultPrivate;

class Q_LOCATION_EXPORT QPlaceResult : public QPlaceSearchResult
{
public:
    QPlaceResult();

#ifdef Q_QDOC
    QPlaceResult::QPlaceResult(const QPlaceSearchResult &other);
#else
     Q_DECLARE_SEARCHRESULT_COPY_CTOR(QPlaceResult)
#endif

    virtual ~QPlaceResult();

    qreal distance() const;
    void setDistance(qreal distance);

    QPlace place() const;
    void setPlace(const QPlace &place);

    bool isSponsored() const;
    void setSponsored(bool sponsored);

private:
    Q_DECLARE_SEARCHRESULT_D_FUNC(QPlaceResult)
};

Q_DECLARE_TYPEINFO(QPlaceResult, Q_MOVABLE_TYPE);

QT_END_NAMESPACE

#endif
