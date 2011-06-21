#include "qdeclarativedescription_p.h"

QTM_USE_NAMESPACE

/*!
    \qmlclass Description

    \brief The Description element holds description data.
    \inherits QObject

    Description cointains many properties holding data of the description like content,
    language, title, etc.

    \ingroup qml-places
*/

QDeclarativeDescription::QDeclarativeDescription(QObject* parent)
        : QObject(parent)
{
}

QDeclarativeDescription::QDeclarativeDescription(const QPlaceDescription &src,
        QObject *parent)
        : QObject(parent),
          m_declarativeSupplier(src.supplier()),
          m_src(src)
{
}

QDeclarativeDescription::~QDeclarativeDescription()
{
}

void QDeclarativeDescription::setDescription(const QPlaceDescription &src)
{
    QPlaceDescription previous = m_src;
    m_src = src;

    if (previous.content() != m_src.content()) {
        emit contentChanged();
    }
    if (previous.contentTitle() != m_src.contentTitle()) {
        emit contentTitleChanged();
    }
    if (previous.contentType() != m_src.contentType()) {
        emit contentTypeChanged();
    }
    if (previous.language() != m_src.language()) {
        emit languageChanged();
    }
    if (previous.sourceURL() != m_src.sourceURL()) {
        emit sourceURLChanged();
    }
    if (previous.supplier() != m_src.supplier()) {
        m_declarativeSupplier.setSupplier(src.supplier());
        emit supplierChanged();
    }
}

QPlaceDescription QDeclarativeDescription::description() const
{
    return m_src;
}

/*!
    \qmlproperty string Description::content

    This property holds content of the description.
*/

void QDeclarativeDescription::setContent(const QString &content)
{
    if (m_src.content() != content) {
        m_src.setContent(content);
        emit contentChanged();
    }
}

QString QDeclarativeDescription::content() const
{
    return m_src.content();
}

/*!
    \qmlproperty string Description::contentTitle

    This property holds title of description.
*/

void QDeclarativeDescription::setContentTitle(const QString &contentTitle)
{
    if (m_src.contentTitle() != contentTitle) {
        m_src.setContentTitle(contentTitle);
        emit contentTitleChanged();
    }
}

QString QDeclarativeDescription::contentTitle() const
{
    return m_src.contentTitle();
}

/*!
    \qmlproperty string Description::contentType

    This property holds content type.
*/

void QDeclarativeDescription::setContentType(const QString &contentType)
{
    if (m_src.contentType() != contentType) {
        m_src.setContentType(contentType);
        emit contentTypeChanged();
    }
}

QString QDeclarativeDescription::contentType() const
{
    return m_src.contentType();
}

/*!
    \qmlproperty string Description::language

    This property holds language of description.
*/

void QDeclarativeDescription::setLanguage(const QString &language)
{
    if (m_src.language() != language) {
        m_src.setLanguage(language);
        emit languageChanged();
    }
}

QString QDeclarativeDescription::language() const
{
    return m_src.language();
}

/*!
    \qmlproperty string Description::sourceURL

    This property holds source URL.
*/

void QDeclarativeDescription::setSourceURL(const QString &sourceURL)
{
    if (m_src.sourceURL() != sourceURL) {
        m_src.setSourceURL(sourceURL);
        emit sourceURLChanged();
    }
}

QString QDeclarativeDescription::sourceURL() const
{
    return m_src.sourceURL();
}

/*!
    \qmlproperty string Description::supplier

    This property holds supplier info.
*/

void QDeclarativeDescription::setSupplier(QDeclarativeSupplier *src)
{
    if (m_declarativeSupplier.supplier() != src->supplier()) {
        m_declarativeSupplier.setSupplier(src->supplier());
        emit supplierChanged();
    }
}

QDeclarativeSupplier *QDeclarativeDescription::supplier()
{
    return &m_declarativeSupplier;
}
