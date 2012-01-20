/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativeplacecontentmodel.h"
#include "qdeclarativeplace_p.h"
#include "qdeclarativegeoserviceprovider_p.h"
#include "qdeclarativeplaceuser_p.h"

#include <QtDeclarative/QDeclarativeInfo>
#include <QtLocation/QGeoServiceProvider>
#include <QtLocation/QPlaceManager>
#include <QtLocation/QPlaceContentRequest>

QT_BEGIN_NAMESPACE

QDeclarativePlaceContentModel::QDeclarativePlaceContentModel(QPlaceContent::Type type,
                                                             QObject *parent)
:   QAbstractListModel(parent), m_place(0), m_type(type), m_batchSize(1), m_contentCount(-1),
    m_reply(0), m_complete(false)
{
    QHash<int, QByteArray> roles = roleNames();
    roles.insert(SupplierRole, "supplier");
    roles.insert(PlaceUserRole, "user");
    roles.insert(AttributionRole, "attribution");
    setRoleNames(roles);
}

QDeclarativePlaceContentModel::~QDeclarativePlaceContentModel()
{
}


QDeclarativePlace *QDeclarativePlaceContentModel::place() const
{
    return m_place;
}

void QDeclarativePlaceContentModel::setPlace(QDeclarativePlace *place)
{
    if (m_place != place) {
        beginResetModel();
        if (m_reply) {
            m_reply->abort();
            m_reply->deleteLater();
            m_reply = 0;
        }

        clearData();

        endResetModel();

        if (m_contentCount != -1) {
            m_contentCount = -1;
            emit totalCountChanged();
        }

        m_place = place;
        emit placeChanged();

        reset();

        fetchMore(QModelIndex());
    }
}

int QDeclarativePlaceContentModel::batchSize() const
{
    return m_batchSize;
}

void QDeclarativePlaceContentModel::setBatchSize(int batchSize)
{
    if (m_batchSize != batchSize) {
        m_batchSize = batchSize;
        emit batchSizeChanged();
    }
}

int QDeclarativePlaceContentModel::totalCount() const
{
    return m_contentCount;
}

static QPair<int, int> findMissingKey(const QMap<int, QPlaceContent> &map)
{
    int start = 0;
    while (map.contains(start))
        ++start;

    QMap<int, QPlaceContent>::const_iterator it = map.lowerBound(start);
    if (it == map.end())
        return qMakePair(start, -1);

    int end = start;
    while (!map.contains(end))
        ++end;

    return qMakePair(start, end - 1);
}

void QDeclarativePlaceContentModel::clear()
{
    beginResetModel();
    clearData();
    endResetModel();
}

void QDeclarativePlaceContentModel::clearData()
{
    qDeleteAll(m_users);
    m_users.clear();

    qDeleteAll(m_suppliers);
    m_suppliers.clear();

    m_contentCount = -1;
    m_content.clear();

    delete m_reply;
    m_reply = 0;
}

int QDeclarativePlaceContentModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_content.count();
}

QVariant QDeclarativePlaceContentModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= rowCount(index.parent()) || index.row() < 0)
        return QVariant();

    const QPlaceContent &content = m_content.value(index.row());

    switch (role) {
    case SupplierRole:
        return QVariant::fromValue(static_cast<QObject *>(m_suppliers.value(content.supplier().supplierId())));
    case PlaceUserRole:
        return QVariant::fromValue(static_cast<QObject *>(m_users.value(content.user().userId())));
    case AttributionRole:
        return content.attribution();
    default:
        return QVariant();
    }
}

bool QDeclarativePlaceContentModel::canFetchMore(const QModelIndex &parent) const
{
    if (parent.isValid())
        return false;

    if (!m_place)
        return false;

    if (m_contentCount == -1)
        return true;

    return m_content.count() != m_contentCount;
}

