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

#include "initreply.h"
#include "jsonconverter.h"

#include <QtCore/QStringList>

CategoryInitReply::CategoryInitReply(QPlaceManagerEngineJsonDb *engine)
    : QPlaceReply(engine), m_engine(engine), m_reqId(-1)
{
}

CategoryInitReply::~CategoryInitReply()
{
}

void CategoryInitReply::start()
{
    connect(db(), SIGNAL(response(int,QVariant)), this, SLOT(processResponse(int,QVariant)));
    connect(db(), SIGNAL(error(int,int,QString)), this, SLOT(processError(int,int,QString)));

    CategoryNode rootNode;
    m_queue.append(rootNode);
    processNodeQueue();
}

void CategoryInitReply::processNodeQueue()
{
    if (!m_queue.isEmpty()) {
        QString categoryId = m_queue.first().category.categoryId();
        //find children
        m_reqId = db()->query(QString("[?%1=\"%2\"][?%3 = \"%4\"]").arg(JsonConverter::Type).arg(JsonConverter::CategoryType)
                    .arg(JsonConverter::CategoryParentId).arg(categoryId));
    } else {
        m_engine->setCategoryTree(m_tree);
        triggerDone(QPlaceReply::NoError);
        return;
    }
}

JsonDbClient *CategoryInitReply::db()
{
    return m_engine->db();
}

void CategoryInitReply::processResponse(int id, const QVariant &data)
{
    if (id != m_reqId)
        return;

    Q_ASSERT(!m_queue.isEmpty());

    QList<QPlaceCategory> categories = JsonConverter::convertJsonResponseToCategories(data.toMap(),
                                                                                      m_engine);

    foreach (const QPlaceCategory &category, categories) {
        CategoryNode node;
        node.category = category;
        node.parentId = m_queue.first().category.categoryId();
        m_queue.append(node);
    }

    CategoryNode node = m_queue.takeFirst();
    if (m_tree.contains(node.parentId)) {
        QStringList parentChildIds = m_tree.value(node.parentId).childIds;
        parentChildIds.append(node.category.categoryId());
        m_tree[node.parentId].childIds = parentChildIds;
    }

    m_tree.insert(node.category.categoryId(), node);
    processNodeQueue();
}

void CategoryInitReply::processError(int id, int code, const QString &jsonDbErrorString)
{
    Q_UNUSED(id)

    QPlaceReply::Error error = QPlaceReply::UnknownError;
    QString errorString = QString::fromLatin1("Unknown error occurred operation: jsondb error code =%1, erroString=%2").
                  arg(code).arg(jsonDbErrorString);
    triggerDone(error, errorString);
}
