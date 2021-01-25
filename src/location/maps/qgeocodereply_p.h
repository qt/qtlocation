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

#ifndef QGEOCODEREPLY_P_H
#define QGEOCODEREPLY_P_H

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
#include "qgeocodereply.h"

#include "qgeoshape.h"

#include <QList>
#include <QVariantMap>

QT_BEGIN_NAMESPACE

class QGeoLocation;

class Q_LOCATION_PRIVATE_EXPORT QGeoCodeReplyPrivate
{
public:
    QGeoCodeReplyPrivate();
    QGeoCodeReplyPrivate(QGeoCodeReply::Error error, const QString &errorString);
    virtual ~QGeoCodeReplyPrivate();

    virtual QVariantMap extraData() const;
    static const QGeoCodeReplyPrivate *get(const QGeoCodeReply &reply);
    static QGeoCodeReplyPrivate *get(QGeoCodeReply &reply);

    QGeoCodeReply::Error error;
    QString errorString;
    bool isFinished;

    QGeoShape viewport;
    QList<QGeoLocation> locations;

    int limit;
    int offset;
private:
    Q_DISABLE_COPY(QGeoCodeReplyPrivate)
};

QT_END_NAMESPACE

#endif
