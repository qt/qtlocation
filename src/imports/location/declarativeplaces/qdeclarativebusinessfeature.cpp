#include "qdeclarativebusinessfeature_p.h"

QTM_USE_NAMESPACE

/*!
    \qmlclass BusinessFeature

    \brief The BusinessFeature element holds business feature information.
    \inherits QObject

    \ingroup qml-places
*/

QDeclarativeBusinessFeature::QDeclarativeBusinessFeature(QObject* parent)
        : QObject(parent)
{
}

QDeclarativeBusinessFeature::QDeclarativeBusinessFeature(const QPlaceBusinessFeature &value,
        QObject *parent)
        : QObject(parent),
          m_value(value)
{
}

QDeclarativeBusinessFeature::~QDeclarativeBusinessFeature()
{
}

void QDeclarativeBusinessFeature::setBusinessFeature(const QPlaceBusinessFeature &value)
{
    QPlaceBusinessFeature previous = m_value;
    m_value = value;

    if (previous.key() != m_value.key()) {
        emit keyChanged();
    }
    if (previous.label() != m_value.label()) {
        emit labelChanged();
    }
    if (previous.value() != m_value.value()) {
        emit valueChanged();
    }
}

QPlaceBusinessFeature QDeclarativeBusinessFeature::businessFeature() const
{
    return m_value;
}

/*!
    \qmlproperty string BusinessFeature::key

    This property holds key description.
*/

void QDeclarativeBusinessFeature::setKey(const QString &key)
{
    if (m_value.key() != key) {
        m_value.setKey(key);
        emit keyChanged();
    }
}

QString QDeclarativeBusinessFeature::key() const
{
    return m_value.key();
}

/*!
    \qmlproperty string BusinessFeature::label

    This property holds label.
*/

void QDeclarativeBusinessFeature::setLabel(const QString &label)
{
    if (m_value.label() != label) {
        m_value.setLabel(label);
        emit labelChanged();
    }
}

QString QDeclarativeBusinessFeature::label() const
{
    return m_value.label();
}

/*!
    \qmlproperty string BusinessFeature::value

    This property holds value.
*/

void QDeclarativeBusinessFeature::setValue(const QString &value)
{
    if (m_value.value() != value) {
        m_value.setValue(value);
        emit valueChanged();
    }
}

QString QDeclarativeBusinessFeature::value() const
{
    return m_value.value();
}
