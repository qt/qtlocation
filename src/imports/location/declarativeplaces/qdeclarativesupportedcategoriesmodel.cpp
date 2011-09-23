/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qdeclarativesupportedcategoriesmodel_p.h"
#include "qgeoserviceprovider.h"

#include <QtDeclarative/QDeclarativeInfo>

#include <qplacemanager.h>

QT_USE_NAMESPACE

/*!
    \qmlclass SupportedCategoryModel QDeclarativeSupportedCategoriesModel
    \brief The SupportedCategoryModel element provides access to supported categories list.
    \inherits QAbstractListModel
    \ingroup qml-places
    \since QtLocation 5.0

    SupportedCategoryModel provides a model of categories from the supported categories list.  The
    model can provide both a flat list of categories or a hierarchical tree representing category
    grouping.  This can be controlled by the \l hierarchy property.

    The model provides a single data role, \c category (\l Category).

    To use SupportedCategoryModel user need to create it in qml file and connect it to some view
    \code
    import QtQuick 2.0
    import Qt.location 5.0

    SupportedCategoriesModel {
        id: categoriesModel
        hierarchical: false
    }

    ListView {
        id: categoriesList
        model: categoriesModel
        delegate: Text {
            text: '<b>name:</b> ' + category.name }
        }
    }
    \endcode

    To access the hierarchical category model it is necessary to use a VisualDataModel to access
    the child items.

    \sa SearchResultModel, SuggestionModel, {QPlaceManager}
*/

/*!
    \qmlproperty Plugin SupportedCategoriesModel::plugin

    this property holds the provider Plugin used by this model.
*/

/*!
    \qmlproperty bool SupportedCategoriesModel::hierarchical

    This property holds whether the model provides a hierarchical tree of categories or a flat
    list.  The default is true.
*/

PlaceCategoryTree::PlaceCategoryTree()
{
}

PlaceCategoryTree::~PlaceCategoryTree()
{
}

QDeclarativeSupportedCategoriesModel::QDeclarativeSupportedCategoriesModel(QObject *parent)
:   QAbstractItemModel(parent), m_plugin(0), m_hierarchical(true), m_complete(false)
{
    QHash<int, QByteArray> roleNames;
    roleNames = QAbstractItemModel::roleNames();
    roleNames.insert(CategoryRole, "category");
    setRoleNames(roleNames);
}

QDeclarativeSupportedCategoriesModel::~QDeclarativeSupportedCategoriesModel()
{
}

// From QDeclarativeParserStatus
void QDeclarativeSupportedCategoriesModel::componentComplete()
{
    m_complete = true;
}

int QDeclarativeSupportedCategoriesModel::rowCount(const QModelIndex& parent) const
{
    PlaceCategoryTree tree = findCategoryTreeByCategory(static_cast<QDeclarativeCategory *>(parent.internalPointer()), m_categoryTree);
    return tree.subCategories.count();
}

int QDeclarativeSupportedCategoriesModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return 1;
}

QModelIndex QDeclarativeSupportedCategoriesModel::index(int row, int column, const QModelIndex &parent) const
{
    if (column != 0 || row < 0)
        return QModelIndex();

    PlaceCategoryTree tree = findCategoryTreeByCategory(static_cast<QDeclarativeCategory *>(parent.internalPointer()), m_categoryTree);

    if (row > tree.subCategories.count())
        return QModelIndex();

    QMap<QString, QDeclarativeCategory *> sortedCategories;
    QHashIterator<QString, PlaceCategoryTree> it(tree.subCategories);
    while (it.hasNext()) {
        it.next();
        sortedCategories.insert(it.value().category->name(), it.value().category.data());
    }

    return createIndex(row, 0, sortedCategories.values().at(row));
}

QModelIndex QDeclarativeSupportedCategoriesModel::parent(const QModelIndex &child) const
{
    QDeclarativeCategory *parentCategory = findParentCategoryByCategory(static_cast<QDeclarativeCategory *>(child.internalPointer()), m_categoryTree);
    if (!parentCategory)
        return QModelIndex();

    QDeclarativeCategory *grandParentCategory = findParentCategoryByCategory(parentCategory, m_categoryTree);
    PlaceCategoryTree tree = findCategoryTreeByCategory(grandParentCategory, m_categoryTree);

    QMap<QString, QDeclarativeCategory *> sortedCategories;
    QHashIterator<QString, PlaceCategoryTree> it(tree.subCategories);
    while (it.hasNext()) {
        it.next();
        sortedCategories.insert(it.value().category->name(), it.value().category.data());
    }

    return createIndex(sortedCategories.values().indexOf(parentCategory), 0, parentCategory);
}

QVariant QDeclarativeSupportedCategoriesModel::data(const QModelIndex &index, int role) const
{
    PlaceCategoryTree tree = findCategoryTreeByCategory(static_cast<QDeclarativeCategory *>(index.internalPointer()), m_categoryTree);
    QDeclarativeCategory *category = tree.category.data();

    if (!category)
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
        return category->name();
    case CategoryRole:
        return QVariant::fromValue(category);
    default:
        return QVariant();
    }
}

