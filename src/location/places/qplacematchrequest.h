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

#ifndef QPLACEMATCHREQUEST_H
#define QPLACEMATCHREQUEST_H

#include <QtCore/QSharedDataPointer>
#include <QtLocation/QPlaceSearchResult>

QT_BEGIN_NAMESPACE

class QPlaceMatchRequestPrivate;

class Q_LOCATION_EXPORT QPlaceMatchRequest
{
public:
    static const QString AlternativeId;

    QPlaceMatchRequest();
    QPlaceMatchRequest(const QPlaceMatchRequest &other);


    QPlaceMatchRequest &operator=(const QPlaceMatchRequest &other);

    bool operator==(const QPlaceMatchRequest &other) const;
    bool operator!=(const QPlaceMatchRequest &other) const;

    ~QPlaceMatchRequest();

    QList<QPlace> places() const;
    void setPlaces(const QList<QPlace> places);

    void setResults(const QList<QPlaceSearchResult> &results);

    QVariantMap parameters() const;
    void setParameters(const QVariantMap &parameters);

    void clear();

private:
    QSharedDataPointer<QPlaceMatchRequestPrivate> d_ptr;
    inline QPlaceMatchRequestPrivate *d_func();
    inline const QPlaceMatchRequestPrivate *d_func() const;
};

QT_END_NAMESPACE

#endif
