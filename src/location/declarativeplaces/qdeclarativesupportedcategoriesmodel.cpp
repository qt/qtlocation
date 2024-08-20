// Copyright (C) 2014 Aaron McCarthy <mccarthy.aaron@gmail.com>
// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qdeclarativesupportedcategoriesmodel_p.h"
#include "qgeoserviceprovider.h"
#include "error_messages_p.h"
#include <QtCore/private/qobject_p.h>

#include <QCoreApplication>
#include <QtQml/QQmlInfo>
#include <QtLocation/QPlaceManager>
#include <QtLocation/QPlaceIcon>

QT_BEGIN_NAMESPACE

/*!
    \qmltype CategoryModel
    \nativetype QDeclarativeSupportedCategoriesModel
    \inqmlmodule QtLocation
    \ingroup qml-QtLocation5-places
    \ingroup qml-QtLocation5-places-models
    \since QtLocation 5.5

    \brief The CategoryModel type provides a model of the categories supported by a \l Plugin.

    The CategoryModel type provides a model of the categories that are available from the
    current \l Plugin.  The model supports both a flat list of categories and a hierarchical tree
    representing category groupings.  This can be controlled by the \l hierarchical property.

    The model supports the following roles:

    \table
        \header
            \li Role
            \li Type
            \li Description
        \row
            \li category
            \li \l Category
            \li Category object for the current item.
      \row
            \li parentCategory
            \li \l Category
            \li Parent category object for the current item.
                If there is no parent, null is returned.
    \endtable

    The following example displays a flat list of all available categories:

    \snippet declarative/places.qml QtQuick import
    \snippet declarative/maps.qml QtLocation import
    \codeline
    \snippet declarative/places.qml CategoryView

    To access the hierarchical category model it is necessary to use a \l DelegateModel to access
    the child items.
*/

/*!
    \qmlproperty Plugin CategoryModel::plugin

    This property holds the provider \l Plugin used by this model.
*/

/*!
    \qmlproperty bool CategoryModel::hierarchical

    This property holds whether the model provides a hierarchical tree of categories or a flat
    list.  The default is true.
*/

/*!
    \qmlmethod string QtLocation::CategoryModel::data(ModelIndex index, int role)
    \internal

    This method retrieves the model's data per \a index and \a role.
*/

/*!
    \qmlmethod string QtLocation::CategoryModel::errorString() const

    This read-only property holds the textual presentation of the latest category model error.
    If no error has occurred, an empty string is returned.

    An empty string may also be returned if an error occurred which has no associated
    textual representation.
*/

/*!
    \qmlmethod void QtLocation::CategoryModel::update()
    \internal

    Updates the model.

    \note The CategoryModel auto updates automatically when needed. Calling this method explicitly is normally not necessary.
*/

/*!
    \internal
    \enum QDeclarativeSupportedCategoriesModel::Roles
*/

QDeclarativeSupportedCategoriesModel::QDeclarativeSupportedCategoriesModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

QDeclarativeSupportedCategoriesModel::~QDeclarativeSupportedCategoriesModel()
{
    qDeleteAll(m_categoriesTree);
}

/*!
    \internal
*/
// From QQmlParserStatus
void QDeclarativeSupportedCategoriesModel::componentComplete()
{
    m_complete = true;
    if (m_plugin) // do not try to load or change status when trying to update in componentComplete() if the plugin hasn't been set yet even once.
        update();
}

/*!
    \internal
*/
int QDeclarativeSupportedCategoriesModel::rowCount(const QModelIndex &parent) const
{
    if (m_categoriesTree.keys().isEmpty())
        return 0;

    PlaceCategoryNode *node = static_cast<PlaceCategoryNode *>(parent.internalPointer());
    if (!node)
        node = m_categoriesTree.value(QString());
    else if (m_categoriesTree.keys(node).isEmpty())
        return 0;

    return node->childIds.count();
}

/*!
    \internal
*/
int QDeclarativeSupportedCategoriesModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 1;
}

