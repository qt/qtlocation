#ifndef QDECLARATIVEPLACE_P_H
#define QDECLARATIVEPLACE_P_H

#include <QObject>
#include <QDeclarativeListProperty>
#include <qgeoplace.h>
#include "qdeclarativegeolocation_p.h"
#include "qdeclarativebusinessinformation_p.h"
#include "qdeclarativecategory_p.h"
#include "qdeclarativesupplier_p.h"
#include "qdeclarativecontact_p.h"
#include "qdeclarativerating_p.h"
#include "qdeclarativedescription_p.h"
#include "qdeclarativealternativevalue_p.h"
#include "qdeclarativemediapaginationlist_p.h"
#include "qdeclarativereviewpaginationlist_p.h"

QT_BEGIN_NAMESPACE

class QDeclarativePlace : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariantHash additionalData READ additionalData WRITE setAdditionalData NOTIFY additionalDataChanged);
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeAlternativeValue> alternativeNames READ alternativeNames NOTIFY alternativeNamesChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeCategory> categories READ categories NOTIFY categoriesChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeContact> contacts READ contacts NOTIFY contactsChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeDescription> descriptions READ descriptions NOTIFY descriptionsChanged)
    Q_PROPERTY(QDeclarativeGeoLocation* location READ location WRITE setLocation NOTIFY locationChanged);
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeGeoLocation> alternativeLocations READ alternativeLocations NOTIFY alternativeLocationsChanged)
    Q_PROPERTY(QDeclarativeBusinessInformation* businessInformation READ businessInformation WRITE setBusinessInformation NOTIFY businessInformationChanged);
    Q_PROPERTY(QDeclarativeRating* rating READ rating WRITE setRating NOTIFY ratingChanged);
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeSupplier> suppliers READ suppliers NOTIFY suppliersChanged)
    Q_PROPERTY(QStringList feeds READ feeds WRITE setFeeds NOTIFY feedsChanged);
    Q_PROPERTY(int mediaCount READ mediaCount WRITE setMediaCount NOTIFY mediaCountChanged);
    Q_PROPERTY(QDeclarativeMediaPaginationList* media READ media WRITE setMedia NOTIFY mediaChanged);
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged);
    Q_PROPERTY(QString placeId READ placeId WRITE setPlaceId NOTIFY placeIdChanged);
    Q_PROPERTY(int reviewCount READ reviewCount WRITE setReviewCount NOTIFY reviewCountChanged);
    Q_PROPERTY(QDeclarativeReviewPaginationList* reviews READ reviews WRITE setReviews NOTIFY reviewsChanged);
    Q_PROPERTY(QString shortDescription READ shortDescription WRITE setShortDescription NOTIFY shortDescriptionChanged);
    Q_PROPERTY(QStringList tags READ tags WRITE setTags NOTIFY tagsChanged);
    Q_PROPERTY(bool detailsFetched READ detailsFetched WRITE setDetailsFetched NOTIFY detailsFetchedChanged);

