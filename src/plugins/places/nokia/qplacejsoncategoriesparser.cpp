/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptValueIterator>
#include <qmobilityglobal.h>

#if defined(QT_PLACES_LOGGING)
    #include <QDebug>
#endif

static const char *place_categories_element = "categories";
static const char *place_category_element = "category";
static const char *place_category_name_element = "displayName";
static const char *place_category_id_element = "name";

static const char *place_group_element = "group";
static const char *place_groupingcategory_element = "groupingCategory";

QTM_USE_NAMESPACE

QPlaceJSonCategoriesParser::QPlaceJSonCategoriesParser(QObject *parent) :
    QPlaceJSonParser(parent)
{
}

QPlaceJSonCategoriesParser::~QPlaceJSonCategoriesParser()
{
    allCategories.clear();
}

QList<QPlaceCategory> QPlaceJSonCategoriesParser::resultCategories()
{
    return allCategories;
}

void QPlaceJSonCategoriesParser::processJSonData(const QScriptValue &sv)
{
    allCategories.clear();

    if (sv.isValid()) {
        QScriptValue sv2 = sv.property(place_categories_element);
        if (sv2.isValid()) {
            allCategories = processCategories(sv2);
            emit finished(NoError, QString());
        }
    } else {
        emit finished(ParsingError, QString("JSON data are invalid"));
    }
}

QList<QPlaceCategory> QPlaceJSonCategoriesParser::processCategories(const QScriptValue &categories)
{
    QHash<QString, QPlaceCategory> results;
    QPlaceCategory cat;
    QScriptValue value = categories.property(place_category_element);
    if (value.isValid()) {
        if (value.isArray()) {
            QScriptValueIterator it(value);
            while (it.hasNext()) {
                it.next();
                // array contains count as last element
                if (it.name() != "length") {
                    cat = processCategory(it.value());
                    if (!cat.isEmpty() && !results.contains(cat.categoryId())) {
                        results.insert(cat.categoryId(), cat);
                    }
                }
            }
        } else {
            cat = processCategory(value);
            if (!cat.isEmpty() && !results.contains(cat.categoryId())) {
                results.insert(cat.categoryId(), cat);
            }
        }
    }
    foreach (cat, processGroups(categories)) {
        if (!results.contains(cat.categoryId())) {
            results.insert(cat.categoryId(), cat);
        }
    }

    return results.values();
}

QPlaceCategory QPlaceJSonCategoriesParser::processCategory(const QScriptValue &categoryValue)
{
    QPlaceCategory category;
    QScriptValue value = categoryValue.property(place_category_id_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        category.setCategoryId(value.toString());
        value = categoryValue.property(place_category_name_element);
        if (value.isValid() && !value.toString().isEmpty()) {
            category.setName(value.toString());
        }
    }
    return category;
}

QList<QPlaceCategory> QPlaceJSonCategoriesParser::processGroups(const QScriptValue &categories)
{
    QList<QPlaceCategory> results;
    QScriptValue value = categories.property(place_group_element);
    if (value.isValid()) {
        if (value.isArray()) {
            QScriptValueIterator it(value);
            while (it.hasNext()) {
                it.next();
                // array contains count as last element
                if (it.name() != "length") {
                    results.append(processGroup(it.value()));
                }
            }
        } else {
            results.append(processGroup(value));
        }
    }
    return results;
}

QList<QPlaceCategory> QPlaceJSonCategoriesParser::processGroup(const QScriptValue &group)
{
    QList<QPlaceCategory> results;
    QPlaceCategory parentCategory;

    QScriptValue value = group.property(place_groupingcategory_element);
    if (value.isValid()) {
        parentCategory = processCategory(value);
    }
    if (!parentCategory.isEmpty()) {
        results = processCategories(group);
        results.append(parentCategory);
    }

    return results;
}
