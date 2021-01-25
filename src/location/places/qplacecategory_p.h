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

#ifndef QPLACECATEGORY_P_H
#define QPLACECATEGORY_P_H

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

#include <QtCore/QSharedData>
#include <QtCore/QString>
#include <QtLocation/QLocation>

#include "qplaceicon.h"

QT_BEGIN_NAMESPACE

class QPlaceCategoryPrivate : public QSharedData
{
public:
    QPlaceCategoryPrivate();
    QPlaceCategoryPrivate(const QPlaceCategoryPrivate &other);

    ~QPlaceCategoryPrivate();
    QPlaceCategoryPrivate &operator= (const QPlaceCategoryPrivate &other);
    bool operator==(const QPlaceCategoryPrivate &other) const;
    bool isEmpty() const;

    QString categoryId;
    QString name;
    QLocation::Visibility visibility;
    QPlaceIcon icon;
};

QT_END_NAMESPACE

#endif // QPLACECATEGORY_P_H
