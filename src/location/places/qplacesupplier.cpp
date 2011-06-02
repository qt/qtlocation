#include "qplacesupplier.h"
#include "qplacesupplier_p.h"

using namespace QT_PLACES_NAMESPACE;

QPlaceSupplierPrivate::QPlaceSupplierPrivate() : QSharedData()
{
}

QPlaceSupplierPrivate::QPlaceSupplierPrivate(const QPlaceSupplierPrivate &other)
    : QSharedData()
{
    this->name = other.name;
    this->supplierId = other.supplierId;
    this->url = other.url;
    this->supplierIconURL = other.supplierIconURL;
}

QPlaceSupplierPrivate::~QPlaceSupplierPrivate()
{
}

bool QPlaceSupplierPrivate::operator==(const QPlaceSupplierPrivate &other) const
{
    return (
            this->name == other.name
            && this->supplierId == other.supplierId
            && this->url == other.url
            && this->supplierIconURL == other.supplierIconURL
    );
}

/*!
    \class QPlaceSupplier

    \inmodule QPlaces

    \brief The QPlaceSupplier class represents a supplier object.

    Each QPlaceSupplier represents a supplier object with a number of attributes
    such as name, icon etc. Each QPlaceSupplier is associated with place, media, review or description.

*/

/*!
    Default constructor. Constructs an new supplier object.
*/
QPlaceSupplier::QPlaceSupplier()
    : d(new QPlaceSupplierPrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlaceSupplier::QPlaceSupplier(const QPlaceSupplier &other)
    :d(other.d)
{
}

/*!
    Destructor.
*/
QPlaceSupplier::~QPlaceSupplier()
{
}

QPlaceSupplier &QPlaceSupplier::operator =(const QPlaceSupplier &other) {
    d = other.d;
    return *this;
}

bool QPlaceSupplier::operator==(const QPlaceSupplier &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    Returns name.
*/
QString QPlaceSupplier::name() const
{
    return d->name;
}

/*!
    Sets name.
*/
void QPlaceSupplier::setName(const QString &data)
{
    d->name = data;
}

/*!
    Returns supplier id.
*/
QString QPlaceSupplier::supplierId() const
{
    return d->supplierId;
}

/*!
    Sets supplier id.
*/
void QPlaceSupplier::setSupplierId(const QString &data)
{
    d->supplierId = data;
}

/*!
    Returns URL.
*/
QString QPlaceSupplier::URL() const
{
    return d->url;
}

/*!
    Sets URL of the icon.
*/
void QPlaceSupplier::setURL(const QString &data)
{
    d->url = data;
}

/*!
    Returns URL of the icon.
*/
QString QPlaceSupplier::supplierIconURL() const
{
    return d->supplierIconURL;
}

/*!
    Sets URL of the icon.
*/
void QPlaceSupplier::setSupplierIconURL(const QString &data)
{
    d->supplierIconURL = data;
}
