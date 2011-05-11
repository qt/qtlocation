/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
***************************************************************************/

#include "qdeclarativelandmarkmodel_p.h"
#include <QtDeclarative/qdeclarativeinfo.h>
#include <QTimer>

#ifdef QDECLARATIVE_LANDMARK_DEBUG
#include <QDebug>
#endif

QTM_BEGIN_NAMESPACE


/*!
    \qmlclass LandmarkAbstractModel QDeclarativeLandmarkAbstractModel
    \brief The LandmarkAbstractModel element is an uncreatable /
    uninstantiable base element for LandmarkModel and LandmarkCategoryModel.
    It defines many common properties for the two models.
    \ingroup qml-location-landmarks

    The LandmarkAbstractModel element is an uncreatable /
    uninstantiable base element for LandmarkModel and LandmarkCategoryModel.
    It defines many common properties for the two models.

    This element is part of the \bold{QtMobility.location 1.1} module.

    \sa LandmarkModel, LandmarkCategoryModel
*/

QDeclarativeLandmarkAbstractModel::QDeclarativeLandmarkAbstractModel(QObject *parent) :
        QAbstractListModel(parent), m_manager(0),
        m_componentCompleted(false), m_updatePending(false), m_autoUpdate(true),
        m_limit(-1), m_offset(-1), m_sortingOrder(0), m_sortOrder(NoOrder), m_sortKey(NoSort)
{
}

/*!
    \qmlsignal LandmarkAbstractModel::modelChanged()

    This handler is called when the model data has changed.

*/

/*!
    \qmlsignal LandmarkAbstractModel::databaseChanged()

    This handler is called when the data in the underlying data
    storage has changed. If \l autoUpdate is true, model data
    will be updated shortly afterwards.
*/


QDeclarativeLandmarkAbstractModel::~QDeclarativeLandmarkAbstractModel()
{
    delete m_manager;
}

void QDeclarativeLandmarkAbstractModel::componentComplete()
{
    m_componentCompleted = true;
    if (!m_manager) {
        m_manager = new QLandmarkManager();
        connectManager();
    }
    if (m_autoUpdate) {
        scheduleUpdate();
    }
}

void QDeclarativeLandmarkAbstractModel::connectManager()
{
    if (!m_manager)
        return;
    connect(m_manager, SIGNAL(categoriesAdded(QList<QLandmarkCategoryId>)),
            this, SLOT(categoriesChanged(QList<QLandmarkCategoryId>)));
    connect(m_manager, SIGNAL(categoriesChanged(QList<QLandmarkCategoryId>)),
            this, SLOT(categoriesChanged(QList<QLandmarkCategoryId>)));
    connect(m_manager, SIGNAL(categoriesRemoved(QList<QLandmarkCategoryId>)),
            this, SLOT(categoriesChanged(QList<QLandmarkCategoryId>)));
    connect(m_manager, SIGNAL(landmarksAdded(QList<QLandmarkId>)),
            this, SLOT(landmarksChanged(QList<QLandmarkId>)));
    connect(m_manager, SIGNAL(landmarksChanged(QList<QLandmarkId>)),
            this, SLOT(landmarksChanged(QList<QLandmarkId>)));
    connect(m_manager, SIGNAL(landmarksRemoved(QList<QLandmarkId>)),
            this, SLOT(landmarksChanged(QList<QLandmarkId>)));
    connect(m_manager, SIGNAL(dataChanged()),
            this, SLOT(dataChanged()));
}

void QDeclarativeLandmarkAbstractModel::categoriesChanged(const QList<QLandmarkCategoryId>& ids)
{
    Q_UNUSED(ids)
    if (m_autoUpdate)
        update();
    emit databaseChanged();
}

void QDeclarativeLandmarkAbstractModel::landmarksChanged(const QList<QLandmarkId>& ids)
{
    Q_UNUSED(ids)
    if (m_autoUpdate)
        update();
    emit databaseChanged();
}

