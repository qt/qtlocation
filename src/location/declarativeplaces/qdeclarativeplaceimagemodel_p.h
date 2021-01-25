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

#ifndef QDECLARATIVEPLACEIMAGEMODEL_P_H
#define QDECLARATIVEPLACEIMAGEMODEL_P_H

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

#include <QtLocation/private/qlocationglobal_p.h>
#include <QtLocation/private/qdeclarativeplacecontentmodel_p.h>

QT_BEGIN_NAMESPACE

class QDeclarativeSupplier;

class Q_LOCATION_PRIVATE_EXPORT QDeclarativePlaceImageModel : public QDeclarativePlaceContentModel
{
    Q_OBJECT

public:
    explicit QDeclarativePlaceImageModel(QObject *parent = 0);
    ~QDeclarativePlaceImageModel();

    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;

    enum Roles {
        UrlRole = UserRole,
        ImageIdRole,
        MimeTypeRole
    };
};

QT_END_NAMESPACE

#endif
