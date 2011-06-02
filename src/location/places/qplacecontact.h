#ifndef QPLACECONTACT_H
#define QPLACECONTACT_H

#include <QSharedDataPointer>
#include <QString>
#include "qplaceglobal.h"

namespace QT_PLACES_NAMESPACE {

class QPlaceContactPrivate;

class Q_PLACES_EXPORT QPlaceContact
{
public:
    enum ContactType {
        Phone,
        Email,
        URL,
        Fax,
        IM,
        Undefined
    };

    QPlaceContact();
    QPlaceContact(const QPlaceContact &other);

    virtual ~QPlaceContact();

    QPlaceContact &operator=(const QPlaceContact &other);

    bool operator==(const QPlaceContact &other) const;
    bool operator!=(const QPlaceContact &other) const {
        return !(other == *this);
    }

    QString description() const;
    void setDescription(const QString& data);
    ContactType type() const;
    void setType(const ContactType &data);
    QString value() const;
    void setValue(const QString& data);

private:
    QSharedDataPointer<QPlaceContactPrivate> d;
};

} // QT_PLACES_NAMESPACE

#endif // QPLACECONTACT_H
