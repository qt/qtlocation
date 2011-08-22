#include "qdeclarativeplace_p.h"
#include "qdeclarativegeoserviceprovider_p.h"
#include "qdeclarativeplaceattribute_p.h"

#include <QtDeclarative/QDeclarativeInfo>
#include <QtLocation/QGeoServiceProvider>

QT_USE_NAMESPACE

/*!
    \qmlclass Place

    \brief The Place element holds place data.
    \inherits QObject

    Place cointains many properties holding data of the place like location,
    id, name etc.

    \ingroup qml-places
*/

QDeclarativePlace::QDeclarativePlace(QObject* parent)
:   QObject(parent), m_reviewModel(0), m_imageModel(0), m_detailsReply(0), m_plugin(0),
    m_complete(false), m_extendedAttributes(new QDeclarativePropertyMap())
{
}

QDeclarativePlace::QDeclarativePlace(const QGeoPlace &src, QObject *parent)
:   QObject(parent), m_reviewModel(0), m_imageModel(0), m_src(src), m_detailsReply(0), m_plugin(0),
    m_complete(false), m_extendedAttributes(new QDeclarativePropertyMap())
{
    synchronizeCategories();
    synchronizeDescriptions();
    synchronizeSuppliers();
    synchronizeExtendedAttributes();

    m_rating.setRating(m_src.rating());
    m_location.setLocation(m_src.location());
}

QDeclarativePlace::~QDeclarativePlace()
{
}

// From QDeclarativeParserStatus
void QDeclarativePlace::componentComplete()
{
    m_complete = true;
}

void QDeclarativePlace::setPlugin(QDeclarativeGeoServiceProvider *plugin)
{
    if (m_plugin == plugin)
        return;

    m_plugin = plugin;
    if (m_complete)
        emit pluginChanged();
    QGeoServiceProvider *serviceProvider = m_plugin->sharedGeoServiceProvider();
    QPlaceManager *placeManager = serviceProvider->placeManager();
    if (!placeManager || serviceProvider->error() != QGeoServiceProvider::NoError) {
        qmlInfo(this) << tr("Warning: Plugin does not support places.");
        return;
    }
}

QDeclarativeGeoServiceProvider* QDeclarativePlace::plugin() const
{
    return m_plugin;
}

QDeclarativeReviewModel *QDeclarativePlace::reviewModel()
{
    if (!m_reviewModel) {
        m_reviewModel = new QDeclarativeReviewModel(this);
        m_reviewModel->setPlace(this);
    }

    return m_reviewModel;
}

QDeclarativePlaceImageModel *QDeclarativePlace::imageModel()
{
    if (!m_imageModel) {
        m_imageModel = new QDeclarativePlaceImageModel(this);
        m_imageModel->setPlace(this);
    }

    return m_imageModel;
}

