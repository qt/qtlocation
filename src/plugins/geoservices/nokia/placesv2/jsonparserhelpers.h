/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#ifndef JSONPARSERHELPERS_H
#define JSONPARSERHELPERS_H

#include <QtLocation/QPlaceContent>

QT_BEGIN_NAMESPACE

class QJsonArray;
class QJsonObject;
class QGeoCoordinate;
class QPlaceContactDetail;
class QPlaceImage;
class QPlaceReview;
class QPlaceEditorial;
class QPlaceCategory;
class QPlaceContentRequest;
class QPlaceManagerEngineNokiaV2;

QGeoCoordinate parseCoordinate(const QJsonArray &coordinateArray);
QPlaceSupplier parseSupplier(const QJsonObject &supplierObject,
                             const QPlaceManagerEngineNokiaV2 *engine);
QPlaceCategory parseCategory(const QJsonObject &categoryObject,
                             const QPlaceManagerEngineNokiaV2 *engine);
QList<QPlaceCategory> parseCategories(const QJsonArray &categoryArray,
                             const QPlaceManagerEngineNokiaV2 *engine);
QList<QPlaceContactDetail> parseContactDetails(const QJsonArray &contacts);

QPlaceImage parseImage(const QJsonObject &imageObject,
                       const QPlaceManagerEngineNokiaV2 *engine);
QPlaceReview parseReview(const QJsonObject &reviewObject,
                         const QPlaceManagerEngineNokiaV2 *engine);
QPlaceEditorial parseEditorial(const QJsonObject &editorialObject,
                               const QPlaceManagerEngineNokiaV2 *engine);

void parseCollection(QPlaceContent::Type type, const QJsonObject &object,
                     QPlaceContent::Collection *collection, int *totalCount,
                     QPlaceContentRequest *previous, QPlaceContentRequest *next,
                     const QPlaceManagerEngineNokiaV2 *engine);

QT_END_NAMESPACE

#endif // JSONPARSERHELPERS_H
