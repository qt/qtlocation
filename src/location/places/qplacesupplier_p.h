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

#ifndef QPLACESUPPLIER_P_H
#define QPLACESUPPLIER_P_H

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

#include <QString>
#include <QSharedData>
#include <QUrl>

#include "qplaceicon.h"

QT_BEGIN_NAMESPACE

class QPlaceSupplierPrivate : public QSharedData
{
public:
    QPlaceSupplierPrivate();
    QPlaceSupplierPrivate(const QPlaceSupplierPrivate &other);

    ~QPlaceSupplierPrivate();

    bool operator==(const QPlaceSupplierPrivate &other) const;

    bool isEmpty() const;

    QString name;
    QString supplierId;
    QUrl url;
    QPlaceIcon icon;
};

QT_END_NAMESPACE

#endif // QPLACESUPPLIER_P_H
