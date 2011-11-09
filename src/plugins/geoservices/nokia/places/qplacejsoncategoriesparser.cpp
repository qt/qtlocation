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
** This file is part of the Ovi services plugin for the Maps and
** Navigation API.  The use of these services, whether by use of the
** plugin or by other means, is governed by the terms and conditions
** described by the file OVI_SERVICES_TERMS_AND_CONDITIONS.txt in
** this package, located in the directory containing the Ovi services
** plugin source code.
**
****************************************************************************/

#include "qplacejsoncategoriesparser.h"

#include <QJSEngine>
#include <QJSValue>
#include <QJSValueIterator>

#if defined(QT_PLACES_LOGGING)
    #include <QDebug>
#endif

static const char *place_categories_element = "categories";
static const char *place_category_element = "category";
static const char *place_category_name_element = "displayName";
static const char *place_category_id_element = "name";

static const char *place_group_element = "group";
static const char *place_groupingcategory_element = "groupingCategory";

QT_USE_NAMESPACE

QPlaceJSonCategoriesParser::QPlaceJSonCategoriesParser(QObject *parent) :
    QPlaceJSonParser(parent)
{
}

QPlaceJSonCategoriesParser::~QPlaceJSonCategoriesParser()
{
}

QPlaceCategoryTree QPlaceJSonCategoriesParser::resultCategories() const
{
    return m_tree;
}

QList<QPlaceCategory> QPlaceJSonCategoriesParser::resultCategoriesFlat() const
{
    QList<QPlaceCategory> result;
    foreach (const PlaceCategoryNode node, m_tree.values())
        result.append(node.category);
    return result;
}

QList<QPlaceCategory> QPlaceJSonCategoriesParser::parseFlatCategoryList(const QJSValue &categories)
{
    QPlaceCategoryTree tree;
    PlaceCategoryNode rootNode;
    tree.insert(QString(), rootNode);
    processCategories(categories, QString(), &tree);

    QList<QPlaceCategory> result;
    foreach (const PlaceCategoryNode &node, tree.values())
        result.append(node.category);
    return result;
}

void QPlaceJSonCategoriesParser::processJSonData(const QJSValue &sv)
{
    m_tree.clear();
    PlaceCategoryNode rootNode;
    m_tree.insert(QString(), rootNode);

    if (sv.isValid()) {
        QJSValue sv2 = sv.property(place_categories_element);
        if (sv2.isValid()) {
            processCategories(sv2, QString(), &m_tree);
            emit finished(NoError, QString());
        } else {
            emit finished(ParsingError, QString("JSON data are invalid"));
        }
    } else {
        emit finished(ParsingError, QString("JSON data are invalid"));
    }
}

void QPlaceJSonCategoriesParser::processCategories(const QJSValue &categories, const QString &parentId, QPlaceCategoryTree *tree)
{

    Q_ASSERT(tree->contains(parentId));

    QJSValue value = categories.property(place_category_element);
    if (value.isValid()) {
        if (value.isArray()) {
            QJSValueIterator it(value);
            while (it.hasNext()) {
                it.next();
                // array contains count as last element
                if (it.name() != QLatin1String("length")) {
                    PlaceCategoryNode catNode = processCategory(it.value(), parentId);

                    if (!catNode.category.categoryId().isEmpty() &&
                        !tree->contains(catNode.category.categoryId())) {
                        tree->insert(catNode.category.categoryId(), catNode);
                        (*tree)[parentId].childIds.append(catNode.category.categoryId());
                    }
                }
            }
        } else {
            PlaceCategoryNode catNode = processCategory(value, parentId);
            if (!catNode.category.categoryId().isEmpty() &&
                !tree->contains(catNode.category.categoryId())) {
                tree->insert(catNode.category.categoryId(), catNode);
                (*tree)[parentId].childIds.append(catNode.category.categoryId());
            }
        }
    }

    value = categories.property(place_group_element);
    if (value.isValid()) {
        if (value.isArray()) {
            QJSValueIterator it(value);
            while (it.hasNext()) {
                it.next();
                // array contains count as last element
                if (it.name() != QLatin1String("length"))
                    processGroup(it.value(), parentId, tree);
            }
        } else {
            processGroup(value, parentId, tree);
        }
    }
}

PlaceCategoryNode QPlaceJSonCategoriesParser::processCategory(const QJSValue &categoryValue, const QString &parentId)
{
    PlaceCategoryNode categoryNode;
    categoryNode.parentId = parentId;
    QJSValue value = categoryValue.property(place_category_id_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        categoryNode.category.setCategoryId(value.toString());
        value = categoryValue.property(place_category_name_element);
        if (value.isValid() && !value.toString().isEmpty()) {
            categoryNode.category.setName(value.toString());
        }
    }
    return categoryNode;
}

void QPlaceJSonCategoriesParser::processGroup(const QJSValue &group, const QString &parentId, QPlaceCategoryTree *tree)
{
    QJSValue value = group.property(place_groupingcategory_element);
    if (!value.isValid())
        return;

    PlaceCategoryNode catNode = processCategory(value, parentId);
    if (catNode.category.categoryId().isEmpty()) {
        return;
    } else {
        tree->insert(catNode.category.categoryId(), catNode);
        (*tree)[parentId].childIds.append(catNode.category.categoryId());
    }

    processCategories(group, catNode.category.categoryId(), tree);
}
