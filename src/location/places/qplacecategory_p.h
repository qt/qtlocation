#ifndef QPLACECATEGORY_P_H
#define QPLACECATEGORY_P_H

#include <QSharedData>

#include "qplacecategory.h"

QTM_BEGIN_NAMESPACE

class QPlaceCategoryPrivate : public QSharedData
{
public:
    QPlaceCategoryPrivate();
    QPlaceCategoryPrivate(const QPlaceCategoryPrivate &other);

    ~QPlaceCategoryPrivate();

    bool operator==(const QPlaceCategoryPrivate &other) const;

    QList<QPlaceAlternativeValue> alternativeNames;
    QString categoryId;
    QString categorySystemId;
    QString description;
    QString name;
};

QTM_END_NAMESPACE

#endif // QPLACECATEGORY_P_H
