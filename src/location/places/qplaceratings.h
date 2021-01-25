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

#ifndef QPLACERATINGS_H
#define QPLACERATINGS_H

#include <QtCore/QMetaType>
#include <QtCore/QSharedDataPointer>
#include <QtLocation/qlocationglobal.h>

QT_BEGIN_NAMESPACE

class QPlaceRatingsPrivate;

class Q_LOCATION_EXPORT QPlaceRatings
{
public:
    QPlaceRatings();
    QPlaceRatings(const QPlaceRatings &other);

    ~QPlaceRatings();

    QPlaceRatings &operator=(const QPlaceRatings &other);

    bool operator==(const QPlaceRatings &other) const;
    bool operator!=(const QPlaceRatings &other) const {
        return !(other == *this);
    }

    qreal average() const;
    void setAverage(qreal average);

    int count() const;
    void setCount(int count);

    qreal maximum() const;
    void setMaximum(qreal max);

    bool isEmpty() const;

private:
    QSharedDataPointer<QPlaceRatingsPrivate> d;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QPlaceRatings)

#endif
