/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qgeouriprovider.h"

#include <QMap>
#include <QVariant>
#include <QRandomGenerator>
#include <QSet>
#include <QString>

QT_BEGIN_NAMESPACE

QGeoUriProvider::QGeoUriProvider(
                QObject *parent,
                const QVariantMap &parameters,
                const QString &hostParameterName,
                const QString &internationalHost,
                const QString &localizedHost)
    : QObject(parent)
    , m_internationalHost(parameters.value(hostParameterName, internationalHost).toString())
    , m_localizedHost(localizedHost)
    , m_firstSubdomain(QChar::Null)
    , m_maxSubdomains(0)
{
    setCurrentHost(isInternationalNetwork() || m_localizedHost.isEmpty() ? m_internationalHost : m_localizedHost);
}

QString QGeoUriProvider::getCurrentHost() const
{
    if (m_maxSubdomains) {
        auto result = QString("%1%2").arg(m_firstSubdomain.toLatin1()).arg(QRandomGenerator::global()->bounded(m_maxSubdomains));
        result += '.' + m_currentHost;
        return result;
    }
    return m_currentHost;
}

void QGeoUriProvider::setCurrentHost(const QString &host)
{
    if (host.length() > 4 && host.at(1) == QChar('-') && host.at(3) == QChar('.')) {
        QString realHost = host.right(host.length() - 4);
        m_firstSubdomain = host.at(0);
        m_maxSubdomains = host.at(2).toLatin1() - host.at(0).toLatin1() + 1;
        m_currentHost = realHost;
    } else {
        m_currentHost = host;
        m_firstSubdomain = QChar::Null;
        m_maxSubdomains = 0;
    }
}

void QGeoUriProvider::mobileCountryCodeChanged(int interfaceId, const QString& mcc)
{
    Q_UNUSED(interfaceId);
    Q_UNUSED(mcc);

    setCurrentHost(isInternationalNetwork() || m_localizedHost.isEmpty() ? m_internationalHost : m_localizedHost);
}

bool QGeoUriProvider::isInternationalNetwork() const
{
    return true;
}

QT_END_NAMESPACE
