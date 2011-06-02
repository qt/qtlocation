#ifndef QPLACECONTACT_P_H
#define QPLACECONTACT_P_H

#include <QSharedData>

#include "qplacecontact.h"

namespace QT_PLACES_NAMESPACE {

class QPlaceContactPrivate : public QSharedData
{
public:
    QPlaceContactPrivate();
    QPlaceContactPrivate(const QPlaceContactPrivate &other);

    ~QPlaceContactPrivate();

    bool operator==(const QPlaceContactPrivate &other) const;

    QString description;
    QPlaceContact::ContactType type;
    QString value;
};

} // QT_PLACES_NAMESPACE

#endif // QPLACECONTACT_P_H
