#include "qdeclarativeplace_p.h"

QT_USE_NAMESPACE

/*!
    \qmlclass Place

    \brief The Place element holds place data.
    \inherits QObject

    Place cointains many properties holding data of the place like location,
    id, contacts etc.

    \ingroup qml-places
*/

QDeclarativePlace::QDeclarativePlace(QObject* parent)
        : QObject(parent)
{
}

QDeclarativePlace::QDeclarativePlace(const QGeoPlace &src,
        QObject *parent)
        : QObject(parent),
          m_src(src)
{
    synchronizeCategories();
    synchronizeContacts();
    synchronizeDescriptions();
    synchronizeSuppliers();
    m_rating.setRating(m_src.rating());
    m_location.setLocation(m_src.location());
    m_businessInformation.setBusinessInformation(m_src.businessInformation());
}

QDeclarativePlace::~QDeclarativePlace()
{
}

void QDeclarativePlace::setPlace(const QGeoPlace &src)
{
    QGeoPlace previous = m_src;
    m_src = src;

    if (previous.additionalData() != m_src.additionalData()) {
        emit additionalDataChanged();
    }
    if (previous.businessInformation() != m_src.businessInformation()) {
        m_businessInformation.setBusinessInformation(m_src.businessInformation());
        emit businessInformationChanged();
    }
    if (previous.categories() != m_src.categories()) {
        synchronizeCategories();
        emit categoriesChanged();
    }
    if (previous.contacts() != m_src.contacts()) {
        synchronizeContacts();
        emit contactsChanged();
    }
    if (previous.descriptions() != m_src.descriptions()) {
        synchronizeDescriptions();
        emit descriptionsChanged();
    }
    if (previous.location() != m_src.location()) {
        m_location.setLocation(src.location());
        emit locationChanged();
    }
    if (previous.rating() != m_src.rating()) {
        m_rating.setRating(src.rating());
        emit ratingChanged();
    }
    if (previous.suppliers() != m_src.suppliers()) {
        synchronizeSuppliers();
        emit suppliersChanged();
    }
    if (previous.feeds() != m_src.feeds()) {
        emit feedsChanged();
    }
    if (previous.mediaCount() != m_src.mediaCount()) {
        emit mediaCountChanged();
    }
    if (previous.media() != m_src.media()) {
        m_mediaList.setPaginationList(m_src.media());
        emit mediaChanged();
    }
    if (previous.name() != m_src.name()) {
        emit nameChanged();
    }
    if (previous.placeId() != m_src.placeId()) {
        emit placeIdChanged();
    }
    if (previous.reviewCount() != m_src.reviewCount()) {
        emit reviewCountChanged();
    }
    if (previous.reviews() != m_src.reviews()) {
        m_reviewList.setPaginationList(m_src.reviews());
        emit reviewsChanged();
    }
    if (previous.shortDescription() != m_src.shortDescription()) {
        emit shortDescriptionChanged();
    }
    if (previous.tags() != m_src.tags()) {
        emit tagsChanged();
    }
    if (previous.detailsFetched() != m_src.detailsFetched()) {
        emit detailsFetchedChanged();
    }
}

QGeoPlace QDeclarativePlace::place()
{
    QList<QPlaceCategory> categories;
    foreach (QDeclarativeCategory *value, m_categories) {
        categories.append(value->category());
    }
    m_src.setCategories(categories);
    QList<QPlaceContact> contacts;
    foreach (QDeclarativeContact *value, m_contacts) {
        contacts.append(value->contact());
    }
    m_src.setContacts(contacts);
    QList<QPlaceDescription> descriptions;
    foreach (QDeclarativeDescription *value, m_descriptions) {
        descriptions.append(value->description());
    }
    m_src.setDescriptions(descriptions);
    m_src.setLocation(m_location.location());
    m_src.setRating(m_rating.rating());
    QList<QPlaceSupplier> suppliers;
    foreach (QDeclarativeSupplier *value, m_suppliers) {
        suppliers.append(value->supplier());
    }
    m_src.setSuppliers(suppliers);
    m_src.setMedia(m_mediaList.paginationList());
    m_src.setReviews(m_reviewList.paginationList());
    m_src.setBusinessInformation(m_businessInformation.businessInformation());
    return m_src;
}

