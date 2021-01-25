/****************************************************************************
**
** Copyright (C) 2014 Appello Systems AB.
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

#include "qgeomapversion.h"

#include <QJsonDocument>

QT_BEGIN_NAMESPACE

QGeoMapVersion::QGeoMapVersion()
    : m_version(-1) {}

bool QGeoMapVersion::isNewVersion(const QJsonObject &newVersionData)
{
    return m_versionData != newVersionData;
}

int QGeoMapVersion::version() const
{
    return m_version;
}

void QGeoMapVersion::setVersion(int version)
{
    m_version = version;
}

void QGeoMapVersion::setVersionData(const QJsonObject &versionData)
{
    m_versionData = versionData;
}


QByteArray QGeoMapVersion::toJson() const
{

    QJsonObject object;
    object[QLatin1String("version")] = m_version;
    object[QLatin1String("data")] = m_versionData;

    QJsonDocument document(object);

    return document.toJson();
}

QT_END_NAMESPACE
