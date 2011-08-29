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

#include "qplacejsondetailsparser.h"

#include <QtCore/QUrl>
#include <QtCore/QTime>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptValueIterator>

#include <qgeoaddress.h>
#include <qgeocoordinate.h>
#include <qgeoplace.h>
#include <qplacecategory.h>
#include <qplacedescription.h>
#include <qplacerating.h>
#include <qgeolocation.h>
#include <qplacesupplier.h>
#include "qplacejsoncategoriesparser.h"
#include "qplacesuppliersrepository.h"

#if defined(QT_PLACES_LOGGING)
    #include <QDebug>
#endif

//search const
static const char *place_place_element = "place";
static const char *place_id_element = "a_id";
static const char *place_is_ad_place_value = "isAdPlace";

static const char *place_provider = "provider";
static const char *place_provider_url = "providerUrl";

static const char *place_categories_element = "categories";

static const char *place_contact_element = "contact";
static const char *place_contact_email_element = "email";
static const char *place_contact_fax_element = "fax";
static const char *place_contact_phone_element = "phone";
static const char *place_contact_website_element = "website";
static const char *place_contact_im_element = "im";

static const char *place_tags_element = "tags";
static const char *place_tags_value_element = "value";

static const char *place_names_element = "names";
static const char *place_alternativenames_element = "alternativeNames";
static const char *place_name_default_element = "defaultName";
static const char *place_name_localized_element = "localizedName";
static const char *place_name_value_element = "name";
static const char *place_name_language_element = "language";

static const char *place_ratings_element = "averageRatings";
static const char *place_rating_element = "averageRating";
static const char *place_rating_rating_element = "rating";
static const char *place_rating_count_element = "ratingCount";
static const char *place_rating_type_element = "ratingType";
static const char *place_rating_type_overall_element = "OVERALL";

static const char *place_location_element = "location";
static const char *place_geoCoordinates_element = "geoCoordinates";
static const char *place_geoCoordinates_longitude_value = "longitude";
static const char *place_geoCoordinates_latitude_value = "latitude";
//! @todo check if bounding box should be supported
//static const char *place_geoCoordinates_longitude_bb1_value = "GEO_BBX_LONGITUDE_1";
//static const char *place_geoCoordinates_latitude_bb1_value = "GEO_BBX_LATITUDE_1";
//static const char *place_geoCoordinates_longitude_bb2_value = "GEO_BBX_LONGITUDE_2";
//static const char *place_geoCoordinates_latitude_bb2_value = "GEO_BBX_LATITUDE_2";

static const char *place_address_element = "address";
static const char *place_address_country = "localizedCountryName";
static const char *place_address_county = "county";
static const char *place_address_country_code = "countryCode3L";
static const char *place_address_state = "state";
static const char *place_address_code = "zipCode";
static const char *place_address_city = "city";
static const char *place_address_district = "district";
static const char *place_address_street = "street";
static const char *place_address_house_number = "houseNumber";

static const char *place_adcontent_element = "adPlaceContent";
static const char *place_adcontent_descriptions_element = "descriptions";
static const char *place_adcontent_description_element = "description";
static const char *place_adcontent_localizedDescription_element = "localizedDescription";
static const char *place_adcontent_descriptionLanguage_element = "languageOfDescription";
static const char *place_adcontent_medias_element = "mediaTypes";
static const char *place_adcontent_media_element = "adPlaceMedia";
static const char *place_adcontent_media_mime_element = "mediaMimeType";
static const char *place_adcontent_media_url_element = "mediaUrl";
static const char *place_adcontent_paymentmethods_element = "paymentMethods";
static const char *place_adcontent_paymentmethod_element = "paymentMetod";
static const char *place_adcontent_paymentmethod_name_element = "name";
static const char *place_adcontent_hours_element = "businessHours";
static const char *place_adcontent_hours_annualclosingsnotes_element = "annualclosingsnotes";
static const char *place_adcontent_hours_annualclosingsnote_element = "annualclosingsnote";
static const char *place_adcontent_hours_annualclosingsnote_v_element = "_v";
static const char *place_adcontent_hours_open_element = "openingSchedule";
static const char *place_adcontent_hours_open_hours_element = "openingHours";
static const char *place_adcontent_hours_open_day_element = "dayOfWeek";
static const char *place_adcontent_hours_open_from_element = "openFrom";
static const char *place_adcontent_hours_open_to_element = "openTo";
static const char *place_adcontent_hours_openingnotes_element = "openingnotes";
static const char *place_adcontent_hours_openingnote_element = "openingnote";
static const char *place_adcontent_hours_openingnote_v_element = "_v";
static const char *place_adcontent_packages_element = "packages";
static const char *place_adcontent_package_element = "package";
static const char *place_adcontent_package_type_element = "packageType";
static const char *place_adcontent_package_type_plus_value = "PLUS";
static const char *place_adcontent_package_type_plus_plus_value = "PLUS+";