void QDeclarativeLandmarkAbstractModel::dataChanged()
{
    if (m_autoUpdate)
        update();
    emit databaseChanged();
}

/*!
    \qmlproperty bool LandmarkAbstractModel::autoUpdate

    Controls whether the model data should be automatically updated when
    appropriate events occur: limit changes, offset changes, filter changes,
    filter contents change, or sorting rules change. Property also controls
    whether the data is updated automatically once model instantiation is completed.

    Note: while autoUpdate adds to development comfort, each change in appropriate parameters
    cause full fetch from underlying database and therefore its use should be thought through.
    E.g. if your application inherently behaves such that filters, their contents,
    and underlying database change very frequently, it may be wiser to bind a manual
    \l update call to a less hectic trigger source.

*/

void QDeclarativeLandmarkAbstractModel::setAutoUpdate(bool autoUpdate)
{
    if (autoUpdate == m_autoUpdate)
        return;
    m_autoUpdate = autoUpdate;
    emit autoUpdateChanged();
}

bool QDeclarativeLandmarkAbstractModel::autoUpdate() const
{
    return m_autoUpdate;
}

void QDeclarativeLandmarkAbstractModel::scheduleUpdate()
{
    if (!m_componentCompleted || m_updatePending)
        return;
    m_updatePending = true; // Disallow possible duplicate request triggering
    QMetaObject::invokeMethod(this, "startUpdate", Qt::QueuedConnection);
}

/*!
    \qmlproperty string LandmarkAbstractModel::error

    Human readable string of the last error that occurred
    (read-only).

*/

QString QDeclarativeLandmarkAbstractModel::error() const
{
    return m_error;
}

// This is purely for testing purposes, not part of public interface
void QDeclarativeLandmarkAbstractModel::setDbFileName(QString fileName)
{
    if (fileName == m_dbFileName)
        return;
    m_dbFileName = fileName;
    if (m_manager) {
        cancelUpdate();
        cancelImport();
        delete m_manager;
        m_manager = 0;
    }
    QMap<QString, QString> map;
    map["filename"] = m_dbFileName;
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_6) || defined(Q_WS_MEEGO)
    m_manager = new QLandmarkManager();
#else
    m_manager = new QLandmarkManager("com.nokia.qt.landmarks.engines.sqlite", map);
#endif
    connectManager();
    if (m_autoUpdate)
        scheduleUpdate();
}


/*!
    \qmlmethod LandmarkAbstractModel::update()

    Updates the items represented by the model from the
    underlying store.

    \sa autoUpdate
*/

void QDeclarativeLandmarkAbstractModel::update()
{
    scheduleUpdate();
}

int QDeclarativeLandmarkAbstractModel::limit()
{
    return m_limit;
}

/*!
    \qmlproperty int LandmarkAbstractModel::limit

    Sets the maximum amount of items held by the model.
*/

void QDeclarativeLandmarkAbstractModel::setLimit(int limit)
{
    if (limit == m_limit)
        return;
    m_limit = limit;
    if (m_autoUpdate) {
        scheduleUpdate();
    }
    emit limitChanged();
}

int QDeclarativeLandmarkAbstractModel::offset()
{
    return m_offset;
}

/*!
    \qmlproperty int LandmarkAbstractModel::offset

    Sets the offset for the landmarks. For example if
    one creates a tabbed / paged application, the offset
    could be changed dynamically by a multiple of items per page.

*/

void QDeclarativeLandmarkAbstractModel::setOffset(int offset)
{
    if (offset == m_offset)
        return;
    m_offset = offset;
    if (m_autoUpdate) {
        scheduleUpdate();
    }
    emit offsetChanged();
}

QDeclarativeLandmarkAbstractModel::SortKey QDeclarativeLandmarkAbstractModel::sortBy() const
{
    return m_sortKey;
}

