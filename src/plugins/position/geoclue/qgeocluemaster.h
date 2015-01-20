/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd, author: Aaron McCarthy <aaron.mccarthy@jollamobile.com>
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtPositioning module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGEOCLUEMASTER_H
#define QGEOCLUEMASTER_H

#include "geocluetypes.h"

#include <QtCore/QObject>

class OrgFreedesktopGeoclueMasterInterface;
class OrgFreedesktopGeoclueInterface;
class OrgFreedesktopGeoclueMasterClientInterface;

QT_BEGIN_NAMESPACE

class QGeoclueMaster : public QObject
{
    Q_OBJECT

public:
    QGeoclueMaster(QObject *parent = 0);
    ~QGeoclueMaster();

    enum ResourceFlag
    {
        ResourceNone = 0,
        ResourceNetwork = 1 << 0,
        ResourceCell = 1 << 1,
        ResourceGps = 1 << 2,
        ResourceAll = (1 << 10) - 1
    };

    Q_DECLARE_FLAGS(ResourceFlags, ResourceFlag)

    bool hasMasterClient() const;
    bool createMasterClient(Accuracy::Level accuracyLevel, ResourceFlags resourceFlags);
    void releaseMasterClient();

signals:
    void positionProviderChanged(const QString &name, const QString &description,
                                 const QString &service, const QString &path);

private:
    OrgFreedesktopGeoclueMasterInterface *m_master;
    OrgFreedesktopGeoclueInterface *m_provider;
    OrgFreedesktopGeoclueMasterClientInterface *m_client;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QGeoclueMaster::ResourceFlags)

QT_END_NAMESPACE

#endif // QGEOCLUEMASTER_H
