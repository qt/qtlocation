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

#include "qplacejsonsearchparser.h"

#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptValueIterator>

#include <qplace.h>
#include <qgeocoordinate.h>
#include <qplacecontact.h>
#include <qplacelocation.h>
#include <qplaceaddress.h>
#include <qplacerating.h>
#include <qplacecategory.h>
#include <qplacesupplier.h>
#include <qplacesearchresult.h>

#include "qplacesuppliersrepository.h"
#include "qplacecategoriesrepository.h"

#if defined(QT_PLACES_LOGGING)
    #include <QDebug>
#endif

//search const
static const char *search_results_element = "results";
static const char *search_type_element = "type";
static const char *search_type_dym_value = "DID_YOU_MEAN_SEARCH";
static const char *search_type_addplace_value = "ADPLACE";

static const char *search_categories_element = "categories";
static const char *search_categories_id_value = "id";
static const char *search_properties_element = "properties";
static const char *search_properties_title_value = "title";
static const char *search_properties_distance_value = "geoDistance";

static const char *search_properties_placeid_value = "placesId";
static const char *search_properties_description_value = "description";
static const char *search_properties_supliers_value = "dataProvider";
static const char *search_properties_phone_value = "phoneNumber";
static const char *search_properties_url_value = "url";
static const char *search_properties_rating_value = "placesRating";

static const char *search_properties_longitude_value = "geoLongitude";
static const char *search_properties_latitude_value = "geoLatitude";
static const char *search_properties_longitude_bb1_value = "GEO_BBX_LONGITUDE_1";
static const char *search_properties_latitude_bb1_value = "GEO_BBX_LATITUDE_1";
static const char *search_properties_longitude_bb2_value = "GEO_BBX_LONGITUDE_2";
static const char *search_properties_latitude_bb2_value = "GEO_BBX_LATITUDE_2";

static const char *search_properties_address_country = "addrCountryName";
static const char *search_properties_address_county = "addrCountyName";
static const char *search_properties_address_country_code = "addrCountryCode";
static const char *search_properties_address_state = "addrStateName";
static const char *search_properties_address_code = "addrPostalCode";
static const char *search_properties_address_city = "addrCityName";
static const char *search_properties_address_district = "addrDistrictName";
static const char *search_properties_address_street = "addrStreetName";
static const char *search_properties_address_house_number = "addrHouseNumber";

QTM_USE_NAMESPACE

QPlaceJSonSearchParser::QPlaceJSonSearchParser(QObject *parent) :
    QObject(parent),
    engine(NULL)
{
}

QPlaceJSonSearchParser::~QPlaceJSonSearchParser()
{
    searchResultsList.clear();
}

QList<QPlaceSearchResult> QPlaceJSonSearchParser::searchResults()
{
    return searchResultsList;
}

void QPlaceJSonSearchParser::processData(const QString &data)
{
    if (!engine) {
        engine = new QScriptEngine(this);
    }
    searchResultsList.clear();

    QScriptValue sv = engine->evaluate("(" + data + ")");
    if (sv.isValid()) {
        QScriptValueIterator it(sv.property(search_results_element));
            while (it.hasNext()) {
                it.next();
                // array contains count as last element
                if (it.name() != "length") {
                    processResultElement(it.value());
                }
            }
        emit finished(NoError, QString());
    } else {
        emit finished(ParsingError, QString("JSON data are invalid"));
    }
}

void QPlaceJSonSearchParser::processResultElement(const QScriptValue &value)
{
    // Procesing DYM string
    QScriptValue type = value.property(search_type_element);
    if (type.isValid() && type.toString() == search_type_dym_value) {
        QScriptValue properties = value.property(search_properties_element);
        if (properties.isValid()) {
            QScriptValue title = properties.property(search_properties_title_value);
            if (title.isValid() && !title.toString().isEmpty()) {
                QPlaceSearchResult result;
                QScriptValue type = properties.property(search_type_element);
                if (type.isValid()) {
                    result = processPlaceElement(value);
                }
                result.setType(QPlaceSearchResult::DidYouMeanSuggestion);
                result.setDidYouMeanSuggestion(title.toString());
                searchResultsList.append(result);
            }
        }
    // processing place element
    } else {
        searchResultsList.append(processPlaceElement(value));
    }
}

