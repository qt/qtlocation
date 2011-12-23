/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qplacejsondetailsparser.h"

#include <QtCore/QUrl>
#include <QtCore/QTime>
#include <QJSEngine>
#include <QJSValue>
#include <QJSValueIterator>
#include <QtLocation/QPlaceContactDetail>

#include <qgeoaddress.h>
#include <qgeocoordinate.h>
#include <qplace.h>
#include <qplacecategory.h>
#include <qplaceeditorial.h>
#include <qplaceratings.h>
#include <qgeolocation.h>
#include <qplacesupplier.h>
#include <qplacemanager.h>
#include "qplacejsoncategoriesparser.h"
#include "qplacesuppliersrepository.h"

#if defined(QT_PLACES_LOGGING)
    #include <QDebug>
#endif

//search const
static const char *place_place_element = "place";
static const char *place_id_element = "a_id";
/* currently unused:
static const char *place_is_ad_place_value = "isAdPlace";
*/

static const char *place_provider = "provider";
static const char *place_provider_url = "providerUrl";

static const char *place_categories_element = "categories";

static const char *place_contact_element = "contact";
static const char *place_contact_email_element = "email";
static const char *place_contact_fax_element = "fax";
static const char *place_contact_phone_element = "phone";
static const char *place_contact_website_element = "website";
/* currently unused:
static const char *place_contact_im_element = "im";
*/

static const char *place_tags_element = "tags";
static const char *place_tags_value_element = "value";

static const char *place_names_element = "names";
static const char *place_alternativenames_element = "alternativeNames";
static const char *place_name_default_element = "defaultName";
static const char *place_name_localized_element = "localizedName";
static const char *place_name_value_element = "name";
/* currently unused:
static const char *place_name_language_element = "language";
*/

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
/* currently unused:
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
*/

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

QPlaceJSonDetailsParser::QPlaceJSonDetailsParser(QPlaceManager *manager, QObject *parent) :
    QPlaceJSonParser(parent), m_manager(manager)
{
}

QPlaceJSonDetailsParser::~QPlaceJSonDetailsParser()
{
}

QPlace QPlaceJSonDetailsParser::buildPlace(const QJSValue &placeValue, QPlaceManager *manager)
{
    QPlace newPlace;
    if (placeValue.isValid()) {
        buildPlace(placeValue, &newPlace);
    }

    if (!newPlace.icon().baseUrl().isEmpty() || !newPlace.icon().fullUrl().isEmpty()) {
        QPlaceIcon icon = newPlace.icon();
        icon.setManager(manager);
        newPlace.setIcon(icon);
    }

    QPlaceAttribute managerName;
    managerName.setText(manager->managerName());

    newPlace.setExtendedAttribute(QLatin1String("x_provider"), managerName);

    return newPlace;
}

QPlace QPlaceJSonDetailsParser::result()
{
    return place;
}

void QPlaceJSonDetailsParser::processJSonData(const QJSValue &sv)
{
    if (sv.isValid()) {
        QJSValue placeProperty = sv.property(place_place_element);
        if (placeProperty.isValid()) {
            place = buildPlace(placeProperty, m_manager);
            emit finished(NoError, QString());
        } else {
            emit finished(ParsingError, QString("JSON data are invalid"));
        }
    } else {
        emit finished(ParsingError, QString("JSON data are invalid"));
    }
}

