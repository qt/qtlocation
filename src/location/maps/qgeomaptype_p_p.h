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

#ifndef QGEOMAPTYPE_P_H
#define QGEOMAPTYPE_P_H

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

#include <QMetaType>
#include <QString>
#include <QVariantMap>
#include <QByteArray>
#include <QSharedData>
#include "qgeocameracapabilities_p.h"
#include "qgeomaptype_p.h"

QT_BEGIN_NAMESPACE

class QGeoMapTypePrivate : public QSharedData
{
public:
    QGeoMapTypePrivate();
    QGeoMapTypePrivate(QGeoMapType::MapStyle style, const QString &name, const QString &description, bool mobile,
                       bool night, int mapId, const QByteArray &pluginName,
                       const QGeoCameraCapabilities &cameraCapabilities,
                       const QVariantMap &metadata);
    QGeoMapTypePrivate(const QGeoMapTypePrivate &other);
    ~QGeoMapTypePrivate();

    QGeoMapTypePrivate &operator = (const QGeoMapTypePrivate &other);

    bool operator == (const QGeoMapTypePrivate &other) const;

    QGeoMapType::MapStyle style_;
    QString name_;
    QString description_;
    bool mobile_;
    bool night_;
    int mapId_;
    QByteArray pluginName_;
    QGeoCameraCapabilities cameraCapabilities_;
    QVariantMap metadata_;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QGeoMapTypePrivate)

#endif // QGEOMAPTYPE_P_H
