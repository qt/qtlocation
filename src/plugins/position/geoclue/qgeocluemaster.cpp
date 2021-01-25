/****************************************************************************
**
** Copyright (C) 2016 Jolla Ltd, author: Aaron McCarthy <aaron.mccarthy@jollamobile.com>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtPositioning module of the Qt Toolkit.
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

#include "qgeocluemaster.h"

#include <master_interface.h>
#include <geoclue_interface.h>
#include <masterclient_interface.h>

#include <QtCore/QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(lcPositioningGeoclue)

QT_BEGIN_NAMESPACE

QGeoclueMaster::QGeoclueMaster(QObject *parent)
:   QObject(parent), m_master(0), m_provider(0), m_client(0)
{
}

QGeoclueMaster::~QGeoclueMaster()
{
    releaseMasterClient();

    delete m_master;
}

bool QGeoclueMaster::hasMasterClient() const
{
    return m_client;
}

bool QGeoclueMaster::createMasterClient(Accuracy::Level accuracyLevel, ResourceFlags resourceFlags)
{
    Q_ASSERT(!m_provider || !m_client);

    if (!m_master) {
        qCDebug(lcPositioningGeoclue) << "creating master interface";
        m_master = new OrgFreedesktopGeoclueMasterInterface(QStringLiteral("org.freedesktop.Geoclue.Master"),
                                                            QStringLiteral("/org/freedesktop/Geoclue/Master"),
                                                            QDBusConnection::sessionBus());
    }

    qCDebug(lcPositioningGeoclue) << "creating client";
    QDBusPendingReply<QDBusObjectPath> client = m_master->Create();
    if (client.isError()) {
        QDBusError e = client.error();
        qCritical("Failed to create Geoclue client interface. Geoclue error: %s",
                  qPrintable(e.errorString(e.type())));
        return false;
    }

    qCDebug(lcPositioningGeoclue) << "Geoclue client path:" << client.value().path();

    m_provider = new OrgFreedesktopGeoclueInterface(QStringLiteral("org.freedesktop.Geoclue.Master"),
                                                    client.value().path(), QDBusConnection::sessionBus());
    m_provider->AddReference();

    m_client = new OrgFreedesktopGeoclueMasterClientInterface(QStringLiteral("org.freedesktop.Geoclue.Master"),
                                                              client.value().path(),
                                                              QDBusConnection::sessionBus());

    connect(m_client, SIGNAL(PositionProviderChanged(QString,QString,QString,QString)),
            this, SIGNAL(positionProviderChanged(QString,QString,QString,QString)));

    QDBusPendingReply<> reply = m_client->SetRequirements(accuracyLevel, 0, true, resourceFlags);
    if (reply.isError()) {
        QDBusError e = reply.error();
        qCritical("Failed to set Geoclue positioning requirements. Geoclue error: %s",
                  qPrintable(e.errorString(e.type())));

        releaseMasterClient();
        return false;
    }

    // Need to create the master position interface even though it will not be used, otherwise
    // GetPositionProvider always returns empty strings.
    reply = m_client->PositionStart();
    if (reply.isError()) {
        QDBusError e = reply.error();
        qCritical("Failed to start positioning. Geoclue error: %s",
                  qPrintable(e.errorString(e.type())));

        releaseMasterClient();
        return false;
    }

    return true;
}

void QGeoclueMaster::releaseMasterClient()
{
    if (m_provider)
        m_provider->RemoveReference();
    delete m_provider;
    m_provider = 0;
    delete m_client;
    m_client = 0;
}

QT_END_NAMESPACE
