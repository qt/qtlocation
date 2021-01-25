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

#ifndef QGEOROUTEPARSER_P_P_H
#define QGEOROUTEPARSER_P_P_H

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

#include <QtCore/private/qobject_p.h>
#include <QtCore/QUrl>
#include <QtLocation/qgeoroutereply.h>
#include <QtLocation/qgeorouterequest.h>

QT_BEGIN_NAMESPACE

class QGeoRouteParserPrivate :  public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QGeoRouteParser)
public:
    QGeoRouteParserPrivate();
    virtual ~QGeoRouteParserPrivate();

    virtual QGeoRouteReply::Error parseReply(QList<QGeoRoute> &routes, QString &errorString, const QByteArray &reply) const = 0;
    virtual QUrl requestUrl(const QGeoRouteRequest &request, const QString &prefix) const = 0;

    QGeoRouteParser::TrafficSide trafficSide;
};

QT_END_NAMESPACE

#endif // QGEOROUTEPARSER_P_P_H
