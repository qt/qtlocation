/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef JSONDBCLEANER_H
#define JSONDBCLEANER_H

#include <QObject>
#include <QMap>
#include <QVariant>
#include <QEventLoop>

#include <jsondb-global.h>
Q_ADDON_JSONDB_BEGIN_NAMESPACE
class JsonDbClient;
Q_ADDON_JSONDB_END_NAMESPACE
Q_USE_JSONDB_NAMESPACE

class JsonDbCleaner : public QObject
{
    Q_OBJECT
public:
    JsonDbCleaner(QObject *parent = 0);
    void cleanDb();

public slots:
    void jsonDbResponse( int id, const QVariant& data);
    void jsonDbError( int id, int code, const QString& data);

signals:
    void dbCleaned();

private:
    JsonDbClient *mDb;
    int cleanReqId;
    QEventLoop m_eventLoop;
};

#endif
