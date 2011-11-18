/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "jsondbcleaner.h"

#include <jsondb-client.h>

#include <QDebug>
#include <QMap>
#include <QString>
#include <QStringList>

#define UUID "_uuid"
#define TYPE "_type"

#define PLACE_TYPE "place"
#define PLACE_CATEGORY_TYPE "com.nokia.mp.location.PlaceCategory"

JsonDbCleaner::JsonDbCleaner(QObject *parent)
    : QObject(parent)
{
    mDb = new JsonDbClient(this);
    if (!mDb->isConnected()) {
        qWarning() << "JsonDb not connected";
    } else  {
        connect(mDb, SIGNAL(response(int, const QVariant&)),
                this, SLOT(jsonDbResponse(int, const QVariant&)), Qt::QueuedConnection);
        connect(mDb, SIGNAL(error(int, int, const QString&)),
                this, SLOT(jsonDbError(int, int, const QString&)), Qt::QueuedConnection);
    }
}

void JsonDbCleaner::jsonDbResponse(int id, const QVariant& data)
{
    Q_UNUSED(id)
    Q_UNUSED(data)

    m_eventLoop.exit();
}

void JsonDbCleaner::jsonDbError(int id, int code, const QString& data)
{
    m_eventLoop.exit();
    qDebug() << Q_FUNC_INFO << " id: " << id << " code: " << code << " data: " << data;
}

void JsonDbCleaner::cleanDb()
{
    mDb->remove(QString::fromLatin1("[?%1=\"%2\"]").arg(TYPE).arg(PLACE_TYPE));
    m_eventLoop.exec();
    mDb->remove(QString::fromLatin1("[?%1=\"%2\"]").arg(TYPE).arg(PLACE_CATEGORY_TYPE));
    m_eventLoop.exec();
    emit dbCleaned();
}
