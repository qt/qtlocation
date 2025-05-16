// Copyright (C) 2014 Appello Systems AB.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#ifndef QGEOMAPVERSION_H
#define QGEOMAPVERSION_H

#include <QByteArray>
#include <QJsonObject>

QT_BEGIN_NAMESPACE

class QGeoMapVersion
{

public:
    QGeoMapVersion();
    bool isNewVersion(const QJsonObject &newVersionData) const;
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
