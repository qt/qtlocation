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

#include "qplacejsonreviewparser.h"

#include <QtCore/QUrl>
#include <QtCore/QDateTime>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptValueIterator>
#include <QtLocation/QPlaceIcon>
#include <QtLocation/QPlaceManager>
#include <QtLocation/QPlaceUser>

#include <qplacereview.h>
#include <qplacesupplier.h>
#include "qplacesuppliersrepository.h"

#if defined(QT_PLACES_LIBRARY)
    #include <QDebug>
#endif

static const char *reviews_element = "reviews";
static const char *review_element = "review";
static const char *review_count = "totalNumberOfReviews";
static const char *review_id_element = "a_id";
static const char *review_date_element = "creationDate";
static const char *review_description_element = "description";
static const char *review_minus_element = "minusCount";
static const char *review_originator_element = "originatorUrl";
static const char *review_plus_element = "plusCount";
static const char *review_rating_element = "rating";
static const char *review_title_element = "title";
static const char *review_username_element = "user";
static const char *review_userid_element = "uuid";
static const char *review_vendor_element = "vendor";
static const char *review_vendorname_element = "vendorDisplayName";
static const char *review_vendoricon_element = "vendorIconUrl";

QT_USE_NAMESPACE

QPlaceJSonReviewParser::QPlaceJSonReviewParser(QPlaceManager *manager, QObject *parent) :
    QPlaceJSonParser(parent),
    allReviews(0),m_manager(manager)
{
}

QPlaceJSonReviewParser::~QPlaceJSonReviewParser()
{
}

QList<QPlaceReview> QPlaceJSonReviewParser::results()
{
    return reviews;
}

int QPlaceJSonReviewParser::allReviewsCount()
{
    return allReviews;
}

QPlaceReview QPlaceJSonReviewParser::buildReview(const QScriptValue &review, QPlaceManager *manager)
{
    QPlaceReview newReview;
    QScriptValue value = review.property(review_id_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        newReview.setReviewId(value.toString());
    }
    value = review.property(review_date_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        newReview.setDateTime(QDateTime::fromString(value.toString()));
    }
    value = review.property(review_originator_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        QPlaceSupplier supplier = newReview.supplier();
        supplier.setUrl(value.toString());
        newReview.setSupplier(supplier);
    }
    value = review.property(review_description_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        newReview.setText(value.toString());
    }
    value = review.property(review_title_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        newReview.setTitle(value.toString());
    }
    value = review.property(review_username_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        QPlaceUser user = newReview.user();
        user.setName(value.toString());
        newReview.setUser(user);
    }
    value = review.property(review_userid_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        QPlaceUser user = newReview.user();
        user.setUserId(value.toString());
        newReview.setUser(user);
    }
    QString name, id, icon;
    value = review.property(review_vendorname_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        name = value.toString();
    }
    value = review.property(review_vendor_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        id = value.toString();
    }
    value = review.property(review_vendoricon_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        icon = value.toString();
    }
    if (!name.isEmpty() || !id.isEmpty()) {
        QPlaceSupplier sup;
        sup.setName(name);
        sup.setSupplierId(id);
        if (!icon.toAscii().isEmpty()) {
            QPlaceIcon supplierIcon;
            supplierIcon.setBaseUrl(QUrl::fromEncoded(icon.toAscii()));
            supplierIcon.setManager(manager);
            sup.setIcon(supplierIcon);
        }
        newReview.setSupplier(QPlaceSuppliersRepository::instance()->addSupplier(sup));
    }

    value = review.property(review_rating_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        bool isConverted;
        double number = value.toString().toDouble(&isConverted);
        if (isConverted) {
            newReview.setRating(number);
        }
    }
    return newReview;
}

void QPlaceJSonReviewParser::processJSonData(const QScriptValue &sv)
{
    reviews.clear();

    if (sv.isValid()) {
        QScriptValue sv2 = sv.property(reviews_element);
        if (sv2.isValid()) {
            processReviews(sv2);
            emit finished(NoError, QString());
        } else {
            emit finished(ParsingError, QString("JSON data are invalid"));
        }
    } else {
        emit finished(ParsingError, QString("JSON data are invalid"));
    }
}

void QPlaceJSonReviewParser::processReviews(const QScriptValue &reviewsElement)
{
    QScriptValue value = reviewsElement.property(review_element);
    if (value.isValid()) {
        if (value.isArray()) {
            QScriptValueIterator it(value);
            while (it.hasNext()) {
                it.next();
                // array contains count as last element
                if (it.name() != "length") {
                    reviews.append(buildReview(it.value(), m_manager));
                }
            }
        } else {
            reviews.append(buildReview(value, m_manager));
        }
    }
    value = reviewsElement.property(review_count);
    if (value.isValid()) {
        allReviews = value.toInt32();
    }
}
