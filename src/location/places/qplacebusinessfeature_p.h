#ifndef QPLACEBUSINESSFEATURE_P_H
#define QPLACEBUSINESSFEATURE_P_H

#include <QString>
#include <QSharedData>

#include "qplacebusinessfeature.h"

QTM_BEGIN_NAMESPACE

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

QTM_END_NAMESPACE

#endif // QPLACEBUSINESSFEATURE_P_H