void QDeclarativeSupportedCategoriesModel::setPlugin(QDeclarativeGeoServiceProvider *plugin)
{
    if (m_plugin == plugin)
        return;

    reset(); // reset the model
    m_plugin = plugin;
    if (m_complete)
        emit pluginChanged();
    QGeoServiceProvider *serviceProvider = m_plugin->sharedGeoServiceProvider();
    QPlaceManager *placeManager = serviceProvider->placeManager();
    if (!placeManager || serviceProvider->error() != QGeoServiceProvider::NoError) {
        qmlInfo(this) << tr("Warning: Plugin does not support places.");
        return;
    }

    m_categoryTree.subCategories = populatedCategories(placeManager);

    m_response = placeManager->initializeCategories();
    if (m_response) {
        connect(m_response, SIGNAL(finished()), this, SLOT(replyFinished()));
        connect(m_response, SIGNAL(error(QPlaceReply::Error,QString)),
                this, SLOT(replyError(QPlaceReply::Error,QString)));

        emit updatingChanged();
    }
}

QDeclarativeGeoServiceProvider* QDeclarativeSupportedCategoriesModel::plugin() const
{
    return m_plugin;
}

void QDeclarativeSupportedCategoriesModel::setHierarchical(bool hierarchical)
{
    if (m_hierarchical == hierarchical)
        return;

    m_hierarchical = hierarchical;
    emit hierarchicalChanged();

    updateCategories();
}

bool QDeclarativeSupportedCategoriesModel::hierarchical() const
{
    return m_hierarchical;
}

bool QDeclarativeSupportedCategoriesModel::updating() const
{
    return m_response;
}

void QDeclarativeSupportedCategoriesModel::replyFinished()
{
    if (!m_response)
        return;

    m_response->deleteLater();
    m_response = 0;

    updateCategories();

    emit updatingChanged();
}

void QDeclarativeSupportedCategoriesModel::replyError(QPlaceReply::Error error,
                                                 const QString &errorString)
{
    Q_UNUSED(error);
    Q_UNUSED(errorString);

    m_response->deleteLater();
    m_response = 0;
    emit updatingChanged();
}

void QDeclarativeSupportedCategoriesModel::updateCategories()
{
    if (!m_plugin)
        return;

    QGeoServiceProvider *serviceProvider = m_plugin->sharedGeoServiceProvider();
    if (!serviceProvider)
        return;

    QPlaceManager *placeManager = serviceProvider->placeManager();
    if (!placeManager) {
        qmlInfo(this) << tr("Places not supported by %1 Plugin.").arg(m_plugin->name());
        return;
    }

    beginResetModel();
    m_categoryTree.subCategories = populatedCategories(placeManager);
    endResetModel();
}

QHash<QString, PlaceCategoryTree> QDeclarativeSupportedCategoriesModel::populatedCategories(QPlaceManager *manager, const QPlaceCategory &parent)
{
    QHash<QString, PlaceCategoryTree> declarativeTree;

    foreach (const QPlaceCategory &category, manager->categories(parent)) {
        PlaceCategoryTree dt;
        dt.category = QSharedPointer<QDeclarativeCategory>(new QDeclarativeCategory(category, this));
        if (m_hierarchical)
            dt.subCategories = populatedCategories(manager, category);
        declarativeTree.insert(category.categoryId(), dt);

        if (!m_hierarchical) {
            QHash<QString, PlaceCategoryTree> sub = populatedCategories(manager, category);
            QHashIterator<QString, PlaceCategoryTree> it(sub);
            while (it.hasNext()) {
                it.next();
                dt.category = QSharedPointer<QDeclarativeCategory>(it.value().category);
                Q_ASSERT(it.value().subCategories.isEmpty());
                declarativeTree.insert(it.key(), dt);
            }
        }
    }

    return declarativeTree;
}

PlaceCategoryTree QDeclarativeSupportedCategoriesModel::findCategoryTreeByCategory(QDeclarativeCategory *category, const PlaceCategoryTree &tree) const
{
    if (tree.category == category)
        return tree;

    QHashIterator<QString, PlaceCategoryTree> it(tree.subCategories);
    while (it.hasNext()) {
        it.next();

        if (it.value().category == category)
            return it.value();

        PlaceCategoryTree t = findCategoryTreeByCategory(category, it.value());
        if (t.category == category)
            return t;
    }

    return PlaceCategoryTree();
}

QDeclarativeCategory *QDeclarativeSupportedCategoriesModel::findParentCategoryByCategory(QDeclarativeCategory *category, const PlaceCategoryTree &tree) const
{
    if (tree.category == category)
        return 0;

    QHashIterator<QString, PlaceCategoryTree> it(tree.subCategories);
    while (it.hasNext()) {
        it.next();

        if (it.value().category == category)
            return tree.category.data();

        QDeclarativeCategory *p = findParentCategoryByCategory(category, it.value());
        if (p)
            return p;
    }

    return 0;
}
