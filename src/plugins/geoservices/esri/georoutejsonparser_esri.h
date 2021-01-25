/****************************************************************************
**
** Copyright (C) 2013-2018 Esri <contracts@esri.com>
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
**
**
**
****************************************************************************/

#ifndef GEOROUTEJSONPARSERESRI_H
#define GEOROUTEJSONPARSERESRI_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QGeoRoute>
#include <QMap>

QT_BEGIN_NAMESPACE

class GeoRouteJsonParserEsri
{
public:
    GeoRouteJsonParserEsri(const QJsonDocument &document);

    QList<QGeoRoute> routes() const;
    bool isValid() const;
    QString errorString() const;

private:
    void parseDirections();
    void parseDirection(const QJsonObject &direction);
    void parseRoutes();
    void parseRoute(const QJsonObject &route);

    QString m_error;
    QMap<int, QGeoRoute> m_routes;
    QJsonObject m_json;
};

QT_END_NAMESPACE

#endif // GEOROUTEJSONPARSERESRI_H