/*!
    \qmlproperty QVariantHash Place::additionalData

    This property holds additional data for place. Those are pairs of strings (key/value).
*/

void QDeclarativePlace::setAdditionalData(const QVariantHash &additionalData)
{
    if (m_src.additionalData() != additionalData) {
        m_src.setAdditionalData(additionalData);
        emit additionalDataChanged();
    }
}

QVariantHash QDeclarativePlace::additionalData() const
{
    return m_src.additionalData();
}

/*!
    \qmlproperty string Place::location

    This property holds location of the place.

    Note: this property's changed() signal is currently emitted only if the
    whole element changes, not if only the contents of the element change.
*/
void QDeclarativePlace::setLocation(QDeclarativeGeoLocation *location)
{
    if (m_src.location() != location->location()) {
        m_location.setLocation(location->location());
        m_src.setLocation(location->location());
        emit locationChanged();
    }
}

QDeclarativeGeoLocation *QDeclarativePlace::location()
{
    return &m_location;
}

/*!
    \qmlproperty Rating Place::rating

    This property holds rating of the place.

    Note: this property's changed() signal is currently emitted only if the
    whole element changes, not if only the contents of the element change.
*/
void QDeclarativePlace::setRating(QDeclarativeRating *obj)
{
    if (m_src.rating() != obj->rating()) {
        m_rating.setRating(obj->rating());
        m_src.setRating(obj->rating());
        emit ratingChanged();
    }
}

QDeclarativeRating *QDeclarativePlace::rating()
{
    return &m_rating;
}

/*!
    \qmlproperty BusinessInformation Place::businessInformation

    This property holds business information of the place.

    Note: this property's changed() signal is currently emitted only if the
    whole element changes, not if only the contents of the element change.
*/
void QDeclarativePlace::setBusinessInformation(QDeclarativeBusinessInformation *obj)
{
    if (m_src.businessInformation() != obj->businessInformation()) {
        m_businessInformation.setBusinessInformation(obj->businessInformation());
        m_src.setBusinessInformation(obj->businessInformation());
        emit businessInformationChanged();
    }
}

QDeclarativeBusinessInformation *QDeclarativePlace::businessInformation()
{
    return &m_businessInformation;
}

/*!
    \qmlproperty string Place::name

    This property holds name.
*/
void QDeclarativePlace::setName(const QString &name)
{
    if (m_src.name() != name) {
        m_src.setName(name);
        emit nameChanged();
    }
}

QString QDeclarativePlace::name() const
{
    return m_src.name();
}

/*!
    \qmlproperty string Place::placeId

    This property holds place id.
*/
void QDeclarativePlace::setPlaceId(const QString &placeId)
{
    if (m_src.placeId() != placeId) {
        m_src.setPlaceId(placeId);
        emit placeIdChanged();
    }
}

QString QDeclarativePlace::placeId() const
{
    return m_src.placeId();
}

/*!
    \qmlproperty string Place::shortDescription

    This property holds short description.
*/
void QDeclarativePlace::setShortDescription(const QString &shortDescription)
{
    if (m_src.shortDescription() != shortDescription) {
        m_src.setShortDescription(shortDescription);
        emit shortDescriptionChanged();
    }
}

QString QDeclarativePlace::shortDescription() const
{
    return m_src.shortDescription();
}

/*!
    \qmlproperty int Place::mediaCount

    This property holds number of all media.
*/
void QDeclarativePlace::setMediaCount(const int &mediaCount)
{
    if (m_src.mediaCount() != mediaCount) {
        m_src.setMediaCount(mediaCount);
        emit mediaCountChanged();
    }
}

int QDeclarativePlace::mediaCount() const
{
    return m_src.mediaCount();
}

/*!
    \qmlproperty string Place::media

    This property holds media list of the place.

    Note: this property's changed() signal is currently emitted only if the
    whole element changes, not if only the contents of the element change.
*/
void QDeclarativePlace::setMedia(QDeclarativeMediaPaginationList *obj)
{
    if (m_src.media() != obj->paginationList()) {
        m_mediaList.setPaginationList(obj->paginationList());
        m_src.setMedia(obj->paginationList());
        emit mediaChanged();
    }
}

