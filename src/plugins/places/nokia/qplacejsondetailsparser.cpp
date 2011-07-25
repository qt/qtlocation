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

#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptValueIterator>

#include <qgeoaddress.h>
#include <qgeocoordinate.h>
#include <qgeoplace.h>
#include <qplacecontact.h>
#include <qplacecategory.h>
#include <qplacedescription.h>
#include <qplacerating.h>
#include <qplacebusinessinformation.h>
#include <qgeolocation.h>
#include <qplacealternativevalue.h>
#include <qplacemediaobject.h>
#include <qplaceperiod.h>
#include <qplaceweekdayhours.h>
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
    QList<QPlaceContact> contacts;
    QScriptValueIterator it(contactsValue);
    while (it.hasNext()) {
        it.next();
        QPlaceContact contact;
        if (it.name() == place_contact_website_element) {
            contact.setType(QPlaceContact::Url);
        }
        if (it.name() == place_contact_phone_element) {
            contact.setType(QPlaceContact::Phone);
        }
        if (it.name() == place_contact_fax_element) {
            contact.setType(QPlaceContact::Fax);
        }
        if (it.name() == place_contact_im_element) {
            contact.setType(QPlaceContact::IM);
        }
        if (it.name() == place_contact_email_element) {
            contact.setType(QPlaceContact::Email);
        }
        contact.setValue(it.value().toString());
        contacts.append(contact);
    }
    targetPlace->setContacts(contacts);
}

void QPlaceJSonDetailsParser::processCategories(const QScriptValue &categories, QGeoPlace*targetPlace)
{
    targetPlace->setCategories(QPlaceJSonCategoriesParser::processCategories(categories));
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
            QList<QPlaceAlternativeValue> list;
            if (value.isArray()) {
                QScriptValueIterator it(value);
                while (it.hasNext()) {
                    it.next();
                    // array contains count as last element
                    if (it.name() != "length") {
                        QPlaceAlternativeValue *name = processName(it.value());
                        if (name) {
                            list.append(*name);
                            delete name;
                            name = NULL;
                        }
                    }
                }
            } else {
                QPlaceAlternativeValue *name = processName(value);
                if (name) {
                    list.append(*name);
                    delete name;
                    name = NULL;
                }
            }
            targetPlace->setAlternativeNames(list);
        }
    }
    value = names.property(place_name_default_element);
    if (value.isValid()) {
        QPlaceAlternativeValue *name = processName(value);
        if (name) {
            targetPlace->setName(name->value());
            delete name;
            name = NULL;
        }
    }
}

QPlaceAlternativeValue *QPlaceJSonDetailsParser::processName(const QScriptValue &nameValue)
{
    QPlaceAlternativeValue *name = NULL;
    QScriptValue value = nameValue.property(place_name_value_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        name = new QPlaceAlternativeValue();
        name->setValue(value.toString());
        value = nameValue.property(place_name_language_element);
        if (value.isValid() && !value.toString().isEmpty()) {
            name->setLanguage(value.toString());
        }
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
            while (it.hasNext()) {
                it.next();
                // array contains count as last element
                if (it.name() != "length") {
                    QPlaceMediaObject *obj = processPremiumContentMediaObject(it.value());
                    if (obj) {
                        obj->setSupplier(supplier);
                        QPlacePaginationList<QPlaceMediaObject> list = targetPlace->media();
                        list.addItem(*obj);
                        targetPlace->setMedia(list);
                        delete obj;
                        obj = NULL;
                    }
                }
            }
        } else {
            QPlaceMediaObject *obj = processPremiumContentMediaObject(value);
            if (obj) {
                obj->setSupplier(supplier);
                QPlacePaginationList<QPlaceMediaObject> list = targetPlace->media();
                list.addItem(*obj);
                targetPlace->setMedia(list);
                delete obj;
                obj = NULL;
            }
        }
    }
}