QPlaceSearchResult QPlaceJSonSearchParser::processPlaceElement(const QScriptValue &results)
{
    QPlaceSearchResult result;
    result.setType(QPlaceSearchResult::Place);
    QPlace newPlace;

    // Processing properties
    QScriptValue properties = results.property(search_properties_element);
    if (properties.isValid()) {
        // QSearchResult properties
        QScriptValue distance = properties.property(search_properties_distance_value);
        if (distance.isValid() && !distance.toString().isEmpty()) {
            bool isConverted;
            double distanceValue = distance.toString().toDouble(&isConverted);
            if (isConverted) {
                result.setDistance(distanceValue);
            }
        }
        // Place properties
        QScriptValue value = properties.property(search_properties_title_value);
        if (value.isValid() && !value.toString().isEmpty()) {
            newPlace.setName(value.toString());
        }
        value = properties.property(search_type_element);
        if (value.isValid() && !value.toString().isEmpty()) {
            QVariantHash addData;
            QString type = value.toString();
            addData.insert(search_type_element, type);
            newPlace.setAdditionalData(addData);
        }
        value = properties.property(search_properties_placeid_value);
        if (value.isValid() && !value.toString().isEmpty()) {
            newPlace.setPlaceId(value.toString());
        }
        value = properties.property(search_properties_description_value);
        if (value.isValid() && !value.toString().isEmpty()) {
            newPlace.setShortDescription(value.toString());
        }
        value = properties.property(search_properties_supliers_value);
        if (value.isValid() && !value.toString().isEmpty()) {
            QPlaceSupplier sup;
            sup.setName(value.toString());
            QList<QPlaceSupplier> list;
            list.append(QPlaceSuppliersRepository::instance()->addSupplier(sup));
            newPlace.setSuppliers(list);
        }
        processContacts(properties, &newPlace);
        processRating(properties, &newPlace);
        processLocation(properties, &newPlace);

        value = results.property(search_categories_element);
        if (value.isValid()) {
            processCategories(value, &newPlace);
        }
    }
    result.setPlace(newPlace);
    return result;
}

void QPlaceJSonSearchParser::processContacts(const QScriptValue &properties, QPlace *place)
{
    QList<QPlaceContact> contacts;
    QScriptValue value = properties.property(search_properties_url_value);
    if (value.isValid() && !value.toString().isEmpty()) {
        QPlaceContact contact;
        contact.setType(QPlaceContact::URL);
        contact.setValue(value.toString());
        contacts.append(contact);
    }
    value = properties.property(search_properties_phone_value);
    if (value.isValid() && !value.toString().isEmpty()) {
        QPlaceContact contact;
        contact.setType(QPlaceContact::Phone);
        contact.setValue(value.toString());
        contacts.append(contact);
    }
    place->setContacts(contacts);
}


void QPlaceJSonSearchParser::processCategories(const QScriptValue &categories, QPlace *place)
{
    QList<QPlaceCategory> categoriesList;
    QScriptValueIterator it(categories);
    while (it.hasNext()) {
        it.next();
        // array contains count as last element
        if (it.name() != "length") {
            QScriptValue value = it.value().property(search_categories_id_value);
            if (value.isValid() && !value.toString().isEmpty()) {
                QPlaceCategory category = QPlaceCategoriesRepository::instance()->mapCategory(
                            value.toString());
                if (!category.isEmpty()) {
                    categoriesList.append(category);
                }
            }
        }
    }
    place->setCategories(categoriesList);
}

void QPlaceJSonSearchParser::processRating(const QScriptValue &properties, QPlace *place)
{
    QScriptValue value = properties.property(search_properties_rating_value);
    if (value.isValid() && !value.toString().isEmpty()) {
        bool isConverted;
        double ratingValue = value.toString().toDouble(&isConverted);
        if (isConverted) {
            QPlaceRating rating;
            rating.setValue(ratingValue);
            place->setRating(rating);
        }
    }
}

