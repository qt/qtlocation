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

#include "qplacejsonsearchparser.h"

#include <QJSEngine>
#include <QJSValue>
#include <QJSValueIterator>

#include <qgeocoordinate.h>
#include <qgeoaddress.h>
#include <qgeolocation.h>
#include <qplace.h>
#include <qplaceratings.h>
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

QT_USE_NAMESPACE

QPlaceJSonSearchParser::QPlaceJSonSearchParser(QObject *parent) :
    QPlaceJSonParser(parent)
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

void QPlaceJSonSearchParser::processJSonData(const QJSValue &sv)
{
    searchResultsList.clear();

    if (sv.isValid()) {
        QJSValue results = sv.property(search_results_element);
        if (results.isValid()) {
            QJSValueIterator it(results);
            while (it.hasNext()) {
                it.next();
                // array contains count as last element
                if (it.name() != QLatin1String("length")) {
                    processResultElement(it.value());
                }
            }
            emit finished(NoError, QString());
        } else {
            emit finished(ParsingError, QString("JSON data are invalid"));
        }
    } else {
        emit finished(ParsingError, QString("JSON data are invalid"));
    }
}

void QPlaceJSonSearchParser::processResultElement(const QJSValue &value)
{
    // Procesing DYM string
    QJSValue type = value.property(search_type_element);
    if (type.isValid() && type.toString() == search_type_dym_value) {
        QJSValue properties = value.property(search_properties_element);
        if (properties.isValid()) {
            QJSValue title = properties.property(search_properties_title_value);
            if (title.isValid() && !title.toString().isEmpty()) {
                QPlaceSearchResult result;
                QJSValue type = properties.property(search_type_element);
                if (type.isValid()) {
                    result = processPlaceElement(value);
                }
                result.setType(QPlaceSearchResult::CorrectionResult);
                result.setCorrection(title.toString());
                searchResultsList.append(result);
            }
        }
    // processing place element
    } else {
        searchResultsList.append(processPlaceElement(value));
    }
}

QPlaceSearchResult QPlaceJSonSearchParser::processPlaceElement(const QJSValue &results)
{
    QPlaceSearchResult result;
    result.setType(QPlaceSearchResult::PlaceResult);
    QPlace newPlace;

    // Processing properties
    QJSValue properties = results.property(search_properties_element);
    if (properties.isValid()) {
        // QSearchResult properties
        QJSValue distance = properties.property(search_properties_distance_value);
        if (distance.isValid() && !distance.toString().isEmpty()) {
            bool isConverted;
            double distanceValue = distance.toString().toDouble(&isConverted);
            if (isConverted) {
                result.setDistance(distanceValue);
            }
        }
        // Place properties
        QJSValue value = properties.property(search_properties_title_value);
        if (value.isValid() && !value.toString().isEmpty()) {
            newPlace.setName(value.toString());
        }
        value = properties.property(search_properties_placeid_value);
        if (value.isValid() && !value.toString().isEmpty()) {
            newPlace.setPlaceId(value.toString());
        }
        value = properties.property(search_properties_description_value);
        if (value.isValid() && !value.toString().isEmpty()) {
            //The JSON data specification defines short description
            //but our API doesn't expose this so we
            //parse and skip assignment
        }
        value = properties.property(search_properties_supliers_value);
        if (value.isValid() && !value.toString().isEmpty()) {
            QPlaceSupplier sup;
            sup.setName(value.toString());
            newPlace.setSupplier(QPlaceSuppliersRepository::instance()->addSupplier(sup));
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

void QPlaceJSonSearchParser::processContacts(const QJSValue &properties, QPlace *place)
{
    QJSValue value = properties.property(search_properties_url_value);
    if (value.isValid() && !value.toString().isEmpty()) {
        QPlaceContactDetail contactDetail;
        contactDetail.setLabel(tr("Website"));
        contactDetail.setValue(value.toString());
        place->appendContactDetail(QPlaceContactDetail::Website, contactDetail);
    }
    value = properties.property(search_properties_phone_value);
    if (value.isValid() && !value.toString().isEmpty()) {
        QPlaceContactDetail contactDetail;
        contactDetail.setLabel(tr("Phone"));
        contactDetail.setValue(value.toString());
        place->appendContactDetail(QPlaceContactDetail::Phone, contactDetail);
    }
}


void QPlaceJSonSearchParser::processCategories(const QJSValue &categories, QPlace *place)
{
    QList<QPlaceCategory> categoriesList;
    QJSValueIterator it(categories);
    while (it.hasNext()) {
        it.next();
        // array contains count as last element
        if (it.name() != "length") {
            QJSValue value = it.value().property(search_categories_id_value);
            if (value.isValid() && !value.toString().isEmpty()) {
                QPlaceCategory category = QPlaceCategoriesRepository::instance()->mapCategory(
                            value.toString());
                if (!category.categoryId().isEmpty()) {
                    categoriesList.append(category);
                }
            }
        }
    }
    place->setCategories(categoriesList);
}

void QPlaceJSonSearchParser::processRating(const QJSValue &properties, QPlace *place)
{
    QJSValue value = properties.property(search_properties_rating_value);
    if (value.isValid() && !value.toString().isEmpty()) {
        bool isConverted;
        double ratingValue = value.toString().toDouble(&isConverted);
        if (isConverted) {
            QPlaceRatings rating;
            rating.setValue(ratingValue);
            place->setRatings(rating);
        }
    }
}

void QPlaceJSonSearchParser::processAddress(const QJSValue &properties, QGeoLocation *location)
{
    QGeoAddress newAddress;
    QJSValue value = properties.property(search_properties_address_country);
    if (value.isValid() && !value.toString().isEmpty()) {
        newAddress.setCountry(value.toString());
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
        newAddress.setPostcode(value.toString());
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
        //TODO: need to figure out how to deal with street number
        //and whether to keep it separate from street.
        newAddress.setStreet(value.toString() + " " + newAddress.street());
    }
    location->setAddress(newAddress);
}

void QPlaceJSonSearchParser::processLocation(const QJSValue &properties, QPlace *place)
{
    QGeoLocation location;

    bool latOK(false);
    bool lonOK(false);
    double latitude;
    double longitude;

    // display position
    QJSValue value = properties.property(search_properties_latitude_value);
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
        location.setCoordinate(pos);
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
        QGeoBoundingBox boundingBox;
        boundingBox.setTopLeft(topLeft);
        boundingBox.setBottomRight(bottomRight);
        location.setBoundingBox(boundingBox);
    }

    processAddress(properties, &location);
    place->setLocation(location);
}