static const char *place_package_type = "type";
static const char *place_package_type_ad_place = "ADPLACE";
static const char *place_package_type_ad_place_prime_plus = "ADPLACE, PRIME_PLUS";
static const char *place_package_type_ad_place_prime_plus_plus = "ADPLACE, PRIME_PLUS_PLUS";
static const char *place_package_type_place = "PLACE";

static const char *place_premiumcontent_element = "premiumContent";
static const char *place_premiumcontent_version_element = "version";
static const char *place_premiumcontent_content_element = "content";
static const char *place_premiumcontent_content_desc_element = "description";
static const char *place_premiumcontent_content_shortdesc_element = "short-description";
static const char *place_premiumcontent_content_name_element = "name";
static const char *place_premiumcontent_content_providername_element = "providerDisplayName";
static const char *place_premiumcontent_content_provider_element = "provider";
static const char *place_premiumcontent_content_vendorurl_element = "vendor-url";
static const char *place_premiumcontent_content_providerIconUrl_element = "providerIconUrl";
static const char *place_premiumcontent_content_language_element = "language";
static const char *place_premiumcontent_content_media_element = "media";
static const char *place_premiumcontent_content_mediaurl_element = "content";
static const char *place_premiumcontent_content_mediamimetype_element = "mimeType";

static const char *place_premiumcontent_content_monday = "MON";
static const char *place_premiumcontent_content_tuesday = "TUE";
static const char *place_premiumcontent_content_wednesday = "WED";
static const char *place_premiumcontent_content_thursday = "THU";
static const char *place_premiumcontent_content_friday = "FRI";
static const char *place_premiumcontent_content_saturday = "SAT";
static const char *place_premiumcontent_content_sunday = "SUN";

QT_USE_NAMESPACE

QPlaceJSonDetailsParser::QPlaceJSonDetailsParser(QObject *parent) :
    QPlaceJSonParser(parent)
{
}

QPlaceJSonDetailsParser::~QPlaceJSonDetailsParser()
{
}

QGeoPlace QPlaceJSonDetailsParser::buildPlace(const QScriptValue &placeValue)
{
    QGeoPlace newPlace;
    if (placeValue.isValid()) {
        buildPlace(placeValue, &newPlace);
    }
    return newPlace;
}

QGeoPlace QPlaceJSonDetailsParser::result()
{
    return place;
}

void QPlaceJSonDetailsParser::processJSonData(const QScriptValue &sv)
{
    if (sv.isValid()) {
        QScriptValue placeProperty = sv.property(place_place_element);
        if (placeProperty.isValid()) {
            buildPlace(placeProperty, &place);
            emit finished(NoError, QString());
        } else {
            emit finished(ParsingError, QString("JSON data are invalid"));
        }
    } else {
        emit finished(ParsingError, QString("JSON data are invalid"));
    }
}

void QPlaceJSonDetailsParser::buildPlace(const QScriptValue &placeValue, QGeoPlace*targetPlace)
{
    if (placeValue.isValid()) {
        QScriptValue value = placeValue.property(place_id_element);
        if (value.isValid() && !value.toString().isEmpty()) {
            targetPlace->setPlaceId(value.toString());
        }
        processMainProvider(placeValue, targetPlace);
        value = placeValue.property(place_categories_element);
        if (value.isValid()) {
            processCategories(value, targetPlace);
        }
        value = placeValue.property(place_contact_element);
        if (value.isValid()) {
            processContacts(value, targetPlace);
        }
        value = placeValue.property(place_tags_element);
        if (value.isValid()) {
            processTags(value, targetPlace);
        }
        value = placeValue.property(place_names_element);
        if (value.isValid()) {
            processNames(value, targetPlace);
        }
        value = placeValue.property(place_ratings_element);
        if (value.isValid()) {
            processRatings(value, targetPlace);
        }
        value = placeValue.property(place_location_element);
        if (value.isValid()) {
            processLocation(value, targetPlace);
        }
        value = placeValue.property(place_adcontent_element);
        if (value.isValid()) {
            processAdContent(value, targetPlace);
        }
        value = placeValue.property(place_premiumcontent_element);
        if (value.isValid()) {
            processPremiumContents(value, targetPlace);
        }
    }
}

