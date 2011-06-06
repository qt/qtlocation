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

#include "qdeclarativelandmarkcategorymodel_p.h"
#include <qlandmarkcategoryfetchrequest.h>
#include <qlandmarkcategoryfetchbyidrequest.h>
#include <QTimer>

#ifdef QDECLARATIVE_LANDMARK_DEBUG
#include <QDebug>
#endif

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass LandmarkCategoryModel QDeclarativeLandmarkCategoryModel
    \brief The LandmarkCategoryModel element provides access to categories.
    \inherits LandmarkAbstractModel
    \ingroup qml-location-landmarks

    This element is part of the \bold{QtMobility.location 1.1} module.

    LandmarkCategoryModel provides a model of categories from the categories store.
    The contents of the model can be specified with \l landmark, and sorted
    via the \l LandmarkAbstractModel::sortBy and \l LandmarkAbstractModel::sortOrder properties.
    Whether the model is automatically updated when the store or \l landmark changes, can be
    controlled with \l LandmarkAbstractModel::autoUpdate property.

    There are two ways of accessing the category data: via model by using views and delegates,
    or alternatively via \l categories list property. Of the two, the model access is preferred.
    Direct list access (i.e. non-model) is not guaranteed to be in order set by sortBy and
    sortOrder.

    At the moment only data role provided by the model is \c category.
    Through that one can access any data provided by the LandmarkCategory element.

    \snippet examples/declarative-location/landmarkmap/landmarkmap.qml Category model

    \sa LandmarkAbstractModel, LandmarkModel, {QLandmarkManager}
*/

QDeclarativeLandmarkCategoryModel::QDeclarativeLandmarkCategoryModel(QObject *parent) :
        QDeclarativeLandmarkAbstractModel(parent), m_fetchRequest(0), m_landmark(0)
{
    // Establish role names so that they can be queried from this model
    QHash<int, QByteArray> roleNames;
    roleNames = QAbstractItemModel::roleNames();
    roleNames.insert(CategoryRole, "category");
    setRoleNames(roleNames);
}

QDeclarativeLandmarkCategoryModel::~QDeclarativeLandmarkCategoryModel()
{
    delete m_fetchRequest;
    delete m_sortingOrder;
    qDeleteAll(m_categoryMap.values());
    m_categoryMap.clear();
}

// When the parent is valid it means that rowCount is returning the number of children of parent.
int QDeclarativeLandmarkCategoryModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_categories.count();
}

// Returns the stored under the given role for the item referred to by the index.
QVariant QDeclarativeLandmarkCategoryModel::data(const QModelIndex &index, int role) const
{
    QLandmarkCategory category = m_categories.value(index.row());

    switch (role) {
        case Qt::DisplayRole:
            return category.name();
        case CategoryRole:
            return QVariant::fromValue(m_categoryMap.value(category.categoryId().localId()));
    }
    return QVariant();
}

QDeclarativeLandmark* QDeclarativeLandmarkCategoryModel::landmark() const
{
    return m_landmark;
}

/*!
  \qmlproperty Landmark LandmarkCategoryModel::landmark

  Landmark whose categories the model should represent.
  Note that the landmark needs to be from \l LandmarkModel
  because its internal category identifiers need to be set.

 */

void QDeclarativeLandmarkCategoryModel::setLandmark(QDeclarativeLandmark *landmark)
{
    if (landmark == m_landmark)
        return;
    m_landmark = landmark;
    if (m_autoUpdate)
        scheduleUpdate();
    emit landmarkChanged();
}

int QDeclarativeLandmarkCategoryModel::count() const
{
    return m_categories.count();
}

