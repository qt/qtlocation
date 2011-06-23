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
    if (previous.url() != m_src.url()) {
        emit urlChanged();
    }
    if (previous.supplierIconUrl() != m_src.supplierIconUrl()) {
        emit supplierIconUrlChanged();
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
    \qmlproperty string Supplier::supplierIconUrl

    This property holds icon URL.
*/

void QDeclarativeSupplier::setSupplierIconUrl(const QString &supplierIconUrl)
{
    if (m_src.supplierIconUrl() != supplierIconUrl) {
        m_src.setSupplierIconUrl(supplierIconUrl);
        emit supplierIconUrlChanged();
    }
}

QString QDeclarativeSupplier::supplierIconUrl() const
{
    return m_src.supplierIconUrl();
}

/*!
    \qmlproperty string Supplier::url

    This property holds supplier URL.
*/

void QDeclarativeSupplier::setUrl(const QString &url)
{
    if (m_src.url() != url) {
        m_src.setUrl(url);
        emit urlChanged();
    }
}

QString QDeclarativeSupplier::url() const
{
    return m_src.url();
}