void QPlaceJSonDetailsParser::processMainProvider(const QScriptValue &placeValue, QGeoPlace*targetPlace)
{
    QPlaceSupplier sup;
    QScriptValue value = placeValue.property(place_provider);
    if (value.isValid() && !value.toString().isEmpty()) {
        sup.setName(value.toString());
    }
    else {
        return;
    }
    value = placeValue.property(place_provider_url);
    if (value.isValid() && !value.toString().isEmpty()){
        sup.setSupplierIconUrl(value.toString());
    }
    QList<QPlaceSupplier> list;
    list.append(QPlaceSuppliersRepository::instance()->addSupplier(sup));
    targetPlace->setSuppliers(list);
}

void QPlaceJSonDetailsParser::processContacts(const QScriptValue &contactsValue, QGeoPlace*targetPlace)
{
    QScriptValueIterator it(contactsValue);
    bool phoneRetrieved = false;
    bool faxRetrieved = false;
    bool emailRetrieved = false;
    bool urlRetrieved = false;
    while (it.hasNext()) {
        it.next();
        if (it.name() == place_contact_website_element) {
            if (!urlRetrieved) {
                targetPlace->setPrimaryUrl(QUrl::fromEncoded(it.value().toString().toAscii()));
                urlRetrieved = true;
            }
        } else if (it.name() == place_contact_phone_element) {
            if (!phoneRetrieved) {
                targetPlace->setPrimaryPhone(it.value().toString());
                phoneRetrieved = true;
            }
        } else if (it.name() == place_contact_fax_element) {
            if (!faxRetrieved) {
                targetPlace->setPrimaryFax(it.value().toString());
                faxRetrieved = true;
            }
        } else if (it.name() == place_contact_email_element) {
            if (!emailRetrieved) {
                targetPlace->setPrimaryEmail(it.value().toString());
                faxRetrieved = true;
            }
        } else {
            //unknown contact element, do nothing
        }
    }
}

void QPlaceJSonDetailsParser::processCategories(const QScriptValue &categories, QGeoPlace*targetPlace)
{
    targetPlace->setCategories(QPlaceJSonCategoriesParser::parseFlatCategoryList(categories));
}

void QPlaceJSonDetailsParser::processRatings(const QScriptValue &ratings, QGeoPlace*targetPlace)
{
    QPlaceRating *rating = NULL;
    QScriptValue value = ratings.property(place_rating_element);
    if (value.isValid()) {
        if (value.isArray()) {
            QScriptValueIterator it(value);
            while (it.hasNext()) {
                it.next();
                // array contains count as last element
                if (it.name() != "length") {
                    rating = processRating(it.value());
                    if (rating) {
                        break;
                    }
                }
            }
        } else {
            rating = processRating(value);
        }
    }
    if (rating) {
        targetPlace->setRating(*rating);
        delete rating;
        rating = NULL;
    }
}

QPlaceRating *QPlaceJSonDetailsParser::processRating(const QScriptValue &ratingElement)
{
    QPlaceRating *rating = NULL;
    QScriptValue value = ratingElement.property(place_rating_type_element);
    // Only overall elements are interesting
    if (value.isValid() && value.toString() == place_rating_type_overall_element) {
        rating = new QPlaceRating();
        value = ratingElement.property(place_rating_count_element);
        if (value.isValid() && !value.toString().isEmpty()) {
            bool isConverted;
            uint ratingValue = value.toString().toInt(&isConverted);
            if (isConverted) {
                rating->setCount(ratingValue);
            }
        }
        value = ratingElement.property(place_rating_rating_element);
        if (value.isValid() && !value.toString().isEmpty()) {
            bool isConverted;
            double ratingValue = value.toString().toDouble(&isConverted);
            if (isConverted) {
                rating->setValue(ratingValue);
            }
        }
    }
    return rating;
}

