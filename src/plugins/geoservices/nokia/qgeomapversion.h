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

#ifndef QGEOMAPVERSION_H
#define QGEOMAPVERSION_H

#include <QByteArray>
#include <QJsonObject>

QT_BEGIN_NAMESPACE

class QGeoMapVersion
{

public:
    QGeoMapVersion();
    bool isNewVersion(const QJsonObject &newVersionData);
    int version() const;
    void setVersion(const int);
    void setVersionData(const QJsonObject &versionData);
    QByteArray toJson() const;

private:
    int m_version;
    QJsonObject m_versionData;
};

QT_END_NAMESPACE

#endif // QGEOMAPVERSION_H
