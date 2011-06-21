#include "qdeclarativemediaobject_p.h"

QTM_USE_NAMESPACE

/*!
    \qmlclass MediaObject

    \brief The MediaObject element holds media data.
    \inherits QObject

    MediaObject cointains many properties holding data of the media like URL,
    type, etc.

    \ingroup qml-places
*/

QDeclarativeMediaObject::QDeclarativeMediaObject(QObject* parent)
        : QObject(parent)
{
}

QDeclarativeMediaObject::QDeclarativeMediaObject(const QPlaceMediaObject &src,
        QObject *parent)
        : QObject(parent),
          m_declarativeSupplier(src.supplier()),
          m_src(src)
{
}

QDeclarativeMediaObject::~QDeclarativeMediaObject()
{
}

void QDeclarativeMediaObject::setMediaObject(const QPlaceMediaObject &src)
{
    QPlaceMediaObject previous = m_src;
    m_src = src;

    if (previous.url() != m_src.url()) {
        emit urlChanged();
    }
    if (previous.thumbnailUrl() != m_src.thumbnailUrl()) {
        emit thumbnailUrlChanged();
    }
    if (previous.mimeType() != m_src.mimeType()) {
        emit mimeTypeChanged();
    }
    if (previous.metaInfo() != m_src.metaInfo()) {
        emit metaInfoChanged();
    }
    if (previous.id() != m_src.id()) {
        emit idChanged();
    }
    if (previous.supplier() != m_src.supplier()) {
        m_declarativeSupplier.setSupplier(src.supplier());
        emit supplierChanged();
    }
}

QPlaceMediaObject QDeclarativeMediaObject::mediaObject() const
{
    return m_src;
}

/*!
    \qmlproperty string MediaObject::URL

    This property holds URL of the media.
*/

void QDeclarativeMediaObject::setUrl(const QString &url)
{
    if (m_src.url() != url) {
        m_src.setUrl(url);
        emit urlChanged();
    }
}

QString QDeclarativeMediaObject::url() const
{
    return m_src.url();
}

/*!
    \qmlproperty string MediaObject::thumbnailURL

    This property holds thumbnail URL.
*/

void QDeclarativeMediaObject::setThumbnailUrl(const QString &thumbnailUrl)
{
    if (m_src.thumbnailUrl() != thumbnailUrl) {
        m_src.setThumbnailUrl(thumbnailUrl);
        emit thumbnailUrlChanged();
    }
}

QString QDeclarativeMediaObject::thumbnailUrl() const
{
    return m_src.thumbnailUrl();
}

/*!
    \qmlproperty string MediaObject::mimeType

    This property holds mime type.
*/

void QDeclarativeMediaObject::setMimeType(const QString &mimeType)
{
    if (m_src.mimeType() != mimeType) {
        m_src.setMimeType(mimeType);
        emit mimeTypeChanged();
    }
}

QString QDeclarativeMediaObject::mimeType() const
{
    return m_src.mimeType();
}

/*!
    \qmlproperty string MediaObject::metaInfo

    This property holds meta information.
*/

void QDeclarativeMediaObject::setMetaInfo(const QString &metaInfo)
{
    if (m_src.metaInfo() != metaInfo) {
        m_src.setMetaInfo(metaInfo);
        emit metaInfoChanged();
    }
}

QString QDeclarativeMediaObject::metaInfo() const
{
    return m_src.metaInfo();
}

/*!
    \qmlproperty string MediaObject::id

    This property holds id of media.
*/

void QDeclarativeMediaObject::setId(const QString &id)
{
    if (m_src.id() != id) {
        m_src.setId(id);
        emit idChanged();
    }
}

QString QDeclarativeMediaObject::id() const
{
    return m_src.id();
}

/*!
    \qmlproperty string MediaObject::supplier

    This property holds supplier info.
*/

void QDeclarativeMediaObject::setSupplier(QDeclarativeSupplier *src)
{
    if (m_declarativeSupplier.supplier() != src->supplier()) {
        m_declarativeSupplier.setSupplier(src->supplier());
        emit supplierChanged();
    }
}

QDeclarativeSupplier *QDeclarativeMediaObject::supplier()
{
    return &m_declarativeSupplier;
}