QDeclarativeMediaPaginationList *QDeclarativePlace::media()
{
    return &m_mediaList;
}

/*!
    \qmlproperty stringlist Place::tags

    This property holds tag list.
*/
void QDeclarativePlace::setTags(const QStringList &tags)
{
    if (m_src.tags() != tags) {
        m_src.setTags(tags);
        emit tagsChanged();
    }
}

QStringList QDeclarativePlace::tags() const
{
    return m_src.tags();
}

/*!
    \qmlproperty qint64 Place::detailsFetched()

    This property holds a boolean indicating
    whether the details of the place have been fetched
    or not.
*/
void QDeclarativePlace::setDetailsFetched(bool fetched)
{
    if (m_src.detailsFetched() != fetched) {
        m_src.setDetailsFetched(fetched);
        emit detailsFetchedChanged();
    }
}

bool QDeclarativePlace::detailsFetched() const
{
    return m_src.detailsFetched();
}

/*!
    \qmlproperty stringlist Place::feeds

    This property holds feeds list.
*/
void QDeclarativePlace::setFeeds(const QStringList &feeds)
{
    if (m_src.feeds() != feeds) {
        m_src.setFeeds(feeds);
        emit feedsChanged();
    }
}

QStringList QDeclarativePlace::feeds() const
{
    return m_src.feeds();
}

/*!
    \qmlproperty int Place::reviewCount

    This property holds number of all reviews.

    Note: this property's changed() signal is currently emitted only if the
    whole element changes, not if only the contents of the element change.
*/
void QDeclarativePlace::setReviewCount(const int &reviewCount)
{
    if (m_src.reviewCount() != reviewCount) {
        m_src.setReviewCount(reviewCount);
        emit reviewCountChanged();
    }
}

int QDeclarativePlace::reviewCount() const
{
    return m_src.reviewCount();
}

/*!
    \qmlproperty string Place::reviews

    This property holds reviews list of the place.
*/
void QDeclarativePlace::setReviews(QDeclarativeReviewPaginationList *obj)
{
    if (m_src.reviews() != obj->paginationList()) {
        m_reviewList.setPaginationList(obj->paginationList());
        m_src.setReviews(obj->paginationList());
        emit reviewsChanged();
    }
}

QDeclarativeReviewPaginationList *QDeclarativePlace::reviews()
{
    return &m_reviewList;
}

/*!
    \qmlproperty QDeclarativeListProperty<QDeclarativeCategory> Place::categories

    This property categories list.

    Note: this property's changed() signal is currently emitted only if the
    whole element changes, not if only the contents of the element change.
*/
QDeclarativeListProperty<QDeclarativeCategory> QDeclarativePlace::categories()
{
    return QDeclarativeListProperty<QDeclarativeCategory>(this,
                                                          0, // opaque data parameter
                                                          category_append,
                                                          category_count,
                                                          category_at,
                                                          category_clear);
}

void QDeclarativePlace::category_append(QDeclarativeListProperty<QDeclarativeCategory> *prop,
                                                  QDeclarativeCategory *value)
{
    QDeclarativePlace* object = static_cast<QDeclarativePlace*>(prop->object);
    QDeclarativeCategory *altValue = new QDeclarativeCategory(object);
    altValue->setCategory(value->category());
    object->m_categories.append(altValue);
    QList<QPlaceCategory> list = object->m_src.categories();
    list.append(value->category());
    object->m_src.setCategories(list);
    emit object->categoriesChanged();
}

int QDeclarativePlace::category_count(QDeclarativeListProperty<QDeclarativeCategory> *prop)
{
    return static_cast<QDeclarativePlace*>(prop->object)->m_categories.count();
}

QDeclarativeCategory* QDeclarativePlace::category_at(QDeclarativeListProperty<QDeclarativeCategory> *prop,
                                                                          int index)
{
    QDeclarativePlace* object = static_cast<QDeclarativePlace*>(prop->object);
    QDeclarativeCategory *res = NULL;
    if (object->m_categories.count() > index && index > -1) {
        res = object->m_categories[index];
    }
    return res;
}