void QPlaceJSonDetailsParser::processAddress(const QScriptValue &address, QGeoLocation *location)
{
    QGeoAddress newAddress;
    QScriptValue value = address.property(place_address_street);
    if (value.isValid() && !value.toString().isEmpty()) {
        newAddress.setStreet(value.toString());
    }
    value = address.property(place_address_country);
    if (value.isValid() && !value.toString().isEmpty()) {
        newAddress.setCountry(value.toString());
    }
    value = address.property(place_address_county);
    if (value.isValid() && !value.toString().isEmpty()) {
        newAddress.setCounty(value.toString());
    }
    value = address.property(place_address_country_code);
    if (value.isValid() && !value.toString().isEmpty()) {
        newAddress.setCountry(value.toString());
    }
    value = address.property(place_address_state);
    if (value.isValid() && !value.toString().isEmpty()) {
        newAddress.setState(value.toString());
    }
    value = address.property(place_address_city);
    if (value.isValid() && !value.toString().isEmpty()) {
        newAddress.setCity(value.toString());
    }
    value = address.property(place_address_code);
    if (value.isValid() && !value.toString().isEmpty()) {
        newAddress.setPostcode(value.toString());
    }
    value = address.property(place_address_district);
    if (value.isValid() && !value.toString().isEmpty()) {
        newAddress.setDistrict(value.toString());
    }
    value = address.property(place_address_house_number);
    if (value.isValid() && !value.toString().isEmpty()) {
        newAddress.setStreet(value.toString() + " " + newAddress.street());
    }
    location->setAddress(newAddress);
}

void QPlaceJSonDetailsParser::processLocation(const QScriptValue &location, QGeoPlace *targetPlace)
{
    QGeoLocation newLocation;
    QScriptValue property = location.property(place_geoCoordinates_element);
    if (property.isValid()) {
        QGeoCoordinate pos;
        QScriptValue value = property.property(place_geoCoordinates_longitude_value);
        if (value.isValid() && !value.toString().isEmpty()) {
            bool isConverted;
            double longitude = value.toString().toDouble(&isConverted);
            if (isConverted) {
                pos.setLongitude(longitude);
            }
        }
        value = property.property(place_geoCoordinates_latitude_value);
        if (value.isValid() && !value.toString().isEmpty()) {
            bool isConverted;
            double latitude = value.toString().toDouble(&isConverted);
            if (isConverted) {
                pos.setLatitude(latitude);
            }
        }
        newLocation.setCoordinate(pos);
    }
    property = location.property(place_address_element);
    if (property.isValid()) {
        processAddress(property, &newLocation);
    }
    targetPlace->setLocation(newLocation);
}

void QPlaceJSonDetailsParser::processTags(const QScriptValue &tags, QGeoPlace*targetPlace)
{
    QStringList newTags;
    if (tags.isValid()) {
        if (tags.isArray()) {
            QScriptValueIterator it(tags);
            while (it.hasNext()) {
                it.next();
                QScriptValue value = it.value().property(place_tags_value_element);
                if (value.isValid() && !value.toString().isEmpty()) {
                    newTags.append(value.toString());
                }
            }
        } else {
            QScriptValue value = tags.property(place_tags_value_element);
            if (value.isValid() && !value.toString().isEmpty()) {
                newTags.append(value.toString());
            }
        }
    }
    targetPlace->setTags(newTags);
}

void QPlaceJSonDetailsParser::processNames(const QScriptValue &names, QGeoPlace*targetPlace)
{
    QScriptValue value = names.property(place_alternativenames_element);
    if (value.isValid()) {
        value = value.property(place_name_localized_element);
        if (value.isValid()) {
            QStringList list;
            if (value.isArray()) {
                QScriptValueIterator it(value);
                while (it.hasNext()) {
                    it.next();
                    // array contains count as last element
                    if (it.name() != "length") {
                        QString name = processName(it.value());
                        if (!name.isEmpty()) {
                            list.append(name);
                        }
                    }
                }
            } else {
                QString name = processName(value);
                if (!name.isEmpty()) {
                    list.append(name);
                }
            }
            //The JSON data specification defines
            //an alternative names element,
            //but our api doesn't expose alternative names
            //so we parse and skip assignment to a place here.
        }
    }
    value = names.property(place_name_default_element);
    if (value.isValid()) {
        QString name = processName(value);
        if (!name.isEmpty()) {
            targetPlace->setName(name);
        }
    }
}

