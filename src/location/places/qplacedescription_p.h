#ifndef QPLACEDESCRIPTION_P_H
#define QPLACEDESCRIPTION_P_H

#include <QSharedData>

#include "qplacedescription.h"

namespace QT_PLACES_NAMESPACE {

class QPlaceDescriptionPrivate : public QSharedData
{
public:
    QPlaceDescriptionPrivate();
    QPlaceDescriptionPrivate(const QPlaceDescriptionPrivate &other);

    ~QPlaceDescriptionPrivate();

    bool operator==(const QPlaceDescriptionPrivate &other) const;

    QString content;
    QString contentTitle;
    QString contentType;
    QPlaceSupplier supplier;
    QString sourceURL;
    QString language;
};

} // QT_PLACES_NAMESPACE

#endif // QPLACEDESCRIPTION_P_H