void QDeclarativePlace::category_clear(QDeclarativeListProperty<QDeclarativeCategory> *prop)
{
    QDeclarativePlace* object = static_cast<QDeclarativePlace*>(prop->object);
    qDeleteAll(object->m_categories);
    object->m_categories.clear();
    object->m_src.setCategories(QList<QPlaceCategory>());
    emit object->categoriesChanged();
}

void QDeclarativePlace::synchronizeCategories()
{
    qDeleteAll(m_categories);
    m_categories.clear();
    foreach (QPlaceCategory value, m_src.categories()) {
        QDeclarativeCategory* declarativeValue = new QDeclarativeCategory(value, this);
        m_categories.append(declarativeValue);
    }
}

/*!
    \qmlproperty QDeclarativeListProperty<QDeclarativeContact> Place::contacts

    This property contacts list.

    Note: this property's changed() signal is currently emitted only if the
    whole element changes, not if only the contents of the element change.
*/
QDeclarativeListProperty<QDeclarativeContact> QDeclarativePlace::contacts()
{
    return QDeclarativeListProperty<QDeclarativeContact>(this,
                                                          0, // opaque data parameter
                                                          contact_append,
                                                          contact_count,
                                                          contact_at,
                                                          contact_clear);
}

void QDeclarativePlace::contact_append(QDeclarativeListProperty<QDeclarativeContact> *prop,
                                                  QDeclarativeContact *value)
{
    QDeclarativePlace* object = static_cast<QDeclarativePlace*>(prop->object);
    QDeclarativeContact *altValue = new QDeclarativeContact(object);
    altValue->setContact(value->contact());
    object->m_contacts.append(altValue);
    QList<QPlaceContact> list = object->m_src.contacts();
    list.append(value->contact());
    object->m_src.setContacts(list);
    emit object->contactsChanged();
}

int QDeclarativePlace::contact_count(QDeclarativeListProperty<QDeclarativeContact> *prop)
{
    return static_cast<QDeclarativePlace*>(prop->object)->m_contacts.count();
}

QDeclarativeContact* QDeclarativePlace::contact_at(QDeclarativeListProperty<QDeclarativeContact> *prop,
                                                                          int index)
{
    QDeclarativePlace* object = static_cast<QDeclarativePlace*>(prop->object);
    QDeclarativeContact *res = NULL;
    if (object->m_contacts.count() > index && index > -1) {
        res = object->m_contacts[index];
    }
    return res;
}

void QDeclarativePlace::contact_clear(QDeclarativeListProperty<QDeclarativeContact> *prop)
{
    QDeclarativePlace* object = static_cast<QDeclarativePlace*>(prop->object);
    qDeleteAll(object->m_contacts);
    object->m_contacts.clear();
    object->m_src.setContacts(QList<QPlaceContact>());
    emit object->contactsChanged();
}

void QDeclarativePlace::synchronizeContacts()
{
    qDeleteAll(m_contacts);
    m_contacts.clear();
    foreach (QPlaceContact value, m_src.contacts()) {
        QDeclarativeContact* declarativeValue = new QDeclarativeContact(value, this);
        m_contacts.append(declarativeValue);
    }
}

/*!
    \qmlproperty QDeclarativeListProperty<QDeclarativeDescription> Place::descriptions

    This property descriptions list.

    Note: this property's changed() signal is currently emitted only if the
    whole element changes, not if only the contents of the element change.
*/
QDeclarativeListProperty<QDeclarativeDescription> QDeclarativePlace::descriptions()
{
    return QDeclarativeListProperty<QDeclarativeDescription>(this,
                                                          0, // opaque data parameter
                                                          descriptions_append,
                                                          descriptions_count,
                                                          descriptions_at,
                                                          descriptions_clear);
}

void QDeclarativePlace::descriptions_append(QDeclarativeListProperty<QDeclarativeDescription> *prop,
                                                  QDeclarativeDescription *value)
{
    QDeclarativePlace* object = static_cast<QDeclarativePlace*>(prop->object);
    QDeclarativeDescription *altValue = new QDeclarativeDescription(object);
    altValue->setDescription(value->description());
    object->m_descriptions.append(altValue);
    QList<QPlaceDescription> list = object->m_src.descriptions();
    list.append(value->description());
    object->m_src.setDescriptions(list);
    emit object->descriptionsChanged();
}

