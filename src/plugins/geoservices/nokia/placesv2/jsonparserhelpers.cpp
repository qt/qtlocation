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

#include "jsonparserhelpers.h"
#include "../qplacemanagerengine_nokiav2.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDateTime>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QVariantMap>
#include <QtLocation/QGeoCoordinate>
#include <QtLocation/QPlaceImage>
#include <QtLocation/QPlaceReview>
#include <QtLocation/QPlaceEditorial>
#include <QtLocation/QPlaceUser>
#include <QtLocation/QPlaceContactDetail>
#include <QtLocation/QPlaceCategory>

QT_BEGIN_NAMESPACE

QGeoCoordinate parseCoordinate(const QJsonArray &coordinateArray)
{
    return QGeoCoordinate(coordinateArray.at(0).toDouble(), coordinateArray.at(1).toDouble());
}

QPlaceSupplier parseSupplier(const QJsonObject &supplierObject)
{
    QPlaceSupplier supplier;
    supplier.setName(supplierObject.value(QLatin1String("title")).toString());
    supplier.setUrl(supplierObject.value(QLatin1String("href")).toString());

    QVariantMap parameters;
    parameters.insert(QPlaceIcon::SingleUrl,
                      QUrl(supplierObject.value(QLatin1String("icon")).toString()));
    QPlaceIcon icon;
    icon.setParameters(parameters);
    supplier.setIcon(icon);

    return supplier;
}

QPlaceCategory parseCategory(const QJsonObject &categoryObject,
                             const QPlaceManagerEngineNokiaV2 *engine)
{
    Q_ASSERT(engine);

    QPlaceCategory category;

    category.setName(categoryObject.value(QLatin1String("title")).toString());

    const QUrl href(categoryObject.value(QLatin1String("href")).toString());
    const QString hrefPath(href.path());
    category.setCategoryId(hrefPath.mid(hrefPath.lastIndexOf(QLatin1Char('/')) + 1));

    QString iconPath = engine->iconPath(
                            categoryObject.value(QLatin1String("icon")).toString());
    QVariantMap parameters;
    parameters.insert(QPlaceIcon::SingleUrl,
                      QUrl(iconPath));
    QPlaceIcon icon;
    icon.setParameters(parameters);
    category.setIcon(icon);

    return category;
}

QList<QPlaceCategory> parseCategories(const QJsonArray &categoryArray,
                                     const QPlaceManagerEngineNokiaV2 *engine)
{
    Q_ASSERT(engine);

    QList<QPlaceCategory> categoryList;
    for (int i = 0; i < categoryArray.count(); ++i)
        categoryList.append(parseCategory(categoryArray.at(i).toObject(),
                                          engine));

    return categoryList;
}

QList<QPlaceContactDetail> parseContactDetails(const QJsonArray &contacts)
{
    QList<QPlaceContactDetail> contactDetails;

    for (int i = 0; i < contacts.count(); ++i) {
        QJsonObject contact = contacts.at(i).toObject();

        QPlaceContactDetail detail;
        detail.setLabel(contact.value(QLatin1String("label")).toString());
        detail.setValue(contact.value(QLatin1String("value")).toString());

        contactDetails.append(detail);
    }

    return contactDetails;
}

QPlaceImage parseImage(const QJsonObject &imageObject)
{
    QPlaceImage image;

    image.setAttribution(imageObject.value(QLatin1String("attribution")).toString());
    image.setUrl(imageObject.value(QLatin1String("src")).toString());
    image.setSupplier(parseSupplier(imageObject.value(QLatin1String("supplier")).toObject()));

    return image;
}

QPlaceReview parseReview(const QJsonObject &reviewObject)
{
    QPlaceReview review;

    review.setDateTime(QDateTime::fromString(reviewObject.value(QLatin1String("date")).toString()));

    if (reviewObject.contains(QLatin1String("title")))
        review.setTitle(reviewObject.value(QLatin1String("title")).toString());

    if (reviewObject.contains(QLatin1String("rating")))
        review.setRating(reviewObject.value(QLatin1String("rating")).toDouble());

    review.setText(reviewObject.value(QLatin1String("description")).toString());

    QJsonObject userObject = reviewObject.value(QLatin1String("user")).toObject();

    QPlaceUser user;
    user.setUserId(userObject.value(QLatin1String("id")).toString());
    user.setName(userObject.value(QLatin1String("title")).toString());
    review.setUser(user);

    review.setAttribution(reviewObject.value(QLatin1String("attribution")).toString());

    review.setLanguage(reviewObject.value(QLatin1String("language")).toString());

    review.setSupplier(parseSupplier(reviewObject.value(QLatin1String("supplier")).toObject()));

    //if (reviewObject.contains(QLatin1String("via"))) {
    //    QJsonObject viaObject = reviewObject.value(QLatin1String("via")).toObject();
    //}

    return review;
}

QPlaceEditorial parseEditorial(const QJsonObject &editorialObject)
{
    QPlaceEditorial editorial;

    editorial.setAttribution(editorialObject.value(QLatin1String("attribution")).toString());

    //if (editorialObject.contains(QLatin1String("via"))) {
    //    QJsonObject viaObject = editorialObject.value(QLatin1String("via")).toObject();
    //}

    editorial.setSupplier(parseSupplier(editorialObject.value(QLatin1String("supplier")).toObject()));
    editorial.setLanguage(editorialObject.value(QLatin1String("language")).toString());
    editorial.setText(editorialObject.value(QLatin1String("description")).toString());

    return editorial;
}

void parseCollection(QPlaceContent::Type type, const QJsonObject &object,
                     QPlaceContent::Collection *collection, int *totalCount)
{
    if (totalCount)
        *totalCount = object.value(QLatin1String("available")).toDouble();

    int offset = 0;
    if (object.contains(QLatin1String("offset")))
        offset = object.value(QLatin1String("offset")).toDouble();

    if (collection) {
        QJsonArray items = object.value(QLatin1String("items")).toArray();
        for (int i = 0; i < items.count(); ++i) {
            QJsonObject itemObject = items.at(i).toObject();

            switch (type) {
            case QPlaceContent::ImageType:
                collection->insert(offset + i, parseImage(itemObject));
                break;
            case QPlaceContent::ReviewType:
                collection->insert(offset + i, parseReview(itemObject));
                break;
            case QPlaceContent::EditorialType:
                collection->insert(offset + i, parseEditorial(itemObject));
                break;
            case QPlaceContent::NoType:
                break;
            }
        }
    }
}

QT_END_NAMESPACE
