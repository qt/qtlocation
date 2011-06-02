#ifndef QPLACEBUSINESSFEATURE_P_H
#define QPLACEBUSINESSFEATURE_P_H

#include <QString>
#include <QSharedData>

#include "qplacebusinessfeature.h"

namespace QT_PLACES_NAMESPACE {

class QPlaceBusinessFeaturePrivate : public QSharedData
{
public:
    QPlaceBusinessFeaturePrivate();
    QPlaceBusinessFeaturePrivate(const QPlaceBusinessFeaturePrivate &other);

    ~QPlaceBusinessFeaturePrivate();

    bool operator==(const QPlaceBusinessFeaturePrivate &other) const;

    QString key;
    QString label;
    QString value;
};

} // QT_PLACES_NAMESPACE

#endif // QPLACEBUSINESSFEATURE_P_H