/*!
    \internal
*/
QModelIndex QDeclarativeSupportedCategoriesModel::index(int row, int column, const QModelIndex &parent) const
{
    if (column != 0 || row < 0)
        return QModelIndex();

    PlaceCategoryNode *node = static_cast<PlaceCategoryNode *>(parent.internalPointer());

    if (!node)
        node = m_categoriesTree.value(QString());
    else if (m_categoriesTree.keys(node).isEmpty()) //return root index if parent is non-existent
        return QModelIndex();

    if (row > node->childIds.count())
        return QModelIndex();

    QString id = node->childIds.at(row);
    Q_ASSERT(m_categoriesTree.contains(id));

    return createIndex(row, 0, m_categoriesTree.value(id));
}

/*!
    \internal
*/
QModelIndex QDeclarativeSupportedCategoriesModel::parent(const QModelIndex &child) const
{
    PlaceCategoryNode *childNode = static_cast<PlaceCategoryNode *>(child.internalPointer());
    if (m_categoriesTree.keys(childNode).isEmpty())
        return QModelIndex();

    return index(childNode->parentId);
}

/*!
    \internal
*/
QVariant QDeclarativeSupportedCategoriesModel::data(const QModelIndex &index, int role) const
{
    PlaceCategoryNode *node = static_cast<PlaceCategoryNode *>(index.internalPointer());
    if (!node)
        node = m_categoriesTree.value(QString());
    else if (m_categoriesTree.keys(node).isEmpty())
        return QVariant();

   QDeclarativeCategory *category = node->declCategory.data();

    switch (role) {
    case Qt::DisplayRole:
        return category->name();
    case CategoryRole:
        return QVariant::fromValue(category);
    case ParentCategoryRole: {
        if (!m_categoriesTree.keys().contains(node->parentId))
            return QVariant();
        else
            return QVariant::fromValue(m_categoriesTree.value(node->parentId)->declCategory.data());
    }
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> QDeclarativeSupportedCategoriesModel::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractItemModel::roleNames();
    roles.insert(CategoryRole, "category");
    roles.insert(ParentCategoryRole, "parentCategory");
    return roles;
}

/*!
    \internal
*/
void QDeclarativeSupportedCategoriesModel::setPlugin(QDeclarativeGeoServiceProvider *plugin)
{
    if (m_plugin == plugin)
        return;

    //disconnect the manager of the old plugin if we have one
    if (m_plugin) {
        disconnect(m_plugin, nullptr, this, nullptr);
        QGeoServiceProvider *serviceProvider = m_plugin->sharedGeoServiceProvider();
        if (serviceProvider) {
            QPlaceManager *placeManager = serviceProvider->placeManager();
            if (placeManager) {
                disconnect(placeManager, &QPlaceManager::categoryAdded,
                           this, &QDeclarativeSupportedCategoriesModel::addedCategory);
                disconnect(placeManager, &QPlaceManager::categoryUpdated,
                           this, &QDeclarativeSupportedCategoriesModel::updatedCategory);
                disconnect(placeManager, &QPlaceManager::categoryRemoved,
                           this, &QDeclarativeSupportedCategoriesModel::removedCategory);
                disconnect(placeManager, &QPlaceManager::dataChanged,
                           this, &QDeclarativeSupportedCategoriesModel::emitDataChanged);
            }
        }
    }

    m_plugin = plugin;

    // handle plugin attached changes -> update categories
    if (m_plugin) {
        if (m_plugin->isAttached()) {
            connectNotificationSignals();
            update();
        } else {
            connect(m_plugin, &QDeclarativeGeoServiceProvider::attached,
                    this, &QDeclarativeSupportedCategoriesModel::update);
            connect(m_plugin, &QDeclarativeGeoServiceProvider::attached,
                    this, &QDeclarativeSupportedCategoriesModel::connectNotificationSignals);
        }
    }

    if (m_complete)
        emit pluginChanged();
}

/*!
    \internal
*/
QDeclarativeGeoServiceProvider *QDeclarativeSupportedCategoriesModel::plugin() const
{
    return m_plugin;
}

/*!
    \internal
*/
void QDeclarativeSupportedCategoriesModel::setHierarchical(bool hierarchical)
{
    if (m_hierarchical == hierarchical)
        return;

    m_hierarchical = hierarchical;
    emit hierarchicalChanged();

    updateLayout();
}

/*!
    \internal
*/
bool QDeclarativeSupportedCategoriesModel::hierarchical() const
{
    return m_hierarchical;
}

/*!
    \internal
*/
void QDeclarativeSupportedCategoriesModel::replyFinished()
{
    if (!m_response)
        return;

    m_response->deleteLater();

    if (m_response->error() == QPlaceReply::NoError) {
        m_errorString.clear();

        m_response = nullptr;

        updateLayout();
        setStatus(QDeclarativeSupportedCategoriesModel::Ready);
    } else {
        const QString errorString = m_response->errorString();

        m_response = nullptr;

        setStatus(Error, errorString);
    }
}

/*!
    \internal
*/
void QDeclarativeSupportedCategoriesModel::addedCategory(const QPlaceCategory &category,
                                                         const QString &parentId)
{
    if (m_response)
        return;

    if (!m_categoriesTree.contains(parentId))
        return;

    if (category.categoryId().isEmpty())
        return;

    PlaceCategoryNode *parentNode = m_categoriesTree.value(parentId);
    if (!parentNode)
        return;

    int rowToBeAdded = rowToAddChild(parentNode, category);
    QModelIndex parentIndex = index(parentId);
    beginInsertRows(parentIndex, rowToBeAdded, rowToBeAdded);
    PlaceCategoryNode *categoryNode = new PlaceCategoryNode;
    categoryNode->parentId = parentId;
    categoryNode->declCategory = QSharedPointer<QDeclarativeCategory>(new QDeclarativeCategory(category, m_plugin, this));

    m_categoriesTree.insert(category.categoryId(), categoryNode);
    parentNode->childIds.insert(rowToBeAdded,category.categoryId());
    endInsertRows();

    //this is a workaround to deal with the fact that the hasModelChildren field of DelegateModel
    //does not get updated when a child is added to a model
    beginResetModel();
    endResetModel();
}

/*!
    \internal
*/
void QDeclarativeSupportedCategoriesModel::updatedCategory(const QPlaceCategory &category,
                                                           const QString &parentId)
{
    if (m_response)
        return;

    QString categoryId = category.categoryId();

    if (!m_categoriesTree.contains(parentId))
        return;

    if (category.categoryId().isEmpty() || !m_categoriesTree.contains(categoryId))
        return;

    PlaceCategoryNode *newParentNode = m_categoriesTree.value(parentId);
    if (!newParentNode)
        return;

    PlaceCategoryNode *categoryNode = m_categoriesTree.value(categoryId);
    if (!categoryNode)
        return;

    categoryNode->declCategory->setCategory(category);

    if (categoryNode->parentId == parentId) { //reparenting to same parent
        QModelIndex parentIndex = index(parentId);
        int rowToBeAdded = rowToAddChild(newParentNode, category);
        int oldRow = newParentNode->childIds.indexOf(categoryId);

        //check if we are changing the position of the category
        if (qAbs(rowToBeAdded - newParentNode->childIds.indexOf(categoryId)) > 1) {
            //if the position has changed we are moving rows
            beginMoveRows(parentIndex, oldRow, oldRow,
                          parentIndex, rowToBeAdded);

            newParentNode->childIds.removeAll(categoryId);
            newParentNode->childIds.insert(rowToBeAdded, categoryId);
            endMoveRows();
        } else {// if the position has not changed we modifying an existing row
            QModelIndex categoryIndex = index(categoryId);
            emit dataChanged(categoryIndex, categoryIndex);
        }
    } else { //reparenting to different parents
        QPlaceCategory oldCategory = categoryNode->declCategory->category();
        PlaceCategoryNode *oldParentNode = m_categoriesTree.value(categoryNode->parentId);
        if (!oldParentNode)
            return;
        QModelIndex oldParentIndex = index(categoryNode->parentId);
        QModelIndex newParentIndex = index(parentId);

        int rowToBeAdded = rowToAddChild(newParentNode, category);
        beginMoveRows(oldParentIndex, oldParentNode->childIds.indexOf(categoryId),
                      oldParentNode->childIds.indexOf(categoryId), newParentIndex, rowToBeAdded);
        oldParentNode->childIds.removeAll(oldCategory.categoryId());
        newParentNode->childIds.insert(rowToBeAdded, categoryId);
        categoryNode->parentId = parentId;
        endMoveRows();

        //this is a workaround to deal with the fact that the hasModelChildren field of DelegateModel
        //does not get updated when an index is updated to contain children
        beginResetModel();
        endResetModel();
    }
}

/*!
    \internal
*/
void QDeclarativeSupportedCategoriesModel::removedCategory(const QString &categoryId, const QString &parentId)
{
    if (m_response)
        return;

    if (!m_categoriesTree.contains(categoryId) || !m_categoriesTree.contains(parentId))
        return;

    QModelIndex parentIndex = index(parentId);
    QModelIndex categoryIndex = index(categoryId);

    beginRemoveRows(parentIndex, categoryIndex.row(), categoryIndex.row());
    PlaceCategoryNode *parentNode = m_categoriesTree.value(parentId);
    parentNode->childIds.removeAll(categoryId);
    delete m_categoriesTree.take(categoryId);
    endRemoveRows();
}

/*!
    \internal
*/
void QDeclarativeSupportedCategoriesModel::connectNotificationSignals()
{
    if (!m_plugin)
        return;

    QGeoServiceProvider *serviceProvider = m_plugin->sharedGeoServiceProvider();
    if (!serviceProvider || serviceProvider->error() != QGeoServiceProvider::NoError)
        return;

    QPlaceManager *placeManager = serviceProvider->placeManager();
    if (!placeManager)
        return;

    // listen for any category notifications so that we can reupdate the categories
    // model.
    connect(placeManager, &QPlaceManager::categoryAdded,
            this, &QDeclarativeSupportedCategoriesModel::addedCategory);
    connect(placeManager, &QPlaceManager::categoryUpdated,
            this, &QDeclarativeSupportedCategoriesModel::updatedCategory);
    connect(placeManager, &QPlaceManager::categoryRemoved,
            this, &QDeclarativeSupportedCategoriesModel::removedCategory);
    connect(placeManager, &QPlaceManager::dataChanged,
            this, &QDeclarativeSupportedCategoriesModel::emitDataChanged);
}

/*!
    \internal
*/
void QDeclarativeSupportedCategoriesModel::update()
{
    if (!m_complete)
        return;

    if (m_response)
        return;

    setStatus(Loading);

    if (!m_plugin) {
        updateLayout();
        setStatus(Error, QCoreApplication::translate(CONTEXT_NAME, PLUGIN_PROPERTY_NOT_SET));
        return;
    }

    QGeoServiceProvider *serviceProvider = m_plugin->sharedGeoServiceProvider();
    if (!serviceProvider || serviceProvider->error() != QGeoServiceProvider::NoError) {
        updateLayout();
        setStatus(Error, QCoreApplication::translate(CONTEXT_NAME, PLUGIN_PROVIDER_ERROR)
                         .arg(m_plugin->name()));
        return;
    }

    QPlaceManager *placeManager = serviceProvider->placeManager();
    if (!placeManager) {
        updateLayout();
        setStatus(Error, QCoreApplication::translate(CONTEXT_NAME, PLUGIN_ERROR)
                         .arg(m_plugin->name()).arg(serviceProvider->errorString()));
        return;
    }

    m_response = placeManager->initializeCategories();
    if (m_response) {
        connect(m_response, &QPlaceReply::finished,
                this, &QDeclarativeSupportedCategoriesModel::replyFinished);
    } else {
        updateLayout();
        setStatus(Error, QCoreApplication::translate(CONTEXT_NAME,
                    CATEGORIES_NOT_INITIALIZED));
    }
}

/*!
    \internal
*/
void QDeclarativeSupportedCategoriesModel::updateLayout()
{
    beginResetModel();
    qDeleteAll(m_categoriesTree);
    m_categoriesTree.clear();

    if (m_plugin) {
        QGeoServiceProvider *serviceProvider = m_plugin->sharedGeoServiceProvider();
        if (serviceProvider && serviceProvider->error() == QGeoServiceProvider::NoError) {
            QPlaceManager *placeManager = serviceProvider->placeManager();
            if (placeManager) {
                PlaceCategoryNode *node = new PlaceCategoryNode;
                node->childIds = populateCategories(placeManager, QPlaceCategory());
                m_categoriesTree.insert(QString(), node);
                node->declCategory = QSharedPointer<QDeclarativeCategory>
                    (new QDeclarativeCategory(QPlaceCategory(), m_plugin, this));
            }
        }
    }

    endResetModel();
}

QString QDeclarativeSupportedCategoriesModel::errorString() const
{
    return m_errorString;
}

/*!
    \qmlproperty enumeration CategoryModel::status

    This property holds the status of the model.  It can be one of:

    \table
        \row
            \li CategoryModel.Null
            \li No category fetch query has been executed.  The model is empty.
        \row
            \li CategoryModel.Ready
            \li No error occurred during the last operation, further operations may be performed on
               the model.
        \row
            \li CategoryModel.Loading
            \li The model is being updated, no other operations may be performed until complete.
        \row
            \li CategoryModel.Error
            \li An error occurred during the last operation, further operations can still be
               performed on the model.
    \endtable
*/
void QDeclarativeSupportedCategoriesModel::setStatus(Status status, const QString &errorString)
{
    Status originalStatus = m_status;
    m_status = status;
    m_errorString = errorString;

    if (originalStatus != m_status)
        emit statusChanged();
}

QDeclarativeSupportedCategoriesModel::Status QDeclarativeSupportedCategoriesModel::status() const
{
    return m_status;
}

/*!
    \internal
*/
QStringList QDeclarativeSupportedCategoriesModel::populateCategories(QPlaceManager *manager, const QPlaceCategory &parent)
{
    Q_ASSERT(manager);

    QStringList childIds;

    const auto byName = [](const QPlaceCategory &lhs, const QPlaceCategory &rhs) {
        return lhs.name() < rhs.name();
    };

    auto categories = manager->childCategories(parent.categoryId());
    std::sort(categories.begin(), categories.end(), byName);

    for (const auto &category : std::as_const(categories)) {
        auto node = new PlaceCategoryNode;
        node->parentId = parent.categoryId();
        node->declCategory = QSharedPointer<QDeclarativeCategory>(new QDeclarativeCategory(category, m_plugin ,this));

        if (m_hierarchical)
            node->childIds = populateCategories(manager, category);

        m_categoriesTree.insert(node->declCategory->categoryId(), node);
        childIds.append(category.categoryId());

        if (!m_hierarchical) {
            childIds.append(populateCategories(manager,node->declCategory->category()));
        }
    }
    return childIds;
}

/*!
    \internal
*/
QModelIndex QDeclarativeSupportedCategoriesModel::index(const QString &categoryId) const
{
    if (categoryId.isEmpty())
        return QModelIndex();

    if (!m_categoriesTree.contains(categoryId))
        return QModelIndex();

    PlaceCategoryNode *categoryNode = m_categoriesTree.value(categoryId);
    if (!categoryNode)
        return QModelIndex();

    QString parentCategoryId = categoryNode->parentId;

    PlaceCategoryNode *parentNode = m_categoriesTree.value(parentCategoryId);

    return createIndex(parentNode->childIds.indexOf(categoryId), 0, categoryNode);
}

/*!
    \internal
*/
int QDeclarativeSupportedCategoriesModel::rowToAddChild(PlaceCategoryNode *node, const QPlaceCategory &category)
{
    Q_ASSERT(node);
    for (qsizetype i = 0; i < node->childIds.count(); ++i) {
        if (category.name() < m_categoriesTree.value(node->childIds.at(i))->declCategory->name())
            return i;
    }
    return node->childIds.count();
}

/*!
    \qmlsignal CategoryModel::dataChanged()

   This signal is emitted when significant changes have been made to the underlying datastore.

   Applications should act on this signal at their own discretion.  The data
   provided by the model could be out of date and so the model should be reupdated
   sometime, however an immediate reupdate may be disconcerting to users if the categories
   change without any action on their part.

   The corresponding handler is \c onDataChanged.
*/

QT_END_NAMESPACE