QString QPlaceJSonDetailsParser::processName(const QScriptValue &nameValue)
{
    QString name;
    QScriptValue value = nameValue.property(place_name_value_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        name = value.toString();
    }
    return name;
}

void QPlaceJSonDetailsParser::processPremiumContents(const QScriptValue &premiumContent, QGeoPlace*targetPlace)
{
    QScriptValue value = premiumContent.property(place_premiumcontent_version_element);
    if (value.isValid()) {
        if (value.isArray()) {
            QScriptValueIterator it(value);
            while (it.hasNext()) {
                it.next();
                // array contains count as last element
                if (it.name() != "length") {
                    processPremiumVersion(it.value(), targetPlace);
                }
            }
        } else {
            processPremiumVersion(value, targetPlace);
        }
    }
}

void QPlaceJSonDetailsParser::processPremiumVersion(const QScriptValue &content, QGeoPlace*targetPlace)
{
    QScriptValue value = content.property(place_premiumcontent_content_element);
    if (value.isValid()) {
        if (value.isArray()) {
            QScriptValueIterator it(value);
            while (it.hasNext()) {
                it.next();
                // array contains count as last element
                if (it.name() != "length") {
                    processPremiumContent(it.value(), targetPlace);
                }
            }
        } else {
            processPremiumContent(value, targetPlace);
        }
    }
}

void QPlaceJSonDetailsParser::processPremiumContent(const QScriptValue &content, QGeoPlace*targetPlace)
{
    QString name, id, iconUrl;
    QScriptValue value = content.property(place_premiumcontent_content_providername_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        name = value.toString();
    }
    value = content.property(place_premiumcontent_content_provider_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        id = value.toString();
    }
    value = content.property(place_premiumcontent_content_providerIconUrl_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        iconUrl = value.toString();
    }
    QPlaceSupplier supplier;
    if (!name.isEmpty() || !id.isEmpty()) {
        supplier.setName(name);
        supplier.setSupplierId(id);
        supplier.setSupplierIconUrl(iconUrl);
        supplier = QPlaceSuppliersRepository::instance()->addSupplier(supplier);
    }
    processPremiumContentDescription(content, supplier, targetPlace);
    processPremiumContentMediaObjects(content, supplier, targetPlace);
}

void QPlaceJSonDetailsParser::processPremiumContentDescription(const QScriptValue &content,
                                                               const QPlaceSupplier &supplier,
                                                               QGeoPlace*targetPlace)
{
    QScriptValue value = content.property(place_premiumcontent_content_desc_element);
    QPlaceDescription desc;
    if (value.isValid() && !value.toString().isEmpty()) {
        desc.setContent(value.toString());
    } else {
        value = content.property(place_premiumcontent_content_shortdesc_element);
        if (value.isValid() && !value.toString().isEmpty()) {
            desc.setContent(value.toString());
        }
    }
    // if no description do not continue
    if (desc.content().isEmpty()) {
        return;
    }
    value = content.property(place_premiumcontent_content_name_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        desc.setContentTitle(value.toString());
    }
    value = content.property(place_premiumcontent_content_vendorurl_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        desc.setSourceUrl(value.toString());
    }
    value = content.property(place_premiumcontent_content_language_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        desc.setLanguage(value.toString());
    }
    desc.setSupplier(supplier);
    QList<QPlaceDescription> list = targetPlace->descriptions();
    list.append(desc);
    targetPlace->setDescriptions(list);
}

void QPlaceJSonDetailsParser::processPremiumContentMediaObjects(const QScriptValue &content,
                                                                const QPlaceSupplier &supplier,
                                                                QGeoPlace*targetPlace)
{
    QScriptValue value = content.property(place_premiumcontent_content_media_element);
    if (value.isValid()) {
        if (value.isArray()) {
            QScriptValueIterator it(value);
            //Note: Currently only image types are supported by the server
            QPlaceContent::Collection images = targetPlace->content(QPlaceContent::ImageType);
            int insertionIndex = 0;
            if (!images.keys().isEmpty())
                insertionIndex = images.keys().last() + 1;

            while (it.hasNext()) {
                it.next();
                // array contains count as last element
                if (it.name() != "length") {
                    QPlaceImage *obj = processPremiumContentMediaObject(it.value());
                    if (obj) {
                        obj->setSupplier(supplier);
                        images.insert(insertionIndex, *obj);
                        insertionIndex++;
                        delete obj;
                    }
                }
            }
            targetPlace->setContent(QPlaceContent::ImageType, images);
        } else {
            QPlaceImage *obj = processPremiumContentMediaObject(value);
            if (obj) {
                obj->setSupplier(supplier);
                QPlaceContent::Collection images = targetPlace->content(QPlaceContent::ImageType);
                int insertionIndex = 0;
                if (!images.keys().isEmpty())
                    insertionIndex = images.keys().last() + 1;
                images.insert(insertionIndex,*obj);
                targetPlace->setContent(QPlaceContent::ImageType, images);
                delete obj;
            }
        }
    }
}