/*!
    \qmlproperty enumeration LandmarkAbstractModel::sortBy

    Specifies the role to sort the items by.

    Note: both sortBy and sortOrder incur performance costs.
    If the order is not important (e.g. when displaying landmarks
    as Map objects) it is advisable not to set them.

    \list
    \o LandmarkAbstractModel.None (default)
    \o LandmarkAbstractModel.NameSort
    \endlist
*/

void QDeclarativeLandmarkAbstractModel::setSortBy(QDeclarativeLandmarkAbstractModel::SortKey key)
{
    if (key == m_sortKey)
        return;
    m_sortKey = key;
    if (m_autoUpdate) {
        scheduleUpdate();
    }
    emit sortByChanged();
}

QDeclarativeLandmarkAbstractModel::SortOrder QDeclarativeLandmarkAbstractModel::sortOrder() const
{
    return m_sortOrder;
}

/*!
    \qmlproperty enumeration LandmarkAbstractModel::sortOrder

    Specifies the sort order.

    Note: both sortBy and sortOrder incur performance costs.
    If the order is not important (e.g. when displaying landmarks
    as Map objects) it is advisable not to set them.

    \list
    \o LandmarkAbstractModel.AscendingOrder
    \o LandmarkAbstractModel.DescendingOrder
    \endlist
*/

void QDeclarativeLandmarkAbstractModel::setSortOrder(QDeclarativeLandmarkAbstractModel::SortOrder order)
{
    if (order == m_sortOrder)
        return;
    m_sortOrder = order;
    if (m_autoUpdate) {
        scheduleUpdate();
    }
    emit sortOrderChanged();
}

/*!
    \qmlclass LandmarkModel QDeclarativeLandmarkModel
    \brief The LandmarkModel element provides access to landmarks.
    \inherits LandmarkAbstractModel
    \ingroup qml-location-landmarks

    This element is part of the \bold{QtMobility.location 1.1} module.

    LandmarkModel provides a model of landmarks from the landmarks store.
    The contents of the model can be specified with a \l filter, and sorted
    with the \l LandmarkAbstractModel::sortBy and \l LandmarkAbstractModel::sortOrder properties.
    Whether the model is automatically updated when the store or filter changes, can be controlled
    with \l LandmarkAbstractModel::autoUpdate property.

    There are two ways of accessing the landmark data: through model by using views and delegates,
    or alternatively via \l landmarks list property. Of the two, the model access is preferred.
    Direct list access (i.e. non-model) is not guaranteed to be in order set by sortBy and
    sortOrder.

    At the moment only data role provided by the model is \c landmark (\l Landmark).
    Through that one can access any data provided by the Landmark element.

    The following example illustrates fetching of all landmarks in ascending name order.
    Example illustrates both model access as well as direct list access.

    \snippet doc/src/snippets/declarative/declarative-landmark.qml LandmarkModel example

    \sa LandmarkAbstractModel, LandmarkCategoryModel, {QLandmarkManager}
*/

QDeclarativeLandmarkModel::QDeclarativeLandmarkModel(QObject *parent) :
        QDeclarativeLandmarkAbstractModel(parent),
        m_filter(0), m_fetchRequest(0), m_importRequest(0), m_importPending(false)
{
    // Establish role names so that they can be queried from this model
    QHash<int, QByteArray> roleNames;
    roleNames = QAbstractItemModel::roleNames();
    roleNames.insert(LandmarkRole, "landmark");
    setRoleNames(roleNames);
}

QDeclarativeLandmarkModel::~QDeclarativeLandmarkModel()
{
    delete m_fetchRequest;
    delete m_sortingOrder;
    delete m_importRequest;

    qDeleteAll(m_declarativeLandmarks);
    m_declarativeLandmarks.clear();
    m_landmarkIdSet.clear();
    m_landmarks.clear();
}

// When the parent is valid it means that rowCount is returning the number of children of parent.
int QDeclarativeLandmarkModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_landmarks.count();
}

