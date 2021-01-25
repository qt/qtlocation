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

#ifndef QOSRMROUTEPARSER_P_H
#define QOSRMROUTEPARSER_P_H

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
#include <QtLocation/qgeoroutereply.h>
#include <QtLocation/qgeorouterequest.h>
#include <QtCore/QByteArray>
#include <QtCore/QUrl>

QT_BEGIN_NAMESPACE

class QGeoRouteParserPrivate;
class Q_LOCATION_PRIVATE_EXPORT QGeoRouteParser : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGeoRouteParser)
    Q_PROPERTY(TrafficSide trafficSide READ trafficSide WRITE setTrafficSide NOTIFY trafficSideChanged)
    Q_ENUMS(TrafficSide)
public:
    enum TrafficSide {
        RightHandTraffic,
        LeftHandTraffic
    };
    virtual ~QGeoRouteParser();
    QGeoRouteReply::Error parseReply(QList<QGeoRoute> &routes, QString &errorString, const QByteArray &reply) const;
    QUrl requestUrl(const QGeoRouteRequest &request, const QString &prefix) const;

    TrafficSide trafficSide() const;

public Q_SLOTS:
    void setTrafficSide(TrafficSide trafficSide);

Q_SIGNALS:
    void trafficSideChanged(TrafficSide trafficSide);

protected:
    QGeoRouteParser(QGeoRouteParserPrivate &dd, QObject *parent = nullptr);

private:
    Q_DISABLE_COPY(QGeoRouteParser)
};

QT_END_NAMESPACE

#endif // QOSRMROUTEPARSER_P_H