QPlaceImage *QPlaceJSonDetailsParser::processPremiumContentMediaObject(const QScriptValue &content)
{
    QPlaceImage *obj = NULL;
    QScriptValue value = content.property(place_premiumcontent_content_mediaurl_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        obj = new QPlaceImage();
        obj->setUrl(QUrl::fromEncoded(value.toString().toAscii()));
        obj->setId(value.toString());
        value = content.property(place_premiumcontent_content_mediamimetype_element);
        if (value.isValid() && !value.toString().isEmpty()) {
            obj->setMimeType(value.toString());
        }
    }
    return obj;
}

void QPlaceJSonDetailsParser::processAdContent(const QScriptValue &content, QGeoPlace*targetPlace)
{
    QScriptValue value = content.property(place_adcontent_descriptions_element);
    if (value.isValid()) {
        processAdContentDescriptions(value, targetPlace);
    }
    value = content.property(place_adcontent_medias_element);
    if (value.isValid()) {
        processAdContentMediaObjects(value, targetPlace);
    }
    value = content.property(place_adcontent_paymentmethods_element);
    if (value.isValid()) {
        processAdContentPaymentMethods(value, targetPlace);
    }
    value = content.property(place_adcontent_hours_element);
    if (value.isValid()) {
        processAdContentBusinessHours(value, targetPlace);
    }
    value = content.property(place_adcontent_packages_element);
    if (value.isValid()) {
        processAdContentPackages(value, targetPlace);
    }
}

void QPlaceJSonDetailsParser::processAdContentPackages(const QScriptValue &content, QGeoPlace*targetPlace)
{
    QScriptValue value = content.property(place_adcontent_package_element);
    if (value.isValid()) {
        value = value.property(place_adcontent_package_type_element);
        if (value.isValid() && !value.toString().isEmpty()) {
            QString package = value.toString();
            QVariantHash data = targetPlace->additionalData();
            if (package == place_adcontent_package_type_plus_value) {
                data[place_package_type] = place_package_type_ad_place_prime_plus;
            } else if (package == place_adcontent_package_type_plus_plus_value) {
                data[place_package_type] = place_package_type_ad_place_prime_plus_plus;
            }
            targetPlace->setAdditionalData(data);
        }
    }
}

void QPlaceJSonDetailsParser::processAdContentDescriptions(const QScriptValue &content, QGeoPlace*targetPlace)
{
    QScriptValue value = content.property(place_adcontent_description_element);
    if (value.isValid()) {
        if (value.isArray()) {
            QScriptValueIterator it(value);
            while (it.hasNext()) {
                it.next();
                // array contains count as last element
                if (it.name() != "length") {
                    QPlaceDescription *des = processAdContentDescription(it.value());
                    if (des) {
                        QList<QPlaceDescription> list = targetPlace->descriptions();
                        list.append(*des);
                        targetPlace->setDescriptions(list);
                        delete des;
                        des = NULL;
                    }
                }
            }
        } else {
            QPlaceDescription *des = processAdContentDescription(value);
            if (des) {
                QList<QPlaceDescription> list = targetPlace->descriptions();
                list.append(*des);
                targetPlace->setDescriptions(list);
                delete des;
                des = NULL;
            }
        }
    }
}

QPlaceDescription *QPlaceJSonDetailsParser::processAdContentDescription(const QScriptValue &content)
{
    QPlaceDescription *description = NULL;
    QScriptValue value = content.property(place_adcontent_localizedDescription_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        description = new QPlaceDescription();
        description->setContent(value.toString());
        value = content.property(place_adcontent_descriptionLanguage_element);
        if (value.isValid() && !value.toString().isEmpty()) {
            description->setLanguage(value.toString());
        }
    }
    return description;
}