// Returns the stored under the given role for the item referred to by the index.
QVariant QDeclarativeLandmarkModel::data(const QModelIndex& index, int role) const
{
    QDeclarativeLandmark *declarativeLandmark = m_declarativeLandmarks.at(index.row());

    switch (role) {
    case Qt::DisplayRole:
        if (declarativeLandmark)
            return declarativeLandmark->landmark().name();
    case LandmarkRole:
        if (declarativeLandmark)
            return QVariant::fromValue(declarativeLandmark);
    }
    return QVariant();
}

QDeclarativeListProperty<QDeclarativeLandmark> QDeclarativeLandmarkModel::landmarks()
{
    return QDeclarativeListProperty<QDeclarativeLandmark>(this,
                                                          0, // opaque data parameter
                                                          landmarks_append,
                                                          landmarks_count,
                                                          landmarks_at,
                                                          landmarks_clear);
}

QDeclarativeLandmarkFilterBase* QDeclarativeLandmarkModel::filter()
{
    return m_filter;
}

/*!
    \qmlproperty LandmarkFilterBase LandmarkModel::filter

    The filter for filtering landmarks.

    \sa LandmarkUnionFilter, LandmarkIntersectionFilter, LandmarkNameFilter,
    LandmarkProximityFilter, LandmarkBoxFilter, LandmarkCategoryFilter

    \snippet doc/src/snippets/declarative/declarative-landmark.qml LandmarkModel filter

*/

void QDeclarativeLandmarkModel::setFilter(QDeclarativeLandmarkFilterBase* filter)
{
    m_filter = filter;
    // Connect to listen for filters' content changes
    if (filter)
        QObject::connect(filter, SIGNAL(filterContentChanged()), this, SLOT(filterContentChanged()));
    if (m_autoUpdate) {
        scheduleUpdate();
    }
    emit filterChanged();
}

void QDeclarativeLandmarkModel::filterContentChanged()
{
    if (m_autoUpdate) {
#ifdef QDECLARATIVE_LANDMARK_DEBUG
        qDebug() << "QDeclarativeLandmarkModel::filterContentChanged(), scheduling update.";
#endif
    scheduleUpdate();
    }
}

void QDeclarativeLandmarkModel::startUpdate()
{
    if (!m_manager) {
        m_updatePending = false;
        return;
    }
    // Clear any previous updates and request new
    cancelUpdate();
    m_fetchRequest = new QLandmarkFetchRequest(m_manager, this);
    QObject::connect(m_fetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)), this, SLOT(fetchRequestStateChanged(QLandmarkAbstractRequest::State)));
    // Set filters, orders, and limits
    if (m_filter) {
        QLandmarkFilter* filter = m_filter->filter();
        if (filter)
            m_fetchRequest->setFilter(*filter);
    }
    setFetchRange();
    setFetchOrder();
#ifdef QDECLARATIVE_LANDMARK_DEBUG
    qDebug() << "============ Calling start for the fetch request: " << m_fetchRequest << " whose manager is: " << m_manager;
#endif
    m_fetchRequest->start();
    m_updatePending = false; // Allow requesting updates again
}

void QDeclarativeLandmarkModel::cancelUpdate()
{
    if (m_fetchRequest) {
#ifdef QDECLARATIVE_LANDMARK_DEBUG
        qDebug() << "QDeclarativeLandmarkModel::cancelUpdate() deleting m_fetchRequest:" << m_fetchRequest;
#endif
        delete m_fetchRequest;
        m_fetchRequest = 0;
    }
    if (m_sortingOrder) {
        delete m_sortingOrder;
        m_sortingOrder = 0;
    }
}

/*!
    \qmlmethod LandmarkModel::importLandmarks()

    Imports landmarks (and categories) in \l importFile.
*/

void QDeclarativeLandmarkModel::importLandmarks()
{
    scheduleImport();
}

