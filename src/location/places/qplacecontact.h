#ifndef QPLACECONTACT_H
#define QPLACECONTACT_H

#include <QSharedDataPointer>
#include <QString>
#include "qmobilityglobal.h"

QTM_BEGIN_NAMESPACE

class QPlaceContactPrivate;

class Q_LOCATION_EXPORT QPlaceContact
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

QTM_END_NAMESPACE

#endif // QPLACECONTACT_H
