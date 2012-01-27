/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtLocation module of the Qt Toolkit.
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef INIT_REPLY_H
#define INIT_REPLY_H

#include "qplacemanagerengine_jsondb.h"
#include "macro.h"

#include <QtCore/QMap>
#include <QtLocation/QPlaceReply>
#include <QtLocation/QPlaceCategory>

class CategoryInitReply : public QPlaceReply
{
    Q_OBJECT
public:
    CategoryInitReply(QPlaceManagerEngineJsonDb *engine);
    virtual ~CategoryInitReply();

    DECLARE_TRIGGER_DONE_FN

    void start();
    void processNodeQueue();

protected:
    JsonDbClient *db();

private slots:
    void processResponse(int id, const QVariant &data);
    void processError(int id, int code, const QString &);

private:
    QPlaceManagerEngineJsonDb *m_engine;
    int m_reqId;
    CategoryTree m_tree;
    QList<CategoryNode> m_queue;
};

#endif
