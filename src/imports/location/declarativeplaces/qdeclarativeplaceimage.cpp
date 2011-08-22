#include "qdeclarativeplaceimage_p.h"

#include <QtCore/QUrl>

QT_USE_NAMESPACE

/*!
    \qmlclass PlaceImage

    \brief The PlaceImage element holds image meta-data.
    \inherits QObject

    PlaceImage cointains many properties holding data of the image like URL,
    thumbnail URL etc.

    \ingroup qml-places
*/

QDeclarativePlaceImage::QDeclarativePlaceImage(QObject* parent)
        : QObject(parent)
{
}

QDeclarativePlaceImage::QDeclarativePlaceImage(const QPlaceImage &src,
        QObject *parent)
        : QObject(parent),
          m_declarativeSupplier(src.supplier()),
          m_src(src)
{
}

QDeclarativePlaceImage::~QDeclarativePlaceImage()
{
}

void QDeclarativePlaceImage::setImage(const QPlaceImage &src)
{
    QPlaceImage previous = m_src;
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

QPlaceImage QDeclarativePlaceImage::image()
{
    m_src.setSupplier(m_declarativeSupplier.supplier());
    return m_src;
}

/*!
    \qmlproperty string PlaceImage::url

    This property holds URL of the image.
*/

void QDeclarativePlaceImage::setUrl(const QUrl &url)
{
    if (m_src.url() != url) {
        m_src.setUrl(url);
        emit urlChanged();
    }
}

QUrl QDeclarativePlaceImage::url() const
{
    return m_src.url();
}

/*!
    \qmlproperty string PlaceImage::thumbnailURL

    This property holds thumbnail URL.
*/

void QDeclarativePlaceImage::setThumbnailUrl(const QUrl &thumbnailUrl)
{
    if (m_src.thumbnailUrl() != thumbnailUrl) {
        m_src.setThumbnailUrl(thumbnailUrl);
        emit thumbnailUrlChanged();
    }
}

QUrl QDeclarativePlaceImage::thumbnailUrl() const
{
    return m_src.thumbnailUrl();
}

/*!
    \qmlproperty string PlaceImage::mimeType

    This property holds mime type.
*/

void QDeclarativePlaceImage::setMimeType(const QString &mimeType)
{
    if (m_src.mimeType() != mimeType) {
        m_src.setMimeType(mimeType);
        emit mimeTypeChanged();
    }
}

QString QDeclarativePlaceImage::mimeType() const
{
    return m_src.mimeType();
}

/*!
    \qmlproperty string PlaceImage::metaInfo

    This property holds meta information.
*/

void QDeclarativePlaceImage::setMetaInfo(const QString &metaInfo)
{
    if (m_src.metaInfo() != metaInfo) {
        m_src.setMetaInfo(metaInfo);
        emit metaInfoChanged();
    }
}

QString QDeclarativePlaceImage::metaInfo() const
{
    return m_src.metaInfo();
}

/*!
    \qmlproperty string PlaceImage::id

    This property holds id of image.
*/

void QDeclarativePlaceImage::setId(const QString &id)
{
    if (m_src.id() != id) {
        m_src.setId(id);
        emit idChanged();
    }
}

QString QDeclarativePlaceImage::id() const
{
    return m_src.id();
}

/*!
    \qmlproperty string PlaceImage::supplier

    This property holds supplier info.

    Note: this property's changed() signal is currently emitted only if the
    whole element changes, not if only the contents of the element change.
*/
void QDeclarativePlaceImage::setSupplier(QDeclarativeSupplier *src)
{
    if (m_declarativeSupplier.supplier() != src->supplier()) {
        m_declarativeSupplier.setSupplier(src->supplier());
        emit supplierChanged();
    }
}

QDeclarativeSupplier *QDeclarativePlaceImage::supplier()
{
    return &m_declarativeSupplier;
}