void QPlaceJSonDetailsParser::processAdContentMediaObjects(const QScriptValue &content, QGeoPlace*targetPlace)
{
    QScriptValue value = content.property(place_adcontent_media_element);
    if (value.isValid()) {
        if (value.isArray()) {
            QScriptValueIterator it(value);
            //The server only has images for now.
            QPlaceContent::Collection images =targetPlace->content(QPlaceContent::ImageType);
            int insertionIndex = 0;
            if (!images.keys().isEmpty())
                insertionIndex = images.keys().last() + 1;

            while (it.hasNext()) {
                it.next();
                // array contains count as last element
                if (it.name() != "length") {
                    QPlaceImage *obj = processAdContentMediaObject(it.value());
                    if (obj) {
                        images.insert(insertionIndex,*obj);
                        insertionIndex++;
                        delete obj;
                    }
                }
            }
            targetPlace->setContent(QPlaceContent::ImageType, images);
        } else {
            QPlaceImage *obj = processAdContentMediaObject(value);
            if (obj) {
                QPlaceContent::Collection images = targetPlace->content(QPlaceContent::ImageType);
                int insertionIndex = 0;
                if (!images.keys().isEmpty())
                    insertionIndex = images.keys().last() + 1;
                images.insert(insertionIndex, *obj);
                targetPlace->setContent(QPlaceContent::ImageType,images);
                delete obj;
            }
        }
    }
}

QPlaceImage *QPlaceJSonDetailsParser::processAdContentMediaObject(const QScriptValue &content)
{
    QPlaceImage *obj = NULL;
    QString mediaMimeType;
    QString mediaUrl;

    QScriptValue value = content.property(place_adcontent_media_mime_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        mediaMimeType = value.toString();
    }
    value = content.property(place_adcontent_media_url_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        mediaUrl = value.toString();
    }
    if (!mediaMimeType.isEmpty() || !mediaUrl.isEmpty()) {
        obj = new QPlaceImage();
        obj->setUrl(QUrl::fromEncoded(mediaUrl.toAscii()));
        obj->setId(mediaUrl);
        obj->setMimeType(mediaMimeType);
    }
    return obj;
}

void QPlaceJSonDetailsParser::processAdContentPaymentMethods(const QScriptValue &content, QGeoPlace*targetPlace)
{
    QScriptValue value = content.property(place_adcontent_paymentmethod_element);
    if (value.isValid()) {
        QStringList list;
        if (value.isArray()) {
            QScriptValueIterator it(value);
            while (it.hasNext()) {
                it.next();
                // array contains count as last element
                if (it.name() != "length") {
                    QString obj = processAdContentPaymentMethod(it.value());
                    if (!obj.isEmpty()) {
                        list.append(obj);
                    }
                }
            }
        } else {
            QString obj = processAdContentPaymentMethod(value);
            if (!obj.isEmpty()) {
                list.append(obj);
            }
        }
        if (list.count()) {
            QPlaceAttribute paymentMethods;
            paymentMethods.setText(list.join(","));
            targetPlace->insertExtendedAttribute(QPlaceAttribute::PaymentMethods, paymentMethods);
        }
    }
}

QString QPlaceJSonDetailsParser::processAdContentPaymentMethod(const QScriptValue &content)
{
    QString obj;
    QScriptValue value = content.property(place_adcontent_paymentmethod_name_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        obj = value.toString();
    }
    return obj;
}

void QPlaceJSonDetailsParser::processAdContentBusinessHours(const QScriptValue &content, QGeoPlace*targetPlace)
{
    QScriptValue value = content.property(place_adcontent_hours_annualclosingsnotes_element);
    if (value.isValid()) {
        processAdContentClosingsNotes(value, targetPlace);
    }
    value = content.property(place_adcontent_hours_open_element);
    if (value.isValid()) {
        processAdContentOpeningHours(value, targetPlace);
    }
    value = content.property(place_adcontent_hours_openingnotes_element);
    if (value.isValid()) {
        processAdContentOpeningNotes(value, targetPlace);
    }
}

