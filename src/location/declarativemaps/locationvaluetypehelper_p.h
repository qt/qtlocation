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

#ifndef LOCATION_VALUE_TYPE_HELPER
#define LOCATION_VALUE_TYPE_HELPER

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
#include <QJSValue>
#include <QVariant>
#include <QGeoCoordinate>
#include <QGeoRectangle>
#include <QGeoCircle>

QGeoCoordinate Q_LOCATION_PRIVATE_EXPORT parseCoordinate(const QJSValue &value, bool *ok = nullptr);
QGeoCoordinate Q_LOCATION_PRIVATE_EXPORT parseCoordinate(const QVariant &value, bool *ok = nullptr);
QGeoRectangle Q_LOCATION_PRIVATE_EXPORT parseRectangle(const QJSValue &value, bool *ok);
QGeoCircle Q_LOCATION_PRIVATE_EXPORT parseCircle(const QJSValue &value, bool *ok);
QJSValue Q_LOCATION_PRIVATE_EXPORT fromList(const QObject *object, const QList<QGeoCoordinate> &list);
QList<QGeoCoordinate> Q_LOCATION_PRIVATE_EXPORT toList(const QObject *object, const QJSValue &value);
#endif
