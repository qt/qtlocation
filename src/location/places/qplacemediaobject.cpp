#include "qplacemediaobject.h"
#include "qplacemediaobject_p.h"

using namespace QT_PLACES_NAMESPACE;

QPlaceMediaObjectPrivate::QPlaceMediaObjectPrivate() : QSharedData()
{
}

QPlaceMediaObjectPrivate::QPlaceMediaObjectPrivate(const QPlaceMediaObjectPrivate &other)
    : QSharedData()
{
    this->url = other.url;
    this->thumbnailURL = other.thumbnailURL;
    this->id = other.id;
    this->metaInfo = other.metaInfo;
    this->mimeType = other.mimeType;
    this->supplier = other.supplier;
}

QPlaceMediaObjectPrivate::~QPlaceMediaObjectPrivate()
{
}

bool QPlaceMediaObjectPrivate::operator==(const QPlaceMediaObjectPrivate &other) const
{
    return (
            this->url == other.url
            && this->thumbnailURL == other.thumbnailURL
            && this->id == other.id
            && this->metaInfo == other.metaInfo
            && this->mimeType == other.mimeType
            && this->supplier == other.supplier
    );
}

/*!
    \class QPlaceMediaObject

    \inmodule QPlaces

    \brief The QPlaceMediaObject class represents a media object.


    Each QPlaceMediaObject represents a media object with a number of attributes
    such as type, thumbnail, media provider etc. Each QPlaceMediaObject may be associated
    with place.

    Media objects are read-only, e.g. user of API might get list of media objects
    associated to specific place but can not edit its content.

    QPlaceMediaObject is an in memory representation of a media object.
*/

/*!
    Constructs an new media object.
*/
QPlaceMediaObject::QPlaceMediaObject()
    : d(new QPlaceMediaObjectPrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlaceMediaObject::QPlaceMediaObject(const QPlaceMediaObject &other)
    :d(other.d)
{
}

/*!
    Destructor.
*/
QPlaceMediaObject::~QPlaceMediaObject()
{
}

QPlaceMediaObject &QPlaceMediaObject::operator =(const QPlaceMediaObject &other) {
    d = other.d;
    return *this;
}

bool QPlaceMediaObject::operator==(const QPlaceMediaObject &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    Returns media url.
*/
QString QPlaceMediaObject::url() const
{
    return d->url;
}

/*!
    Sets media url.
*/
void QPlaceMediaObject::setUrl(const QString &data)
{
    d->url = data;
}

/*!
    Returns media url for thumbnail.
*/
QString QPlaceMediaObject::thumbnailUrl() const
{
    return d->thumbnailURL;
}

/*!
    Sets media url for thumbnail.
*/
void QPlaceMediaObject::setThumbnailUrl(const QString &data)
{
    d->thumbnailURL = data;
}

/*!
    Returns media id.
*/
QString QPlaceMediaObject::id() const
{
    return d->id;
}

/*!
    Sets media id.
*/
void QPlaceMediaObject::setId(const QString &data)
{
    d->id = data;
}

/*!
    Returns media meta info.
*/
QString QPlaceMediaObject::metaInfo() const
{
    return d->metaInfo;
}

/*!
    Sets media meta info.
*/
void QPlaceMediaObject::setMetaInfo(const QString &data)
{
    d->metaInfo = data;
}

/*!
    Returns media mime type.
*/
QString QPlaceMediaObject::mimeType() const
{
    return d->mimeType;
}

/*!
    Sets media mime type.
*/
void QPlaceMediaObject::setMimeType(const QString &data)
{
    d->mimeType = data;
}

/*!
    Returns supplier.
*/
QPlaceSupplier QPlaceMediaObject::supplier() const
{
    return d->supplier;
}

/*!
    Sets supplier.
*/
void QPlaceMediaObject::setSupplier(const QPlaceSupplier &data)
{
    d->supplier = data;
}