void QPlaceJSonDetailsParser::buildPlace(const QJSValue &placeValue, QPlace *targetPlace)
{
    if (placeValue.isValid()) {
        QJSValue value = placeValue.property(place_id_element);
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

void QPlaceJSonDetailsParser::processMainProvider(const QJSValue &placeValue, QPlace *targetPlace)
{
    QPlaceSupplier sup;
    QJSValue value = placeValue.property(place_provider);
    if (value.isValid() && !value.toString().isEmpty()) {
        sup.setName(value.toString());
    }
    else {
        return;
    }
    value = placeValue.property(place_provider_url);
    if (value.isValid() && !value.toString().isEmpty()){
        QPlaceIcon icon;
        icon.setBaseUrl(value.toString());
        //Note: the icon manager is set in QPlaceJSonDetailsParser::buildPlace()
        sup.setIcon(icon);
    }

    targetPlace->setSupplier(QPlaceSuppliersRepository::instance()->addSupplier(sup));
}

void QPlaceJSonDetailsParser::processContacts(const QJSValue &contactsValue, QPlace *targetPlace)
{
    QJSValueIterator it(contactsValue);
    QPlaceContactDetail contactDetail;
    while (it.hasNext()) {
        it.next();
        if (it.name() == place_contact_website_element) {
            contactDetail.setLabel(tr("Website"));
            contactDetail.setValue(it.value().toString());
            targetPlace->appendContactDetail(QPlaceContactDetail::Website, contactDetail);
        } else if (it.name() == place_contact_phone_element) {
            contactDetail.setLabel(tr("Phone"));
            contactDetail.setValue(it.value().toString());
            targetPlace->appendContactDetail(QPlaceContactDetail::Phone, contactDetail);
        } else if (it.name() == place_contact_fax_element) {
            contactDetail.setLabel(tr("Fax"));
            contactDetail.setValue(it.value().toString());
            targetPlace->appendContactDetail(QPlaceContactDetail::Fax, contactDetail);
        } else if (it.name() == place_contact_email_element) {
            contactDetail.setLabel(tr("Email"));
            contactDetail.setValue(it.value().toString());
            targetPlace->appendContactDetail(QPlaceContactDetail::Email, contactDetail);
        } else {
            //unknown contact element, do nothing
        }
    }
}

void QPlaceJSonDetailsParser::processCategories(const QJSValue &categories, QPlace *targetPlace)
{
    targetPlace->setCategories(QPlaceJSonCategoriesParser::parseFlatCategoryList(categories));
}

void QPlaceJSonDetailsParser::processRatings(const QJSValue &ratings, QPlace *targetPlace)
{
    QPlaceRatings *rating = NULL;
    QJSValue value = ratings.property(place_rating_element);
    if (value.isValid()) {
        if (value.isArray()) {
            QJSValueIterator it(value);
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
        targetPlace->setRatings(*rating);
        delete rating;
        rating = NULL;
    }
}

QPlaceRatings *QPlaceJSonDetailsParser::processRating(const QJSValue &ratingElement)
{
    QPlaceRatings *rating = NULL;
    QJSValue value = ratingElement.property(place_rating_type_element);
    // Only overall elements are interesting
    if (value.isValid() && value.toString() == place_rating_type_overall_element) {
        rating = new QPlaceRatings();
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
                rating->setAverage(ratingValue);
            }
        }
    }
    return rating;
}

void QPlaceJSonDetailsParser::processAddress(const QJSValue &address, QGeoLocation *location)
{
    QGeoAddress newAddress;
    QJSValue value = address.property(place_address_street);
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
        newAddress.setPostalCode(value.toString());
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

void QPlaceJSonDetailsParser::processLocation(const QJSValue &location, QPlace *targetPlace)
{
    QGeoLocation newLocation;
    QJSValue property = location.property(place_geoCoordinates_element);
    if (property.isValid()) {
        QGeoCoordinate pos;
        QJSValue value = property.property(place_geoCoordinates_longitude_value);
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

void QPlaceJSonDetailsParser::processTags(const QJSValue &tags, QPlace *targetPlace)
{
    Q_UNUSED(targetPlace)

    QStringList newTags;
    if (tags.isValid()) {
        if (tags.isArray()) {
            QJSValueIterator it(tags);
            while (it.hasNext()) {
                it.next();
                QJSValue value = it.value().property(place_tags_value_element);
                if (value.isValid() && !value.toString().isEmpty()) {
                    newTags.append(value.toString());
                }
            }
        } else {
            QJSValue value = tags.property(place_tags_value_element);
            if (value.isValid() && !value.toString().isEmpty()) {
                newTags.append(value.toString());
            }
        }
    }

    // The JSON data specification defines a tags element, but our API doesn't expose tags so we
    // parse and skip assignment.
}

void QPlaceJSonDetailsParser::processNames(const QJSValue &names, QPlace *targetPlace)
{
    QJSValue value = names.property(place_alternativenames_element);
    if (value.isValid()) {
        value = value.property(place_name_localized_element);
        if (value.isValid()) {
            QStringList list;
            if (value.isArray()) {
                QJSValueIterator it(value);
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

QString QPlaceJSonDetailsParser::processName(const QJSValue &nameValue)
{
    QString name;
    QJSValue value = nameValue.property(place_name_value_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        name = value.toString();
    }
    return name;
}

void QPlaceJSonDetailsParser::processPremiumContents(const QJSValue &premiumContent, QPlace *targetPlace)
{
    QJSValue value = premiumContent.property(place_premiumcontent_version_element);
    if (value.isValid()) {
        if (value.isArray()) {
            QJSValueIterator it(value);
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

void QPlaceJSonDetailsParser::processPremiumVersion(const QJSValue &content, QPlace *targetPlace)
{
    QJSValue value = content.property(place_premiumcontent_content_element);
    if (value.isValid()) {
        if (value.isArray()) {
            QJSValueIterator it(value);
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

void QPlaceJSonDetailsParser::processPremiumContent(const QJSValue &content, QPlace *targetPlace)
{
    QPlaceSupplier supplier;

    QJSValue value = content.property(place_premiumcontent_content_providername_element);
    if (value.isValid() && !value.toString().isEmpty())
        supplier.setName(value.toString());

    value = content.property(place_premiumcontent_content_provider_element);
    if (value.isValid() && !value.toString().isEmpty())
        supplier.setSupplierId(value.toString());

    value = content.property(place_premiumcontent_content_providerIconUrl_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        QPlaceIcon icon;
        icon.setFullUrl(QUrl::fromEncoded(value.toString().toAscii()));
        //note: the icon manager is set in QPlaceJSonDetailsParser::buildPlace()
        supplier.setIcon(icon);
    }

    if (!supplier.supplierId().isEmpty())
        supplier = QPlaceSuppliersRepository::instance()->addSupplier(supplier);

    processPremiumContentDescription(content, supplier, targetPlace);
    processPremiumContentMediaObjects(content, supplier, targetPlace);
}

void QPlaceJSonDetailsParser::processPremiumContentDescription(const QJSValue &content,
                                                               const QPlaceSupplier &supplier,
                                                               QPlace *targetPlace)
{
    QJSValue value = content.property(place_premiumcontent_content_desc_element);
    QPlaceEditorial desc;
    if (value.isValid() && !value.toString().isEmpty()) {
        desc.setText(value.toString());
    } else {
        value = content.property(place_premiumcontent_content_shortdesc_element);
        if (value.isValid() && !value.toString().isEmpty()) {
            desc.setText(value.toString());
        }
    }
    // if no description do not continue
    if (desc.text().isEmpty()) {
        return;
    }
    value = content.property(place_premiumcontent_content_name_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        desc.setTitle(value.toString());
    }
    value = content.property(place_premiumcontent_content_vendorurl_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        QPlaceSupplier supplier = desc.supplier();
        supplier.setUrl(value.toString());
        desc.setSupplier(supplier);
    }
    value = content.property(place_premiumcontent_content_language_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        desc.setLanguage(value.toString());
    }
    desc.setSupplier(supplier);

    QPlaceContent::Collection editorials = targetPlace->content(QPlaceContent::EditorialType);

    int insertionIndex = 0;
    if (!editorials.isEmpty())
        insertionIndex = editorials.keys().last() + 1;

    editorials.insert(insertionIndex, desc);
    targetPlace->setContent(QPlaceContent::EditorialType, editorials);
}

void QPlaceJSonDetailsParser::processPremiumContentMediaObjects(const QJSValue &content,
                                                                const QPlaceSupplier &supplier,
                                                                QPlace *targetPlace)
{
    QJSValue value = content.property(place_premiumcontent_content_media_element);
    if (value.isValid()) {
        if (value.isArray()) {
            QJSValueIterator it(value);
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

QPlaceImage *QPlaceJSonDetailsParser::processPremiumContentMediaObject(const QJSValue &content)
{
    QPlaceImage *obj = NULL;
    QJSValue value = content.property(place_premiumcontent_content_mediaurl_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        obj = new QPlaceImage();
        obj->setUrl(QUrl::fromEncoded(value.toString().toAscii()));
        obj->setImageId(value.toString());
        value = content.property(place_premiumcontent_content_mediamimetype_element);
        if (value.isValid() && !value.toString().isEmpty()) {
            obj->setMimeType(value.toString());
        }
    }
    return obj;
}

void QPlaceJSonDetailsParser::processAdContent(const QJSValue &content, QPlace *targetPlace)
{
    QJSValue value = content.property(place_adcontent_descriptions_element);
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
}

void QPlaceJSonDetailsParser::processAdContentDescriptions(const QJSValue &content, QPlace *targetPlace)
{
    QJSValue value = content.property(place_adcontent_description_element);
    if (value.isValid()) {
        if (value.isArray()) {
            QJSValueIterator it(value);

            QPlaceContent::Collection editorials =
                targetPlace->content(QPlaceContent::EditorialType);

            int insertionIndex = 0;
            if (!editorials.isEmpty())
                insertionIndex = editorials.keys().last() + 1;

            while (it.hasNext()) {
                it.next();
                // array contains count as last element
                if (it.name() != "length") {
                    QPlaceEditorial *des = processAdContentDescription(it.value());
                    if (des) {
                        editorials.insert(insertionIndex, *des);
                        ++insertionIndex;
                        delete des;
                    }
                }
            }
            targetPlace->setContent(QPlaceContent::EditorialType, editorials);
        } else {
            QPlaceEditorial *des = processAdContentDescription(value);
            if (des) {
                QPlaceContent::Collection editorials =
                    targetPlace->content(QPlaceContent::EditorialType);

                int insertionIndex = 0;
                if (!editorials.isEmpty())
                    insertionIndex = editorials.keys().last() + 1;

                editorials.insert(insertionIndex, *des);
                targetPlace->setContent(QPlaceContent::EditorialType, editorials);
                delete des;
            }
        }
    }
}

QPlaceEditorial *QPlaceJSonDetailsParser::processAdContentDescription(const QJSValue &content)
{
    QPlaceEditorial *description = NULL;
    QJSValue value = content.property(place_adcontent_localizedDescription_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        description = new QPlaceEditorial();
        description->setText(value.toString());
        value = content.property(place_adcontent_descriptionLanguage_element);
        if (value.isValid() && !value.toString().isEmpty()) {
            description->setLanguage(value.toString());
        }
    }
    return description;
}

void QPlaceJSonDetailsParser::processAdContentMediaObjects(const QJSValue &content, QPlace *targetPlace)
{
    QJSValue value = content.property(place_adcontent_media_element);
    if (value.isValid()) {
        if (value.isArray()) {
            QJSValueIterator it(value);
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

QPlaceImage *QPlaceJSonDetailsParser::processAdContentMediaObject(const QJSValue &content)
{
    QPlaceImage *obj = NULL;
    QString mediaMimeType;
    QString mediaUrl;

    QJSValue value = content.property(place_adcontent_media_mime_element);
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
        obj->setImageId(mediaUrl);
        obj->setMimeType(mediaMimeType);
    }
    return obj;
}

void QPlaceJSonDetailsParser::processAdContentPaymentMethods(const QJSValue &content, QPlace *targetPlace)
{
    QJSValue value = content.property(place_adcontent_paymentmethod_element);
    if (value.isValid()) {
        QStringList list;
        if (value.isArray()) {
            QJSValueIterator it(value);
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
            paymentMethods.setLabel(tr("Payment methods"));
            paymentMethods.setText(list.join(","));
            targetPlace->setExtendedAttribute(QLatin1String("paymentMethods"), paymentMethods);
        }
    }
}

QString QPlaceJSonDetailsParser::processAdContentPaymentMethod(const QJSValue &content)
{
    QString obj;
    QJSValue value = content.property(place_adcontent_paymentmethod_name_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        obj = value.toString();
    }
    return obj;
}

void QPlaceJSonDetailsParser::processAdContentBusinessHours(const QJSValue &content, QPlace *targetPlace)
{
    QJSValue value = content.property(place_adcontent_hours_annualclosingsnotes_element);
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

void QPlaceJSonDetailsParser::processAdContentClosingsNotes(const QJSValue &content,
                                                            QPlace *targetPlace)
{
    Q_UNUSED(targetPlace)

    QJSValue value = content.property(place_adcontent_hours_annualclosingsnote_element);
    if (value.isValid()) {
        if (value.isArray()) {
            QJSValueIterator it(value);
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

QString QPlaceJSonDetailsParser::processAdContentClosingsNote(const QJSValue &content)
{
    QString ret;
    QJSValue value = content.property(place_adcontent_hours_annualclosingsnote_v_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        ret = value.toString();
    }
    return ret;
}

void QPlaceJSonDetailsParser::processAdContentOpeningHours(const QJSValue &content,
                                                           QPlace *targetPlace)
{
    Q_UNUSED(targetPlace)

    QJSValue value = content.property(place_adcontent_hours_open_hours_element);
    if (value.isValid()) {
        if (value.isArray()) {
            QJSValueIterator it(value);
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

void QPlaceJSonDetailsParser::processAdContentOpeningHoursElement(const QJSValue &content)
{
    //The JSON data specification defines
    //structured opening hour elements
    //but our API doesn't expose this so we
    //parse but do nothing else

    QJSValue value = content.property(place_adcontent_hours_open_day_element);
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

void QPlaceJSonDetailsParser::processAdContentOpeningNotes(const QJSValue &content, QPlace *targetPlace)
{
    QJSValue value = content.property(place_adcontent_hours_openingnote_element);
    if (value.isValid()) {
        if (value.isArray()) {
            QJSValueIterator it(value);
            while (it.hasNext()) {
                it.next();
                // array contains count as last element
                if (it.name() != "length") {
                    QString obj = processAdContentOpeningNote(it.value());
                    if (!obj.isEmpty()) {
                        QPlaceAttribute openingNote;
                        openingNote.setLabel(tr("Opening note"));
                        openingNote.setText(obj);
                        targetPlace->setExtendedAttribute(QLatin1String("openingNote"), openingNote);
                        //! @todo only one is used
                        break;
                    }
                }
            }
        } else {
            QString obj = processAdContentOpeningNote(value);
            if (!obj.isEmpty()) {
                QPlaceAttribute openingNote;
                openingNote.setLabel(tr("Opening note"));
                openingNote.setText(obj);
                targetPlace->setExtendedAttribute(QLatin1String("openingNote"), openingNote);
            }
        }
    }
}

QString QPlaceJSonDetailsParser::processAdContentOpeningNote(const QJSValue &content)
{
    QString ret;
    QJSValue value = content.property(place_adcontent_hours_openingnote_v_element);
    if (value.isValid() && !value.toString().isEmpty()) {
        ret = value.toString();
    }
    return ret;
}