void QPlaceJSonSearchParser::processAddress(const QScriptValue &properties, QPlaceLocation *location)
{
    QPlaceAddress newAddress;
    QScriptValue value = properties.property(search_properties_address_country);
    if (value.isValid() && !value.toString().isEmpty()) {
        newAddress.setCountryName(value.toString());
    }
    value = properties.property(search_properties_address_county);
    if (value.isValid() && !value.toString().isEmpty()) {
        newAddress.setCounty(value.toString());
    }
    value = properties.property(search_properties_address_country_code);
    if (value.isValid() && !value.toString().isEmpty()) {
        newAddress.setCountry(value.toString());
    }
    value = properties.property(search_properties_address_state);
    if (value.isValid() && !value.toString().isEmpty()) {
        newAddress.setState(value.toString());
    }
    value = properties.property(search_properties_address_code);
    if (value.isValid() && !value.toString().isEmpty()) {
        newAddress.setPostalCode(value.toString());
    }
    value = properties.property(search_properties_address_city);
    if (value.isValid() && !value.toString().isEmpty()) {
        newAddress.setCity(value.toString());
    }
    value = properties.property(search_properties_address_district);
    if (value.isValid() && !value.toString().isEmpty()) {
        newAddress.setDistrict(value.toString());
    }
    value = properties.property(search_properties_address_street);
    if (value.isValid() && !value.toString().isEmpty()) {
        newAddress.setStreet(value.toString());
    }
    value = properties.property(search_properties_address_house_number);
    if (value.isValid() && !value.toString().isEmpty()) {
        newAddress.setHouseNumber(value.toString());
    }
    location->setAddress(newAddress);
}

void QPlaceJSonSearchParser::processLocation(const QScriptValue &properties, QPlace *place)
{
    QPlaceLocation location;

    bool latOK(false);
    bool lonOK(false);
    double latitude;
    double longitude;

    // display position
    QScriptValue value = properties.property(search_properties_latitude_value);
    if (value.isValid() && !value.toString().isEmpty()) {
        latitude = value.toString().toDouble(&latOK);
    }
    value = properties.property(search_properties_longitude_value);
    if (value.isValid() && !value.toString().isEmpty()) {
        longitude = value.toString().toDouble(&lonOK);
    }
    if (latOK && lonOK) {
        QGeoCoordinate pos;
        pos.setLatitude(latitude);
        pos.setLongitude(longitude);
        location.setDisplayPosition(pos);
    }

    // bounding box
    QGeoCoordinate bottomRight;
    QGeoCoordinate topLeft;
    latOK = false;
    lonOK = false;
    value = properties.property(search_properties_latitude_bb1_value);
    if (value.isValid() && !value.toString().isEmpty()) {
        latitude = value.toString().toDouble(&latOK);
    }
    value = properties.property(search_properties_longitude_bb1_value);
    if (value.isValid() && !value.toString().isEmpty()) {
        longitude = value.toString().toDouble(&lonOK);
    }
    if (latOK && lonOK) {
        topLeft.setLatitude(latitude);
        topLeft.setLongitude(longitude);
    }

    latOK = false;
    lonOK = false;
    value = properties.property(search_properties_latitude_bb2_value);
    if (value.isValid() && !value.toString().isEmpty()) {
        latitude = value.toString().toDouble(&latOK);
    }
    value = properties.property(search_properties_longitude_bb2_value);
    if (value.isValid() && !value.toString().isEmpty()) {
        longitude = value.toString().toDouble(&lonOK);
    }
    if (latOK && lonOK) {
        bottomRight.setLatitude(latitude);
        bottomRight.setLongitude(longitude);
    }

    if (topLeft.isValid() && bottomRight.isValid()) {
        QPlaceGeoBoundingBox boundingBox;
        boundingBox.setTopLeft(topLeft);
        boundingBox.setBottomRight(bottomRight);
        location.setMapView(boundingBox);
    }

    processAddress(properties, &location);
    place->setLocation(location);
}
