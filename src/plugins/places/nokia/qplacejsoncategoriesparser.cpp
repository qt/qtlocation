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
static const char *place_category_systemname_element = "categorySystemName";
static const char *place_category_id_element = "name";

static const char *place_group_element = "group";
static const char *place_groupingcategory_element = "groupingCategory";

QTM_USE_NAMESPACE

QPlaceJSonCategoriesParser::QPlaceJSonCategoriesParser(QObject *parent) :
    QObject(parent),
    engine(NULL)
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

void QPlaceJSonCategoriesParser::processData(const QString &data)
{
    if (!engine) {
        engine = new QScriptEngine(this);
    }
    allCategories.clear();

    QScriptValue sv = engine->evaluate("(" + data + ")");
    if (sv.isValid()) {
        sv = sv.property(place_categories_element);
        if (sv.isValid()) {
            allCategories = processCategories(sv);
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
