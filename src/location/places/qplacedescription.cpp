#include "qplacedescription.h"
#include "qplacedescription_p.h"

using namespace QT_PLACES_NAMESPACE;

QPlaceDescriptionPrivate::QPlaceDescriptionPrivate() : QSharedData()
{
}

QPlaceDescriptionPrivate::QPlaceDescriptionPrivate(const QPlaceDescriptionPrivate &other)
    : QSharedData()
{
    this->content = other.content;
    this->contentTitle = other.contentTitle;
    this->contentType = other.contentType;
    this->supplier = other.supplier;
    this->sourceURL = other.sourceURL;
    this->language = other.language;
}

QPlaceDescriptionPrivate::~QPlaceDescriptionPrivate()
{
}

bool QPlaceDescriptionPrivate::operator==(const QPlaceDescriptionPrivate &other) const
{
    return (
            this->content == other.content
            && this->contentTitle == other.contentTitle
            && this->contentType == other.contentType
            && this->supplier == other.supplier
            && this->sourceURL == other.sourceURL
            && this->language == other.language
    );
}

/*!
    \class QPlaceDescription

    \inmodule QPlaces

    \brief The QDescription class represents a address object.

    Each QPlaceDescription represents a description object with a number of attributes
    such as title, value etc. Each QPlaceDescription is associated with place.

    Description objects are read-only, e.g. user of API might get description objects
    associated to specific place but can not edit its content. User might also create new description.

*/

/*!
    Constructs an new place description object.
*/
QPlaceDescription::QPlaceDescription()
    : d(new QPlaceDescriptionPrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlaceDescription::QPlaceDescription(const QPlaceDescription &other)
    :d(other.d)
{
}

/*!
    Destructor.
*/
QPlaceDescription::~QPlaceDescription()
{
}

QPlaceDescription &QPlaceDescription::operator =(const QPlaceDescription &other) {
    d = other.d;
    return *this;
}

bool QPlaceDescription::operator==(const QPlaceDescription &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    Returns content.
*/
QString QPlaceDescription::content() const
{
    return d->content;
}

/*!
    Sets content.
*/
void QPlaceDescription::setContent(const QString &data)
{
    d->content = data;
}

/*!
    Returns content title.
*/
QString QPlaceDescription::contentTitle() const
{
    return d->contentTitle;
}

/*!
    Sets content title.
*/
void QPlaceDescription::setContentTitle(const QString &data)
{
    d->contentTitle = data;
}

/*!
    Returns content type.
*/
QString QPlaceDescription::contentType() const
{
    return d->contentType;
}

/*!
    Sets content type.
*/
void QPlaceDescription::setContentType(const QString &data)
{
    d->contentType = data;
}

/*!
    Returns supplier. Do not remove it.
*/
QPlaceSupplier QPlaceDescription::supplier() const
{
    return d->supplier;
}

/*!
    Sets supplier.
*/
void QPlaceDescription::setSupplier(const QPlaceSupplier &data)
{
    d->supplier = data;
}

/*!
    Returns source url.
*/
QString QPlaceDescription::sourceURL() const
{
    return d->sourceURL;
}

/*!
    Sets source url.
*/
void QPlaceDescription::setSourceURL(const QString &data)
{
    d->sourceURL = data;
}

/*!
    Returns language.
*/
QString QPlaceDescription::language() const
{
    return d->language;
}

/*!
    Sets language.
*/
void QPlaceDescription::setLanguage(const QString &data)
{
    d->language = data;
}