void QDeclarativeLandmarkCategoryModel::startUpdate()
{
#ifdef QDECLARATIVE_LANDMARK_DEBUG
    qDebug("QDeclarativeLandmarkCategoryModel::startUpdate()");
#endif
    if (!m_manager) {
        m_updatePending = false;
        return;
    }
    // Clear any previous updates and request new
    cancelUpdate();
    if (m_landmark) {
        QLandmarkCategoryFetchByIdRequest* req = new QLandmarkCategoryFetchByIdRequest(m_manager, this);
        req->setCategoryIds(m_landmark->categoryIds());
        m_fetchRequest = req;
    } else {
        m_fetchRequest = new QLandmarkCategoryFetchRequest(m_manager, this);
        setFetchRange();
        setFetchOrder();
    }
    QObject::connect(m_fetchRequest, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)), this, SLOT(fetchRequestStateChanged(QLandmarkAbstractRequest::State)));
    m_fetchRequest->start();
    m_updatePending = false; // Allow requesting updates again
}

void QDeclarativeLandmarkCategoryModel::setFetchRange()
{
    if (!m_fetchRequest || ((m_limit <= 0) && (m_offset <= 0)) ||
        (m_fetchRequest->type() != QLandmarkAbstractRequest::CategoryFetchRequest))
        return;
    QLandmarkCategoryFetchRequest* req = static_cast<QLandmarkCategoryFetchRequest*>(m_fetchRequest);
    if (m_limit > 0)
        req->setLimit(m_limit);
    if ((m_offset > 0))
        req->setOffset(m_offset);
}

void QDeclarativeLandmarkCategoryModel::setFetchOrder()
{
    if (!m_fetchRequest ||
        ((m_sortKey == NoSort) && (m_sortOrder == NoOrder)) ||
        m_fetchRequest->type() != QLandmarkAbstractRequest::CategoryFetchRequest)
        return;
    if (m_sortingOrder) {
        delete m_sortingOrder;
        m_sortingOrder = 0;
    }
    if (m_sortKey == NameSort) {
        m_sortingOrder = new QLandmarkNameSort();
    } else {
        m_sortingOrder = new QLandmarkSortOrder();
    }
    if (m_sortOrder != NoOrder)
        m_sortingOrder->setDirection((Qt::SortOrder)m_sortOrder);
    static_cast<QLandmarkCategoryFetchRequest*>(m_fetchRequest)->setSorting(*m_sortingOrder);
}

void QDeclarativeLandmarkCategoryModel::cancelUpdate()
{
#ifdef QDECLARATIVE_LANDMARK_DEBUG
    qDebug("QDeclarativeLandmarkCategoryModel::cancelUpdate()");
#endif
    if (m_fetchRequest) {
        delete m_fetchRequest;
        m_fetchRequest = 0;
    }
}

void QDeclarativeLandmarkCategoryModel::convertCategoriesToDeclarative()
{
    QList<QString> categoriesToRemove = m_categoryMap.keys();

    foreach(const QLandmarkCategory& category, m_categories) {
        if (!m_categoryMap.contains(category.categoryId().localId())) {
            QDeclarativeLandmarkCategory* declarativeLandmarkCategory = new QDeclarativeLandmarkCategory(this);
            declarativeLandmarkCategory->setCategory(category);
            m_categoryMap.insert(category.categoryId().localId(), declarativeLandmarkCategory);
        } else {
            // The landmark exists already, update it
            m_categoryMap.value(category.categoryId().localId())->setCategory(category);
            // Item is still valid, remove it from the list of removables
            categoriesToRemove.removeOne(category.categoryId().localId());
        }
    }
    foreach (const QString removable, categoriesToRemove) {
        delete m_categoryMap.value(removable);
        m_categoryMap.remove(removable);
    }
    emit categoriesChanged();
}