int QDeclarativePlace::descriptions_count(QDeclarativeListProperty<QDeclarativeDescription> *prop)
{
    return static_cast<QDeclarativePlace*>(prop->object)->m_descriptions.count();
}

QDeclarativeDescription* QDeclarativePlace::descriptions_at(QDeclarativeListProperty<QDeclarativeDescription> *prop,
                                                                          int index)
{
    QDeclarativePlace* object = static_cast<QDeclarativePlace*>(prop->object);
    QDeclarativeDescription *res = NULL;
    if (object->m_descriptions.count() > index && index > -1) {
        res = object->m_descriptions[index];
    }
    return res;
}

void QDeclarativePlace::descriptions_clear(QDeclarativeListProperty<QDeclarativeDescription> *prop)
{
    QDeclarativePlace* object = static_cast<QDeclarativePlace*>(prop->object);
    qDeleteAll(object->m_descriptions);
    object->m_descriptions.clear();
    object->m_src.setDescriptions(QList<QPlaceDescription>());
    emit object->descriptionsChanged();
}

void QDeclarativePlace::synchronizeDescriptions()
{
    qDeleteAll(m_descriptions);
    m_descriptions.clear();
    foreach (QPlaceDescription value, m_src.descriptions()) {
        QDeclarativeDescription* declarativeValue = new QDeclarativeDescription(value, this);
        m_descriptions.append(declarativeValue);
    }
}

/*!
    \qmlproperty QDeclarativeListProperty<QDeclarativeSupplier> Place::suppliers

    This property suppliers list.

    Note: this property's changed() signal is currently emitted only if the
    whole element changes, not if only the contents of the element change.
*/
QDeclarativeListProperty<QDeclarativeSupplier> QDeclarativePlace::suppliers()
{
    return QDeclarativeListProperty<QDeclarativeSupplier>(this,
                                                          0, // opaque data parameter
                                                          suppliers_append,
                                                          suppliers_count,
                                                          suppliers_at,
                                                          suppliers_clear);
}

void QDeclarativePlace::suppliers_append(QDeclarativeListProperty<QDeclarativeSupplier> *prop,
                                                  QDeclarativeSupplier *value)
{
    QDeclarativePlace* object = static_cast<QDeclarativePlace*>(prop->object);
    QDeclarativeSupplier *altValue = new QDeclarativeSupplier(object);
    altValue->setSupplier(value->supplier());
    object->m_suppliers.append(altValue);
    QList<QPlaceSupplier> list = object->m_src.suppliers();
    list.append(value->supplier());
    object->m_src.setSuppliers(list);
    emit object->suppliersChanged();
}

int QDeclarativePlace::suppliers_count(QDeclarativeListProperty<QDeclarativeSupplier> *prop)
{
    return static_cast<QDeclarativePlace*>(prop->object)->m_suppliers.count();
}

QDeclarativeSupplier* QDeclarativePlace::suppliers_at(QDeclarativeListProperty<QDeclarativeSupplier> *prop,
                                                                          int index)
{
    QDeclarativePlace* object = static_cast<QDeclarativePlace*>(prop->object);
    QDeclarativeSupplier *res = NULL;
    if (object->m_suppliers.count() > index && index > -1) {
        res = object->m_suppliers[index];
    }
    return res;
}

void QDeclarativePlace::suppliers_clear(QDeclarativeListProperty<QDeclarativeSupplier> *prop)
{
    QDeclarativePlace* object = static_cast<QDeclarativePlace*>(prop->object);
    qDeleteAll(object->m_suppliers);
    object->m_suppliers.clear();
    object->m_src.setSuppliers(QList<QPlaceSupplier>());
    emit object->suppliersChanged();
}

void QDeclarativePlace::synchronizeSuppliers()
{
    qDeleteAll(m_suppliers);
    m_suppliers.clear();
    foreach (QPlaceSupplier value, m_src.suppliers()) {
        QDeclarativeSupplier* declarativeValue = new QDeclarativeSupplier(value, this);
        m_suppliers.append(declarativeValue);
    }
}
