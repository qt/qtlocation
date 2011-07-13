#include "qdeclarativecontact_p.h"

QT_USE_NAMESPACE

/*!
    \qmlclass Contact

    \brief The Contact element holds contact data.
    \inherits QObject

    \ingroup qml-places
*/

QDeclarativeContact::QDeclarativeContact(QObject* parent)
        : QObject(parent)
{
}

QDeclarativeContact::QDeclarativeContact(const QPlaceContact &value,
        QObject *parent)
        : QObject(parent),
          m_value(value)
{
}

QDeclarativeContact::~QDeclarativeContact()
{
}

void QDeclarativeContact::setContact(const QPlaceContact &value)
{
    QPlaceContact previous = m_value;
    m_value = value;

    if (previous.description() != m_value.description()) {
        emit descriptionChanged();
    }
    if (previous.type() != m_value.type()) {
        emit typeChanged();
    }
    if (previous.value() != m_value.value()) {
        emit valueChanged();
    }
}

QPlaceContact QDeclarativeContact::contact() const
{
    return m_value;
}

/*!
    \qmlproperty string Contact::description

    This property holds description.
*/

void QDeclarativeContact::setDescription(const QString &description)
{
    if (m_value.description() != description) {
        m_value.setDescription(description);
        emit descriptionChanged();
    }
}

QString QDeclarativeContact::description() const
{
    return m_value.description();
}

/*!
    \qmlproperty string Contact::type

    This property holds type.
*/
void QDeclarativeContact::setType(const QDeclarativeContact::ContactType &type)
{
    if (m_value.type() != (QPlaceContact::ContactType)type) {
        m_value.setType((QPlaceContact::ContactType)type);
        emit typeChanged();
    }
}

QDeclarativeContact::ContactType QDeclarativeContact::type() const
{
    return (QDeclarativeContact::ContactType) m_value.type();
}

/*!
    \qmlproperty string Contact::value

    This property holds value.
*/

void QDeclarativeContact::setValue(const QString &value)
{
    if (m_value.value() != value) {
        m_value.setValue(value);
        emit valueChanged();
    }
}

QString QDeclarativeContact::value() const
{
    return m_value.value();
}
