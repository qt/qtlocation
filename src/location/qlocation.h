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

#ifndef QLOCATION_H
#define QLOCATION_H

#if 0
#pragma qt_class(QLocation)
#endif

#include <QtLocation/qlocationglobal.h>

QT_BEGIN_NAMESPACE

namespace QLocation {

enum Visibility {
    UnspecifiedVisibility = 0x00,
    DeviceVisibility = 0x01,
    PrivateVisibility = 0x02,
    PublicVisibility = 0x04
};

Q_DECLARE_FLAGS(VisibilityScope, Visibility)

}

Q_DECLARE_OPERATORS_FOR_FLAGS(QLocation::VisibilityScope)

QT_END_NAMESPACE

#endif // QLOCATION_H
