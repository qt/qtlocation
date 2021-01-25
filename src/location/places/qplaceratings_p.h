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

#ifndef QPLACERATINGS_P_H
#define QPLACERATINGS_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QSharedData>

QT_BEGIN_NAMESPACE

class QPlaceRatingsPrivate : public QSharedData
{
public:
    QPlaceRatingsPrivate();
    QPlaceRatingsPrivate(const QPlaceRatingsPrivate &other);

    ~QPlaceRatingsPrivate();

    bool operator==(const QPlaceRatingsPrivate &other) const;

    bool isEmpty() const;

    qreal average;
    qreal maximum;
    int count;
};

QT_END_NAMESPACE

#endif // QPLACERATING_P_H
