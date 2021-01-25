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

#ifndef QGEOCODEJSONPARSER_H
#define QGEOCODEJSONPARSER_H

#include <QtPositioning/QGeoShape>
#include <QtPositioning/QGeoLocation>

#include <QtCore/QObject>
#include <QtCore/QRunnable>
#include <QtCore/QJsonDocument>
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QList>

QT_BEGIN_NAMESPACE

class QGeoCodeJsonParser : public QObject, public QRunnable
{
    Q_OBJECT

public:
    void setBounds(const QGeoShape &bounds);
    void parse(const QByteArray &data);
    void run();

signals:
    void results(const QList<QGeoLocation> &locations);
    void error(const QString &errorString);

private:
    QJsonDocument m_document;
    QByteArray m_data;
    QGeoShape m_bounds;
    QList<QGeoLocation> m_results;
    QString m_errorString;
};

QT_END_NAMESPACE

#endif
