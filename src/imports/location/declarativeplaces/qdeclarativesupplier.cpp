#include "qdeclarativesupplier_p.h"

QTM_USE_NAMESPACE

/*!
    \qmlclass Supplier

    \brief The Supplier element holds supplier data.
    \inherits QObject

    Supplier cointains many properties holding data of the supplier like name,
    id, etc.

    \ingroup qml-places
*/

QDeclarativeSupplier::QDeclarativeSupplier(QObject* parent)
        : QObject(parent)
{
}

QDeclarativeSupplier::QDeclarativeSupplier(const QPlaceSupplier &src,
        QObject *parent)
        : QObject(parent),
          m_src(src)
{
}

QDeclarativeSupplier::~QDeclarativeSupplier()
{
}

void QDeclarativeSupplier::setSupplier(const QPlaceSupplier &src)
{
    QPlaceSupplier previous = m_src;
    m_src = src;

   if (previous.name() != m_src.name()) {
        emit nameChanged();
    }
    if (previous.supplierId() != m_src.supplierId()) {
        emit supplierIdChanged();
    }
    if (previous.URL() != m_src.URL()) {
        emit URLChanged();
    }
    if (previous.supplierIconURL() != m_src.supplierIconURL()) {
        emit supplierIconURLChanged();
    }
}

QPlaceSupplier QDeclarativeSupplier::supplier() const
{
    return m_src;
}

/*!
    \qmlproperty string Supplier::supplierId

    This property holds id.
*/

void QDeclarativeSupplier::setSupplierId(const QString &supplierId)
{
    if (m_src.supplierId() != supplierId) {
        m_src.setSupplierId(supplierId);
        emit supplierIdChanged();
    }
}

QString QDeclarativeSupplier::supplierId() const
{
    return m_src.supplierId();
}

/*!
    \qmlproperty string Supplier::name

    This property holds name.
*/

void QDeclarativeSupplier::setName(const QString &name)
{
    if (m_src.name() != name) {
        m_src.setName(name);
        emit nameChanged();
    }
}

QString QDeclarativeSupplier::name() const
{
    return m_src.name();
}

/*!
    \qmlproperty string Supplier::supplierIconURL

    This property holds icon URL.
*/

void QDeclarativeSupplier::setSupplierIconURL(const QString &supplierIconURL)
{
    if (m_src.supplierIconURL() != supplierIconURL) {
        m_src.setSupplierIconURL(supplierIconURL);
        emit supplierIconURLChanged();
    }
}

QString QDeclarativeSupplier::supplierIconURL() const
{
    return m_src.supplierIconURL();
}

/*!
    \qmlproperty string Supplier::URL

    This property holds supplier URL.
*/

void QDeclarativeSupplier::setURL(const QString &URL)
{
    if (m_src.URL() != URL) {
        m_src.setURL(URL);
        emit URLChanged();
    }
}

QString QDeclarativeSupplier::URL() const
{
    return m_src.URL();
}
