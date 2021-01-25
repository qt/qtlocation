/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtPositioning module of the Qt Toolkit.
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
**
**
**
****************************************************************************/
#ifndef QGEOPOSITIONINFO_P_H
#define QGEOPOSITIONINFO_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtPositioning/private/qpositioningglobal_p.h>
#include "qgeopositioninfo.h"
#include <QHash>
#include <QDateTime>
#include <QtPositioning/qgeocoordinate.h>

QT_BEGIN_NAMESPACE

class Q_POSITIONING_PRIVATE_EXPORT QGeoPositionInfoPrivate
{
public:
    virtual ~QGeoPositionInfoPrivate();
    virtual QGeoPositionInfoPrivate *clone() const;

    virtual bool operator==(const QGeoPositionInfoPrivate &other) const;

    QDateTime timestamp;
    QGeoCoordinate coord;
    QHash<QGeoPositionInfo::Attribute, qreal> doubleAttribs;

    static QGeoPositionInfoPrivate *get(const QGeoPositionInfo &info);
};

QT_END_NAMESPACE

#endif // QGEOPOSITIONINFO_P_H