void QDeclarativeLandmarkModel::cancelImport()
{
    if (m_importRequest) {
#ifdef QDECLARATIVE_LANDMARK_DEBUG
    qDebug() << "QDeclarativeLandmarkModel::cancelImport() deleting m_importRequest:" << m_importRequest;
#endif
        delete m_importRequest;
        m_importRequest = 0;
    }
}

/*!
    \qmlproperty int LandmarkAbstractModel::count

    Indicates the number of items currently in the model
    (landmarks/categories).

*/

int QDeclarativeLandmarkModel::count()
{
    return m_landmarks.count();
}

void QDeclarativeLandmarkModel::setFetchRange()
{
    if (!m_fetchRequest || ((m_limit <= 0) && (m_offset <= 0)))
        return;
    if (m_limit > 0)
        m_fetchRequest->setLimit(m_limit);
    if ((m_offset > 0))
        m_fetchRequest->setOffset(m_offset);
}

void QDeclarativeLandmarkModel::setFetchOrder()
{
    if (!m_fetchRequest || ((m_sortKey == NoSort) && (m_sortOrder == NoOrder)))
        return;
    if (m_sortingOrder)
        delete m_sortingOrder;
    if (m_sortKey == NameSort) {
        m_sortingOrder = new QLandmarkNameSort(); // Only supported sort type
    } else {
        m_sortingOrder = new QLandmarkSortOrder();
    }
    if (m_sortOrder != NoOrder)
        m_sortingOrder->setDirection((Qt::SortOrder)m_sortOrder);
    m_fetchRequest->setSorting(*m_sortingOrder);
}

/*!
    \qmlproperty QDeclarativeListProperty LandmarkModel::landmarks

    This element holds the list of \l Landmark elements that the model currently has.
    Accessing landmarks by iterating over this list is not guaranteed to be in the
    order set by \l LandmarkAbstractModel::sortBy or \l LandmarkAbstractModel::sortOrder

    \snippet doc/src/snippets/declarative/declarative-landmark.qml LandmarkModel landmarks iteration

*/

void QDeclarativeLandmarkModel::landmarks_append(QDeclarativeListProperty<QDeclarativeLandmark>* prop, QDeclarativeLandmark* landmark)
{
    Q_UNUSED(prop);
    Q_UNUSED(landmark);
    qWarning() << "LandmarkModel: appending landmarks is not currently supported";
}

int QDeclarativeLandmarkModel::landmarks_count(QDeclarativeListProperty<QDeclarativeLandmark>* prop)
{
    // The 'prop' is in a sense 'this' for this static function (as given in landmarks() function)
    return static_cast<QDeclarativeLandmarkModel*>(prop->object)->m_declarativeLandmarks.count();
}

QDeclarativeLandmark* QDeclarativeLandmarkModel::landmarks_at(QDeclarativeListProperty<QDeclarativeLandmark>* prop, int index)
{
    return static_cast<QDeclarativeLandmarkModel*>(prop->object)->m_declarativeLandmarks.at(index);
}

void QDeclarativeLandmarkModel::landmarks_clear(QDeclarativeListProperty<QDeclarativeLandmark>* prop)
{
    QDeclarativeLandmarkModel* model = static_cast<QDeclarativeLandmarkModel*>(prop->object);
    qDeleteAll(model->m_declarativeLandmarks);
    model->m_declarativeLandmarks.clear();
    model->m_landmarks.clear();
    emit model->landmarksChanged();
}

