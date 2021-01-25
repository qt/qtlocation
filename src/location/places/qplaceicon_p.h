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

#ifndef QPLACEICON_P_H
#define QPLACEICON_P_H

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

#include <QMap>
#include <QUrl>
#include <QSharedData>

QT_BEGIN_NAMESPACE

class QPlaceManager;
class QPlaceIconPrivate: public QSharedData
{
public:
    QPlaceIconPrivate();
    QPlaceIconPrivate(const QPlaceIconPrivate &other);
    ~QPlaceIconPrivate();

    QPlaceIconPrivate &operator=(const QPlaceIconPrivate &other);
    bool operator == (const QPlaceIconPrivate &other) const;

    QPlaceManager *manager;
    QVariantMap parameters;
};

QT_END_NAMESPACE

#endif