public:
    explicit QDeclarativePlace(QObject* parent = 0);
    explicit QDeclarativePlace(const QGeoPlace &src, QObject* parent = 0);
    ~QDeclarativePlace();

    QGeoPlace place();
    void setPlace(const QGeoPlace &src);

    QVariantHash additionalData() const;
    void setAdditionalData(const QVariantHash &data);
    QDeclarativeListProperty<QDeclarativeAlternativeValue> alternativeNames();
    static void alternativeValue_append(QDeclarativeListProperty<QDeclarativeAlternativeValue> *prop,
                                  QDeclarativeAlternativeValue* value);
    static int alternativeValue_count(QDeclarativeListProperty<QDeclarativeAlternativeValue> *prop);
    static QDeclarativeAlternativeValue* alternativeValue_at(QDeclarativeListProperty<QDeclarativeAlternativeValue> *prop, int index);
    static void alternativeValue_clear(QDeclarativeListProperty<QDeclarativeAlternativeValue> *prop);
    QDeclarativeListProperty<QDeclarativeCategory> categories();
    static void category_append(QDeclarativeListProperty<QDeclarativeCategory> *prop,
                                  QDeclarativeCategory* value);
    static int category_count(QDeclarativeListProperty<QDeclarativeCategory> *prop);
    static QDeclarativeCategory* category_at(QDeclarativeListProperty<QDeclarativeCategory> *prop, int index);
    static void category_clear(QDeclarativeListProperty<QDeclarativeCategory> *prop);
    QDeclarativeListProperty<QDeclarativeContact> contacts();
    static void contact_append(QDeclarativeListProperty<QDeclarativeContact> *prop,
                                  QDeclarativeContact* value);
    static int contact_count(QDeclarativeListProperty<QDeclarativeContact> *prop);
    static QDeclarativeContact* contact_at(QDeclarativeListProperty<QDeclarativeContact> *prop, int index);
    static void contact_clear(QDeclarativeListProperty<QDeclarativeContact> *prop);
    QDeclarativeListProperty<QDeclarativeDescription> descriptions();
    static void descriptions_append(QDeclarativeListProperty<QDeclarativeDescription> *prop,
                                  QDeclarativeDescription* value);
    static int descriptions_count(QDeclarativeListProperty<QDeclarativeDescription> *prop);
    static QDeclarativeDescription* descriptions_at(QDeclarativeListProperty<QDeclarativeDescription> *prop, int index);
    static void descriptions_clear(QDeclarativeListProperty<QDeclarativeDescription> *prop);
    QDeclarativeGeoLocation *location();
    void setLocation(QDeclarativeGeoLocation *location);
    QDeclarativeListProperty<QDeclarativeGeoLocation> alternativeLocations();
    static void alternativeLocations_append(QDeclarativeListProperty<QDeclarativeGeoLocation> *prop,
                                  QDeclarativeGeoLocation* value);
    static int alternativeLocations_count(QDeclarativeListProperty<QDeclarativeGeoLocation> *prop);
    static QDeclarativeGeoLocation* alternativeLocations_at(QDeclarativeListProperty<QDeclarativeGeoLocation> *prop, int index);
    static void alternativeLocations_clear(QDeclarativeListProperty<QDeclarativeGeoLocation> *prop);
    QDeclarativeRating *rating();
    void setRating(QDeclarativeRating *rating);
    QDeclarativeListProperty<QDeclarativeSupplier> suppliers();
    static void suppliers_append(QDeclarativeListProperty<QDeclarativeSupplier> *prop,
                                  QDeclarativeSupplier* value);
    static int suppliers_count(QDeclarativeListProperty<QDeclarativeSupplier> *prop);
    static QDeclarativeSupplier* suppliers_at(QDeclarativeListProperty<QDeclarativeSupplier> *prop, int index);
    static void suppliers_clear(QDeclarativeListProperty<QDeclarativeSupplier> *prop);
    QStringList feeds() const;
    void setFeeds(const QStringList &feeds);
    int mediaCount() const;
    void setMediaCount(const int &data);
    QDeclarativeMediaPaginationList *media();
    void setMedia(QDeclarativeMediaPaginationList *media);
    QString name() const;
    void setName(const QString &name);
    QString placeId() const;
    void setPlaceId(const QString &placeId);
    QDeclarativeBusinessInformation *businessInformation();
    void setBusinessInformation(QDeclarativeBusinessInformation *business);
    int reviewCount() const;
    void setReviewCount(const int &data);
    QDeclarativeReviewPaginationList *reviews();
    void setReviews(QDeclarativeReviewPaginationList *reviews);
    QString shortDescription() const;
    void setShortDescription(const QString &description);
    QStringList tags() const;
    void setTags(const QStringList &tags);
    bool detailsFetched() const;
    void setDetailsFetched(bool fetched);

signals:
    void additionalDataChanged();
    void alternativeNamesChanged();
    void categoriesChanged();
    void contactsChanged();
    void descriptionsChanged();
    void locationChanged();
    void alternativeLocationsChanged();
    void ratingChanged();
    void suppliersChanged();
    void feedsChanged();
    void mediaCountChanged();
    void mediaChanged();
    void nameChanged();
    void placeIdChanged();
    void businessInformationChanged();
    void reviewCountChanged();
    void reviewsChanged();
    void shortDescriptionChanged();
    void tagsChanged();
    void detailsFetchedChanged();

private:
    void synchronizeAlternativeValues();
    void synchronizeCategories();
    void synchronizeContacts();
    void synchronizeDescriptions();
    void synchronizeAlternativeLocations();
    void synchronizeSuppliers();

private:
    QList<QDeclarativeAlternativeValue*> m_alternativeValues;
    QList<QDeclarativeCategory*> m_categories;
    QList<QDeclarativeContact*> m_contacts;
    QList<QDeclarativeDescription*> m_descriptions;
    QDeclarativeGeoLocation m_location;
    QList<QDeclarativeGeoLocation*> m_alternativeLocations;
    QDeclarativeRating m_rating;
    QList<QDeclarativeSupplier*> m_suppliers;
    QDeclarativeMediaPaginationList m_mediaList;
    QDeclarativeReviewPaginationList m_reviewList;
    QDeclarativeBusinessInformation m_businessInformation;

    QGeoPlace m_src;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativePlace));

#endif // QDECLARATIVEPLACE_P_H