bool QDeclarativeLandmarkModel::convertLandmarksToDeclarative()
{
    bool changed = false;
    if (m_sortOrder == NoOrder && m_sortKey == NoSort) {
        Q_ASSERT(m_declarativeLandmarks.count() == m_landmarkIdSet.count());
        // If order does not matter, we can up update without reseting the entire model
        // which will enable performance improvements in the view as it does not need
        // to recreate all items (which may have significant cost in rendering engine).
        // Note: this optimization does not take into account changes in the landmark's
        // contents (e.g name or phone number changes). Roughly ~O(2n) + O(rm) + O(new)
        // -cost but we need to enable removing and adding range of items
        // rather than sending signal for each removed item (typically all may change
        // if user pans map). Also we need to be able first to remove obsolete items
        // and only then append new ones.
        QList<int> indexListOfNewItems;
        QSet<QString> landmarksToRemoveIdSet = m_landmarkIdSet;
        for (int i = 0; i < m_landmarks.count(); ++i) {
            QString landmarkId = m_landmarks.at(i).landmarkId().localId();
            if (!m_landmarkIdSet.contains(landmarkId)) {
                 // After looping contains all new items
                indexListOfNewItems.append(i);
                changed = true;
            }
            else {
                // After looping obsolete items will remain
                landmarksToRemoveIdSet.remove(landmarkId);
                changed = true;
            }
        }
        // Find continuous areas for removals, starting from the end so we can remove on the fly.
        int bookmark = m_declarativeLandmarks.count() - 1;
        int landmarksToRemove = landmarksToRemoveIdSet.count();
        while (landmarksToRemove > 0) {
            int removeBegin = -1; // will be index starting from the end
            int removablesInBlock = 0;
            while (bookmark >= 0) {
                QString landmarkId = m_declarativeLandmarks.at(bookmark)->landmark().landmarkId().localId();
                if (landmarksToRemoveIdSet.contains(landmarkId)) {
                    // Landmark is obsolete, mark start of removable block.
                    if (removeBegin == -1) {
                        removeBegin = bookmark;
                    }
                    m_landmarkIdSet.remove(landmarkId);
                    removablesInBlock++;
                    bookmark--;
                } else {
                    // Landmark is a keeper. Remove the block of obsolete landmarks detected so far.
                    bookmark--;
                    break; // this block of removables is ready to be removed
                }
            }
            if (removeBegin != -1 && removablesInBlock > 0) {
                beginRemoveRows(QModelIndex(), removeBegin + 1 - removablesInBlock, removeBegin);
                for (int i = 0; i < removablesInBlock; ++i)
                    delete m_declarativeLandmarks.takeAt(removeBegin + 1 - removablesInBlock);
                endRemoveRows();
                landmarksToRemove -= removablesInBlock;
             }
        } // while landmarks to remove
        // Append the new items
        if (!indexListOfNewItems.isEmpty()) {
            int insertBegin = m_declarativeLandmarks.count();
            beginInsertRows(QModelIndex(), insertBegin, insertBegin + indexListOfNewItems.count() - 1);
            for (int i = 0; i < indexListOfNewItems.count(); ++i) {
                QDeclarativeLandmark* declarativeLandmark = new QDeclarativeLandmark(m_landmarks.at(indexListOfNewItems.at(i)), this);
                m_declarativeLandmarks.append(declarativeLandmark);
                m_landmarkIdSet.insert(m_landmarks.at(indexListOfNewItems.at(i)).landmarkId().localId());
            }
            endInsertRows();
        }
    } else {
        // Order matters, reset the whole model. Brutal but otherwise we would need
        // to create complex logic to detect item removals, additions and
        // moves. This could be optimized to only occur when sortOrder and/or sortBy
        // have changed. However e.g. user panning the map will likely cause complex and costly
        // update sequences (but definately here is room for improvement if this
        // turns in to a bottleneck).
        changed = true;
        beginResetModel();
        qDeleteAll(m_declarativeLandmarks);
        m_declarativeLandmarks.clear();
        m_landmarkIdSet.clear();
        foreach(const QLandmark& landmark, m_landmarks) {
            QDeclarativeLandmark* declarativeLandmark = new QDeclarativeLandmark(landmark, this);
            m_declarativeLandmarks.append(declarativeLandmark);
            m_landmarkIdSet.insert(landmark.landmarkId().localId());
        }
        endResetModel();
    }
    emit landmarksChanged();
    return changed;
}