QPlaceMediaObject *QPlaceJSonDetailsParser::processPremiumContentMediaObject(const QScriptValue &content)
{
    QPlaceMediaObject *obj = NULL;
    QScriptValue value = content.property(place_premiumcontent_content_mediaurl_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        obj = new QPlaceMediaObject();
        obj->setUrl(value.toString());
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
            while (it.hasNext()) {
                it.next();
                // array contains count as last element
                if (it.name() != "length") {
                    QPlaceMediaObject *obj = processAdContentMediaObject(it.value());
                    if (obj) {
                        QPlacePaginationList<QPlaceMediaObject> list = targetPlace->media();
                        list.addItem(*obj);
                        targetPlace->setMedia(list);
                        delete obj;
                        obj = NULL;
                    }
                }
            }
        } else {
            QPlaceMediaObject *obj = processAdContentMediaObject(value);
            if (obj) {
                QPlacePaginationList<QPlaceMediaObject> list = targetPlace->media();
                list.addItem(*obj);
                targetPlace->setMedia(list);
                delete obj;
                obj = NULL;
            }
        }
    }
}

QPlaceMediaObject *QPlaceJSonDetailsParser::processAdContentMediaObject(const QScriptValue &content)
{
    QPlaceMediaObject *obj = NULL;
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
        obj = new QPlaceMediaObject();
        obj->setUrl(mediaUrl);
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
            QPlaceBusinessInformation busInfo = targetPlace->businessInformation();
            busInfo.setPaymentMethods(list);
            targetPlace->setBusinessInformation(busInfo);
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
                        QPlaceBusinessInformation busInfo = targetPlace->businessInformation();
                        busInfo.setAnnualClosingNote(obj);
                        targetPlace->setBusinessInformation(busInfo);
                        //! @todo only one is used
                        break;
                    }
                }
            }
        } else {
            QString obj = processAdContentClosingsNote(value);
            if (!obj.isEmpty()) {
                QPlaceBusinessInformation busInfo = targetPlace->businessInformation();
                busInfo.setAnnualClosingNote(obj);
                targetPlace->setBusinessInformation(busInfo);
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
        QList<QPlaceWeekdayHours> list;
        if (value.isArray()) {
            QScriptValueIterator it(value);
            while (it.hasNext()) {
                it.next();
                // array contains count as last element
                if (it.name() != "length") {
                    QPlaceWeekdayHours *obj = processAdContentOpeningHoursElement(it.value());
                    if (obj) {
                        list.append(*obj);
                        delete obj;
                        obj = NULL;
                    }
                }
            }
        } else {
            QPlaceWeekdayHours *obj = processAdContentOpeningHoursElement(value);
            if (obj) {
                list.append(*obj);
                delete obj;
                obj = NULL;
            }
        }
        QPlaceBusinessInformation busInfo = targetPlace->businessInformation();
        busInfo.setOpeningHours(list);
        targetPlace->setBusinessInformation(busInfo);
    }
}

QPlaceWeekdayHours *QPlaceJSonDetailsParser::processAdContentOpeningHoursElement(const QScriptValue &content)
{
    QPlaceWeekdayHours *openH = new QPlaceWeekdayHours();
    QScriptValue value = content.property(place_adcontent_hours_open_day_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        QString day = value.toString();
        if (place_premiumcontent_content_monday == day) {
            openH->setWeekday(Qt::Monday);
        } else if (place_premiumcontent_content_tuesday == day) {
            openH->setWeekday(Qt::Tuesday);
        } else if (place_premiumcontent_content_wednesday == day) {
            openH->setWeekday(Qt::Wednesday);
        } else if (place_premiumcontent_content_thursday == day) {
            openH->setWeekday(Qt::Thursday);
        } else if (place_premiumcontent_content_friday == day) {
            openH->setWeekday(Qt::Friday);
        } else if (place_premiumcontent_content_saturday == day) {
            openH->setWeekday(Qt::Saturday);
        } else if (place_premiumcontent_content_sunday == day) {
            openH->setWeekday(Qt::Sunday);
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

    QTime startTime;
    startTime.setHMS(start.hour(), start.minute(), 0);
    QTime endTime;
    endTime.setHMS(end.hour(), end.minute(), 0);
    QPlacePeriod period;
    period.setStartTime(startTime);
    period.setEndTime(endTime);
    openH->setPeriod(period);
    return openH;
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
                        QPlaceBusinessInformation busInfo = targetPlace->businessInformation();
                        busInfo.setOpeningNote(obj);
                        targetPlace->setBusinessInformation(busInfo);
                        //! @todo only one is used
                        break;
                    }
                }
            }
        } else {
            QString obj = processAdContentOpeningNote(value);
            if (!obj.isEmpty()) {
                QPlaceBusinessInformation busInfo = targetPlace->businessInformation();
                busInfo.setOpeningNote(obj);
                targetPlace->setBusinessInformation(busInfo);
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
