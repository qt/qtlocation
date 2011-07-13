#include "qdeclarativealternativevalue_p.h"

QT_USE_NAMESPACE

/*!
    \qmlclass AlternativeValue

    \brief The AlternativeValue element holds another value for property.
    \inherits QObject

    \ingroup qml-places
*/

QDeclarativeAlternativeValue::QDeclarativeAlternativeValue(QObject* parent)
        : QObject(parent)
{
}

QDeclarativeAlternativeValue::QDeclarativeAlternativeValue(const QPlaceAlternativeValue &value,
        QObject *parent)
        : QObject(parent),
          m_value(value)
{
}

QDeclarativeAlternativeValue::~QDeclarativeAlternativeValue()
{
}

void QDeclarativeAlternativeValue::setValueObject(const QPlaceAlternativeValue &value)
{
    QPlaceAlternativeValue previous = m_value;
    m_value = value;

    if (previous.key() != m_value.key()) {
        emit keyChanged();
    }
    if (previous.language() != m_value.language()) {
        emit languageChanged();
    }
    if (previous.semantics() != m_value.semantics()) {
        emit semanticsChanged();
    }
    if (previous.type() != m_value.type()) {
        emit typeChanged();
    }
    if (previous.value() != m_value.value()) {
        emit valueChanged();
    }
}

QPlaceAlternativeValue QDeclarativeAlternativeValue::valueObject() const
{
    return m_value;
}

/*!
    \qmlproperty string AlternativeValue::key

    This property holds key description.
*/

void QDeclarativeAlternativeValue::setKey(const QString &key)
{
    if (m_value.key() != key) {
        m_value.setKey(key);
        emit keyChanged();
    }
}

QString QDeclarativeAlternativeValue::key() const
{
    return m_value.key();
}

/*!
    \qmlproperty string AlternativeValue::language

    This property holds language description.
*/

void QDeclarativeAlternativeValue::setLanguage(const QString &language)
{
    if (m_value.language() != language) {
        m_value.setLanguage(language);
        emit languageChanged();
    }
}

QString QDeclarativeAlternativeValue::language() const
{
    return m_value.language();
}

/*!
    \qmlproperty string AlternativeValue::semantics

    This property holds semantics.
*/
void QDeclarativeAlternativeValue::setSemantics(const QDeclarativeAlternativeValue::TextSemantics &semantics)
{
    if (m_value.semantics() != (QPlaceAlternativeValue::TextSemantics)semantics) {
        m_value.setSemantics((QPlaceAlternativeValue::TextSemantics)semantics);
        emit semanticsChanged();
    }
}

QDeclarativeAlternativeValue::TextSemantics QDeclarativeAlternativeValue::semantics() const
{
    return (QDeclarativeAlternativeValue::TextSemantics) m_value.semantics();
}

/*!
    \qmlproperty string AlternativeValue::type

    This property holds type.
*/
void QDeclarativeAlternativeValue::setType(const QDeclarativeAlternativeValue::NameType &type)
{
    if (m_value.type() != (QPlaceAlternativeValue::NameType)type) {
        m_value.setType((QPlaceAlternativeValue::NameType)type);
        emit typeChanged();
    }
}

QDeclarativeAlternativeValue::NameType QDeclarativeAlternativeValue::type() const
{
    return (QDeclarativeAlternativeValue::NameType) m_value.type();
}

/*!
    \qmlproperty string AlternativeValue::value

    This property holds value.
*/

void QDeclarativeAlternativeValue::setValue(const QString &value)
{
    if (m_value.value() != value) {
        m_value.setValue(value);
        emit valueChanged();
    }
}

QString QDeclarativeAlternativeValue::value() const
{
    return m_value.value();
}