QString QDeclarativeLandmarkModel::importFile() const
{
    return m_importFile;
}

/*!
  \qmlproperty string LandmarkModel::importFile

  Landmarks and their related categories can be imported by setting
  the import file. If \l LandmarkAbstractModel::autoUpdate is true, the import will be done
  automatically and as a result model updates. Alternatively \l importLandmarks() can be invoked.

  For supported file formats, refer to \l QLandmarkManager and \l QLandmarkImportRequest.

  \snippet doc/src/snippets/declarative/declarative-landmark.qml LandmarkModel import file

  */

void QDeclarativeLandmarkModel::setImportFile(QString importFile)
{
    if (importFile == m_importFile)
        return;
    m_importFile = importFile;
    if (m_importRequest) {
        delete m_importRequest;
        m_importRequest = 0;
    }
    if (m_autoUpdate)
        scheduleImport();
    emit importFileChanged();
}

void QDeclarativeLandmarkModel::scheduleImport()
{
    if (!m_componentCompleted || m_importPending)
        return;
    m_importPending = true; // Disallow possible duplicate request triggering
    QMetaObject::invokeMethod(this, "startImport", Qt::QueuedConnection);
}

void QDeclarativeLandmarkModel::componentComplete()
{
    m_componentCompleted = true;
    if (!m_manager) {
        m_manager = new QLandmarkManager();
        connectManager();
    }
    if (m_autoUpdate) {
        if (!m_importFile.isEmpty())
            scheduleImport();
        scheduleUpdate();
    }
}

void QDeclarativeLandmarkModel::startImport()
{
    if (!m_manager || m_importFile.isEmpty()) {
        qmlInfo(this) << tr("Cannot import, import file is empty.");
        m_importPending = false;
        return;
    }
    if (!QFile::exists(m_importFile))
        qmlInfo(this) << tr("Import file not found.");

    if (m_importRequest)
        delete m_importRequest;
    m_importRequest = new QLandmarkImportRequest(m_manager);
    m_importRequest->setFileName(m_importFile);
#ifdef QDECLARATIVE_LANDMARK_DEBUG
    qDebug() << "============ Calling start for the import request: " << m_importRequest << " whose manager is: " << m_manager << "and file is:" << m_importFile;

#endif
    m_importRequest->start(); // If successful, will result in landmark/category added signals
    m_importPending = false;
}


QList<QLandmark> QDeclarativeLandmarkModel::landmarkList()
{
    return m_landmarks;
}

void QDeclarativeLandmarkModel::fetchRequestStateChanged(QLandmarkAbstractRequest::State state)
{
#ifdef QDECLARATIVE_LANDMARK_DEBUG
    qDebug() << "QDeclarativeLandmarkModel::Fetch request finished with state: " << state;
    if (m_fetchRequest) {
        qDebug() << "and related request error code is: " << m_fetchRequest->errorString();
        qDebug() << "and received this many landmarks:" << m_fetchRequest->landmarks().count();
    }
#endif
    if (!m_fetchRequest || state != QLandmarkAbstractRequest::FinishedState)
        return;

    if (m_fetchRequest->error() == QLandmarkManager::NoError) {
        // Later improvement item is to make udpate incremental by connecting to resultsAvailable() -function.
        int oldCount = m_landmarks.count();
        m_landmarks = m_fetchRequest->landmarks();
        // Convert into declarative classes
        if (convertLandmarksToDeclarative()) {
            emit modelChanged();
        }
        if (oldCount != m_landmarks.count())
            emit countChanged();
    } else if (m_error != m_fetchRequest->errorString()) {        
        m_error = m_fetchRequest->errorString();
        emit errorChanged();
    }
}

#include "moc_qdeclarativelandmarkmodel_p.cpp"

QTM_END_NAMESPACE
