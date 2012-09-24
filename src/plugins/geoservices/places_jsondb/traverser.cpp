/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "jsondb.h"
#include "traverser.h"

CategoryTraverser::CategoryTraverser(JsonDb *db, QObject *parent)
    : QObject(parent), m_db(db)
{
    Q_ASSERT(m_db);
}

void CategoryTraverser::start(const QString &rootId)
{
    m_rootId = rootId;

    if (!rootId.isEmpty()) {
        m_db->getCategory(rootId, this, SLOT(rootCategoryFinished()));
    } else {
        m_catIdQueue.append(rootId);
        processQueue();
    }
}

CategoryTree CategoryTraverser::convertToTree(const QList<QJsonObject> &catObjects,
                                              const QPlaceManagerEngineJsonDb *engine)
{
    CategoryTree tree;
    tree.insert(QString(), CategoryNode());

    //first pass to populate the tree
    foreach (const QJsonObject &catObject, catObjects) {
        CategoryNode node;
        QPlaceCategory category = JsonDb::convertJsonObjectToCategory(catObject, engine);
        node.category = category;
        tree.insert(category.categoryId(), node);
    }

    //second pass to assign parent and children ids
    foreach (const QJsonObject &catObject, catObjects) {
        QString parentId = catObject.value(JsonDb::CategoryParentId).toString();
        QString childId = catObject.value(JsonDb::Uuid).toString();
        if (!tree.contains(parentId) || !tree.contains(childId))
            continue;

        QStringList childIds = tree.value(parentId).childIds;
        childIds += childId;
        tree[parentId].childIds = childIds;

        tree[childId].parentId = parentId;
    }

    return tree;
}

void CategoryTraverser::processQueue()
{
    if (!m_catIdQueue.isEmpty()) {
        QString categoryId = m_catIdQueue.takeFirst();
        m_db->getChildCategories(categoryId, this, SLOT(requestFinished()));
    } else {
        emit finished();
        return;
    }
}

void CategoryTraverser::rootCategoryFinished()
{
    QJsonDbRequest *request = qobject_cast<QJsonDbRequest *>(sender());
    Q_ASSERT(request);

    QList<QJsonObject> objects = request->takeResults();

    if (objects.isEmpty()) {
        m_errorString = QString::fromLatin1("Specified category not found: %1")
                .arg(m_rootId);
        emit finished();
        return;
    }

    QJsonObject object = objects.takeFirst();
    QString rootId = object.value(JsonDb::Uuid).toString();
    if (rootId != m_rootId) {
        m_errorString = QString::fromLatin1("Retrieved category does not match expected "
                                            "category, expectedId: %1, actualId %2")
                .arg(m_rootId)
                .arg(object.value(JsonDb::Uuid).toString());
        emit finished();
        return;
    }

    m_catObjs.append(object);
    m_catIdQueue.append(rootId);
    processQueue();
}

void CategoryTraverser::requestFinished()
{
    QJsonDbRequest *request = qobject_cast<QJsonDbRequest *>(sender());
    Q_ASSERT(request);

    QList<QJsonObject> objects = request->takeResults();
    foreach (const QJsonObject &object, objects) {
        QString categoryId = object.value(JsonDb::Uuid).toString();
        if (object.value(JsonDb::Type).toString() == JsonDb::CategoryType
                && !categoryId.isEmpty()) {
            m_catObjs.append(object);
            m_catIdQueue.append(categoryId);
        }
    }
    processQueue();
}

void CategoryTraverser::requestError(QJsonDbRequest::ErrorCode dbCode, const QString &dbErrorString)
{
    m_errorString = QString::fromLatin1("JsonDb error code: %1\n"
                                        "JsonDb error string: %2")
            .arg(dbCode).arg(dbErrorString);
    emit finished();
}

