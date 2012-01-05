/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef DETAILSREPLY_H
#define DETAILSREPLY_H

#include <qplacedetailsreply.h>
#include <qplacemanagerengine_jsondb.h>

#include "macro.h"

QT_BEGIN_NAMESPACE

class DetailsReply : public QPlaceDetailsReply
{
    Q_OBJECT
    enum State {
        Initial,
        GetCategories,
        GetPlace
    };

public:
    DetailsReply(QPlaceManagerEngineJsonDb *engine);
    virtual ~DetailsReply();

    DECLARE_TRIGGER_DONE_FN

    void setPlaceId(const QString &placeId);

    void start();

protected:
    JsonDbClient *db() {return m_engine->db();}

private slots:
    void processResponse(int id, const QVariant &data);
    void processError(int id, int code, const QString &errorString);

private:
    QPlaceManagerEngineJsonDb *m_engine;
    QString m_placeId;
    int m_reqId;
    State m_state;
};

QT_END_NAMESPACE

#endif