void QDeclarativePlaceContentModel::fetchMore(const QModelIndex &parent)
{
    if (parent.isValid())
        return;

    if (!m_place)
        return;

    if (m_reply)
        return;

    if (!m_place->plugin())
        return;

    QDeclarativeGeoServiceProvider *plugin = m_place->plugin();

    QGeoServiceProvider *serviceProvider = plugin->sharedGeoServiceProvider();
    if (!serviceProvider)
        return;

    QPlaceManager *placeManager = serviceProvider->placeManager();
    if (!placeManager) {
        qmlInfo(this) << tr("Places not supported by %1 Plugin.").arg(plugin->name());
        return;
    }

    QPlaceContentRequest request;
    request.setContentType(m_type);

    if (m_contentCount == -1) {
        request.setOffset(0);
        request.setLimit(m_batchSize);
    } else {
        QPair<int, int> missing = findMissingKey(m_content);
        request.setOffset(missing.first);
        if (missing.second == -1)
            request.setLimit(m_batchSize);
        else
            request.setLimit(qMin(m_batchSize, missing.second - missing.first + 1));
    }

    m_reply = placeManager->getPlaceContent(m_place->place().placeId(), request);
    connect(m_reply, SIGNAL(finished()), this, SLOT(fetchFinished()), Qt::QueuedConnection);
}

void QDeclarativePlaceContentModel::classBegin()
{
}

void QDeclarativePlaceContentModel::componentComplete()
{
    m_complete = true;
    fetchMore(QModelIndex());
}

void QDeclarativePlaceContentModel::fetchFinished()
{
    QPlaceContentReply *reply = m_reply;
    m_reply = 0;

    if (m_contentCount != reply->totalCount()) {
        m_contentCount = reply->totalCount();
        emit totalCountChanged();
    }

    if (!reply->content().isEmpty()) {
        QPlaceContent::Collection contents = reply->content();

        //find out which indexes are new and which ones have changed.
        QMapIterator<int, QPlaceContent> it(contents);
        QList<int> changedIndexes;
        QList<int> newIndexes;
        while (it.hasNext()) {
            it.next();
            if (!m_content.contains(it.key()))
                newIndexes.append(it.key());
            else if (it.value() != m_content.value(it.key()))
                changedIndexes.append(it.key());
        }

        //insert new indexes in blocks where within each
        //block, the indexes are consecutive.
        QListIterator<int> newIndexesIter(newIndexes);
        int startIndex = -1;
        while (newIndexesIter.hasNext()) {
            int currentIndex = newIndexesIter.next();
            if (startIndex == -1)
                startIndex = currentIndex;

            if (!newIndexesIter.hasNext() || (newIndexesIter.hasNext() && (newIndexesIter.peekNext() > (currentIndex + 1)))) {
                beginInsertRows(QModelIndex(),startIndex,currentIndex);
                for (int i = startIndex; i <= currentIndex; ++i) {
                    const QPlaceContent &content = contents.value(i);

                    m_content.insert(i, content);
                    if (!m_suppliers.contains(content.supplier().supplierId())) {
                        m_suppliers.insert(content.supplier().supplierId(),
                                           new QDeclarativeSupplier(content.supplier(), m_place->plugin(), this));
                    }
                    if (!m_users.contains(content.user().userId())) {
                        m_users.insert(content.user().userId(),
                                           new QDeclarativePlaceUser(content.user(), this));
                    }
                }
                endInsertRows();
                startIndex = -1;
            }
        }

        //modify changed indexes in blocks where within each
        //block, the indexes are consecutive.
        startIndex = -1;
        QListIterator<int> changedIndexesIter(changedIndexes);
        while (changedIndexesIter.hasNext()) {
            int currentIndex = changedIndexesIter.next();
            if (startIndex == -1)
                startIndex = currentIndex;

            if (!changedIndexesIter.hasNext() || (changedIndexesIter.hasNext() && changedIndexesIter.peekNext() > (currentIndex +1))) {
                for (int i = startIndex; i <= currentIndex; ++i) {
                    const QPlaceContent &content = contents.value(i);
                    m_content.insert(i, content);
                    if (!m_suppliers.contains(content.supplier().supplierId())) {
                        m_suppliers.insert(content.supplier().supplierId(),
                                           new QDeclarativeSupplier(content.supplier(), m_place->plugin(), this));
                    }
                    if (!m_users.contains(content.user().userId())) {
                        m_users.insert(content.user().userId(),
                                           new QDeclarativePlaceUser(content.user(), this));
                    }
                }
                emit dataChanged(index(startIndex),index(currentIndex));
                startIndex = -1;
            }
        }
    }

    reply->deleteLater();
}

QT_END_NAMESPACE