void QDeclarativeLandmarkCategoryModel::fetchRequestStateChanged(QLandmarkAbstractRequest::State state)
{
#ifdef QDECLARATIVE_LANDMARK_DEBUG
    qDebug() << "QDeclarativeLandmarkCategoryModel::Fetch request finished with state: " << state;
    if (m_fetchRequest)
        qDebug() << "and related request error code is: " << m_fetchRequest->errorString();
#endif
    if (!m_fetchRequest || state != QLandmarkAbstractRequest::FinishedState)
        return;

    if (m_fetchRequest->error() == QLandmarkManager::NoError) {
        // Later improvement item is to make udpate incremental by
        // connecting to resultsAvailable() -function.
        beginResetModel();
        int oldCount = m_categories.count();
        switch (m_fetchRequest->type())
        {
        case QLandmarkAbstractRequest::CategoryFetchRequest:
            {
            QLandmarkCategoryFetchRequest* req = static_cast<QLandmarkCategoryFetchRequest*>(m_fetchRequest);
            m_categories = req->categories();
            }
            break;
        case QLandmarkAbstractRequest::CategoryFetchByIdRequest:
            {
            QLandmarkCategoryFetchByIdRequest* req = static_cast<QLandmarkCategoryFetchByIdRequest*>(m_fetchRequest);
            m_categories = req->categories();
            }
            break;

        default:
            // No other types supported
            return;
        }
        // Convert into declarative classes
        convertCategoriesToDeclarative();
        endResetModel();
        if (!(oldCount == 0 && m_categories.count() == 0))
            emit modelChanged();
        if (oldCount != m_categories.count())
            emit countChanged();
    } else if (m_error != m_fetchRequest->errorString()) {
        m_error = m_fetchRequest->errorString();
        emit errorChanged();
    }
}

// For testing purposes in order to access ordered data (i.e. as filters dictate)
QList<QLandmarkCategory> QDeclarativeLandmarkCategoryModel::categoryList()
{
    return m_categories;
}

/*!
    \qmlproperty QDeclarativeListProperty LandmarkCategoryModel::categories

    This element holds the list of \l LandmarkCategory elements that the model currently has.
    Accessing categories by iterating over this list is not guaranteed to be in the
    order set by \l LandmarkAbstractModel::sortBy or \l LandmarkAbstractModel::sortOrder

    \snippet doc/src/snippets/declarative/declarative-landmark.qml Categories list iteration

*/

QDeclarativeListProperty<QDeclarativeLandmarkCategory> QDeclarativeLandmarkCategoryModel::categories()
{
    return QDeclarativeListProperty<QDeclarativeLandmarkCategory>(this,
                                                          0, // opaque data parameter
                                                          categories_append,
                                                          categories_count,
                                                          categories_at,
                                                          categories_clear);
}

void QDeclarativeLandmarkCategoryModel::categories_append(QDeclarativeListProperty<QDeclarativeLandmarkCategory>* prop, QDeclarativeLandmarkCategory* category)
{
    Q_UNUSED(prop);
    Q_UNUSED(category);
    qWarning() << "LandmarkModel: appending categories is not currently supported";
}

int QDeclarativeLandmarkCategoryModel::categories_count(QDeclarativeListProperty<QDeclarativeLandmarkCategory>* prop)
{
    // The 'prop' is in a sense 'this' for this static function (as given in categories() function)
    return static_cast<QDeclarativeLandmarkCategoryModel*>(prop->object)->m_categoryMap.values().count();
}

QDeclarativeLandmarkCategory* QDeclarativeLandmarkCategoryModel::categories_at(QDeclarativeListProperty<QDeclarativeLandmarkCategory>* prop, int index)
{
    return static_cast<QDeclarativeLandmarkCategoryModel*>(prop->object)->m_categoryMap.values().at(index);
}

void QDeclarativeLandmarkCategoryModel::categories_clear(QDeclarativeListProperty<QDeclarativeLandmarkCategory>* prop)
{
    QDeclarativeLandmarkCategoryModel* model = static_cast<QDeclarativeLandmarkCategoryModel*>(prop->object);
    QMap<QString, QDeclarativeLandmarkCategory*>* categoryMap = &model->m_categoryMap;
    qDeleteAll(categoryMap->values());
    categoryMap->clear();
    model->m_categories.clear();
    emit model->categoriesChanged();
}

#include "moc_qdeclarativelandmarkcategorymodel_p.cpp"

QTM_END_NAMESPACE