void QPlaceJSonDetailsParser::processAdContentClosingsNotes(const QScriptValue &content, QGeoPlace*targetPlace)
{
    QScriptValue value = content.property(place_adcontent_hours_annualclosingsnote_element);
    if (value.isValid()) {
        if (value.isArray()) {
            QScriptValueIterator it(value);
            while (it.hasNext()) {
                it.next();
                // array contains count as last element
                if (it.name() != "length") {
                    QString obj = processAdContentClosingsNote(it.value());
                    if (!obj.isEmpty()) {
                        //The JSON data specification defines closing notes
                        //but our API doesn't expose this so we
                        //parse and skip assignment

                        //! @todo only one is used
                        break;
                    }
                }
            }
        } else {
            QString obj = processAdContentClosingsNote(value);
            if (!obj.isEmpty()) {
                //The JSON data specification defines closing notes
                //but our API doesn't expose this so we
                //parse and skip assignment
            }
        }
    }
}

QString QPlaceJSonDetailsParser::processAdContentClosingsNote(const QScriptValue &content)
{
    QString ret;
    QScriptValue value = content.property(place_adcontent_hours_annualclosingsnote_v_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        ret = value.toString();
    }
    return ret;
}

void QPlaceJSonDetailsParser::processAdContentOpeningHours(const QScriptValue &content, QGeoPlace*targetPlace)
{
    QScriptValue value = content.property(place_adcontent_hours_open_hours_element);
    if (value.isValid()) {
        if (value.isArray()) {
            QScriptValueIterator it(value);
            while (it.hasNext()) {
                it.next();
                // array contains count as last element
                if (it.name() != "length") {
                    processAdContentOpeningHoursElement(it.value());
                    //The JSON data specification defines
                    //structured opening hour elements
                    //but our API doesn't expose this so we
                    //parse and skip assignment
                }
            }
        } else {
            processAdContentOpeningHoursElement(value);
            //The JSON data specification defines
            //structured opening hour elements
            //but our API doesn't expose this so we
            //parse and skip assignment
        }
    }
}

void QPlaceJSonDetailsParser::processAdContentOpeningHoursElement(const QScriptValue &content)
{
    //The JSON data specification defines
    //structured opening hour elements
    //but our API doesn't expose this so we
    //parse but do nothing else

    QScriptValue value = content.property(place_adcontent_hours_open_day_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        QString day = value.toString();
        if (place_premiumcontent_content_monday == day) {
        } else if (place_premiumcontent_content_tuesday == day) {
        } else if (place_premiumcontent_content_wednesday == day) {
        } else if (place_premiumcontent_content_thursday == day) {
        } else if (place_premiumcontent_content_friday == day) {
        } else if (place_premiumcontent_content_saturday == day) {
        } else if (place_premiumcontent_content_sunday == day) {
        }
    }
    QTime start, end;
    value = content.property(place_adcontent_hours_open_from_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        start = QTime::fromString(value.toString(),"hh:mm");
    }
    value = content.property(place_adcontent_hours_open_to_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        end = QTime::fromString(value.toString(),"hh:mm");
    }
    return;
}

void QPlaceJSonDetailsParser::processAdContentOpeningNotes(const QScriptValue &content, QGeoPlace*targetPlace)
{
    QScriptValue value = content.property(place_adcontent_hours_openingnote_element);
    if (value.isValid()) {
        if (value.isArray()) {
            QScriptValueIterator it(value);
            while (it.hasNext()) {
                it.next();
                // array contains count as last element
                if (it.name() != "length") {
                    QString obj = processAdContentOpeningNote(it.value());
                    if (!obj.isEmpty()) {
                        QPlaceAttribute openingNote;
                        openingNote.setText(obj);
                        targetPlace->insertExtendedAttribute(QPlaceAttribute::OpeningNote, openingNote);
                        //! @todo only one is used
                        break;
                    }
                }
            }
        } else {
            QString obj = processAdContentOpeningNote(value);
            if (!obj.isEmpty()) {
                QPlaceAttribute openingNote;
                openingNote.setText(obj);
                targetPlace->insertExtendedAttribute(QPlaceAttribute::OpeningNote, openingNote);
            }
        }
    }
}

QString QPlaceJSonDetailsParser::processAdContentOpeningNote(const QScriptValue &content)
{
    QString ret;
    QScriptValue value = content.property(place_adcontent_hours_openingnote_v_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        ret = value.toString();
    }
    return ret;
}