void QDeclarativePlace::setPlace(const QGeoPlace &src)
{
    QGeoPlace previous = m_src;
    m_src = src;

    if (previous.additionalData() != m_src.additionalData()) {
        emit additionalDataChanged();
    }
    if (previous.categories() != m_src.categories()) {
        synchronizeCategories();
        emit categoriesChanged();
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
    if (previous.name() != m_src.name()) {
        emit nameChanged();
    }
    if (previous.placeId() != m_src.placeId()) {
        emit placeIdChanged();
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
    if (previous.primaryPhone() != m_src.primaryPhone()) {
        emit primaryPhoneChanged();
    }
    if (previous.primaryFax() != m_src.primaryFax()) {
        emit primaryFaxChanged();
    }
    if (previous.primaryEmail() != m_src.primaryEmail()) {
        emit primaryEmailChanged();
    }
    if (previous.primaryUrl() != m_src.primaryUrl()) {
        emit primaryUrlChanged();
    }

    if (previous.placeId() != m_src.placeId()) {
        m_reviewModel->clear();
        m_imageModel->clear();
    }

    if (previous.extendedAttributes() != m_src.extendedAttributes())
    {
        synchronizeExtendedAttributes();
        emit extendedAttributesChanged();
    }
}

QGeoPlace QDeclarativePlace::place()
{
    QList<QPlaceCategory> categories;
    foreach (QDeclarativeCategory *value, m_categories) {
        categories.append(value->category());
    }
    m_src.setCategories(categories);
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
    \qmlproperty bool Place::detailsFetched

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
    \qmlproperty bool Place::fetchingDetails

    This property holds a boolean indicating whether the details are currently being fetched.
*/
void QDeclarativePlace::setFetchingDetails(bool fetching)
{
    if (fetching && m_detailsReply)
        return;

    if (!fetching && !m_detailsReply)
        return;

    if (!fetching && m_detailsReply) {
        m_detailsReply->abort();
        m_detailsReply->deleteLater();
        m_detailsReply = 0;
        emit fetchingDetailsChanged();
        return;
    }

    if (!m_plugin) {
        qmlInfo(this) << "plugin not set.";
        return;
    }

    QGeoServiceProvider *serviceProvider = m_plugin->sharedGeoServiceProvider();
    if (!serviceProvider)
        return;

    QPlaceManager *placeManager = serviceProvider->placeManager();
    if (!placeManager) {
        qmlInfo(this) << tr("Places not supported by %1 Plugin.").arg(m_plugin->name());
        return;
    }

    m_detailsReply = placeManager->getPlaceDetails(placeId());

    connect(m_detailsReply, SIGNAL(finished()), this, SLOT(detailsFetchedFinished()));
    connect(m_detailsReply, SIGNAL(error(QPlaceReply::Error)),
            this, SLOT(detailsError(QPlaceReply::Error)));

    emit fetchingDetailsChanged();
}

bool QDeclarativePlace::fetchingDetails() const
{
    return m_detailsReply;
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

void QDeclarativePlace::detailsFetchedFinished()
{
    if (!m_detailsReply)
        return;

    QGeoPlace details = m_detailsReply->result();

    setPlace(details);

    m_detailsReply->deleteLater();
    m_detailsReply = 0;
    emit fetchingDetailsChanged();
}

void QDeclarativePlace::detailsError(QPlaceReply::Error error)
{
    Q_UNUSED(error);

    m_detailsReply->deleteLater();
    m_detailsReply = 0;
    emit fetchingDetailsChanged();
}

/*!
    \qmlmethod void Place::getDetails()

    This methods starts fetching place details.

    \sa Place::fetchingDetails
*/
void QDeclarativePlace::getDetails()
{
    setFetchingDetails(true);
}

void QDeclarativePlace::ratePlace(qreal rating)
{
    if (!m_plugin) {
        qmlInfo(this) << tr("plugin not set.");
        return;
    }

    QGeoServiceProvider *serviceProvider = m_plugin->sharedGeoServiceProvider();
    if (!serviceProvider)
        return;

    QPlaceManager *placeManager = serviceProvider->placeManager();
    if (!placeManager) {
        qmlInfo(this) << tr("Places not supported by %1 Plugin.").arg(m_plugin->name());
        return;
    }

    placeManager->postRating(placeId(), rating);
}

/*!
    \qmlproperty string Place::primaryPhone

    This property holds the primary phone number of the place.
*/
void QDeclarativePlace::setPrimaryPhone(const QString &phone)
{
    if (m_src.primaryPhone() != phone) {
        m_src.setPrimaryPhone(phone);
        emit primaryPhoneChanged();
    }
}

QString QDeclarativePlace::primaryPhone() const
{
    return m_src.primaryPhone();
}

/*!
    \qmlproperty string Place::primaryFax

    This property holds the primary fax number of the place.
*/
void QDeclarativePlace::setPrimaryFax(const QString &fax)
{
    if (m_src.primaryFax() != fax) {
        m_src.setPrimaryFax(fax);
        emit primaryFaxChanged();
    }
}

QString QDeclarativePlace::primaryFax() const
{
    return m_src.primaryFax();
}

/*!
    \qmlproperty string Place::primaryEmail

    This property holds the primary email address of the place.
*/
void QDeclarativePlace::setPrimaryEmail(const QString &email)
{
    if (m_src.primaryEmail() != email) {
        m_src.setPrimaryEmail(email);
        emit primaryEmailChanged();
    }
}

QString QDeclarativePlace::primaryEmail() const
{
    return m_src.primaryEmail();
}

/*!
    \qmlproperty string Place::primaryUrl

    This property holds the primary website address of the place.
*/
void QDeclarativePlace::setPrimaryUrl(const QUrl &url)
{
    if (m_src.primaryUrl() != url) {
        m_src.setPrimaryUrl(url);
        emit primaryUrlChanged();
    }
}

QUrl QDeclarativePlace::primaryUrl() const
{
    return m_src.primaryUrl();
}

/*!
    \qmlproperty ExtendedAttributes extendedAttributes

    This property holds the extended attributes of a place.
    Note: this property's changed() signal is currently only emitted
    if the whole element changes, not if only the contents of
    the element changes.
*/
void QDeclarativePlace::setExtendedAttributes(QDeclarativePropertyMap *attribs)
{
    QStringList otherKeys = attribs->keys();
    bool isSame = true;
    if (otherKeys.count() == m_src.extendedAttributes().count()) {
        foreach (const QString &key, otherKeys) {
            if (m_src.extendedAttributes().value(key) !=
                    qvariant_cast<QDeclarativePlaceAttribute*>(attribs->value(key))->attribute()) {
                isSame = false;
                break;
            }
        }
    } else {
        isSame = false;
    }

    if (!isSame) {
        m_src.extendedAttributes().clear();
        QGeoPlace::ExtendedAttributes extendedAttributes;
        foreach (const QString &key, otherKeys)
            extendedAttributes.insert(key, (qvariant_cast<QDeclarativePlaceAttribute*>(attribs->value(key)))->attribute());
        m_src.setExtendedAttributes(extendedAttributes);

        synchronizeExtendedAttributes();
        emit extendedAttributesChanged();
    }
}

QDeclarativePropertyMap *QDeclarativePlace::extendedAttributes() const
{
    return m_extendedAttributes;
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

void QDeclarativePlace::synchronizeExtendedAttributes()
{
    QStringList keys = m_extendedAttributes->keys();
    foreach (const QString &key, keys)
        m_extendedAttributes->clear(key);

    QMapIterator<QString, QPlaceAttribute> attribIter(m_src.extendedAttributes());
    while (attribIter.hasNext()) {
        attribIter.next();
        m_extendedAttributes->insert(attribIter.key(),
            qVariantFromValue(new QDeclarativePlaceAttribute(attribIter.value())));
    }
}
