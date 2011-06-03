#ifndef QPLACESUPPLIER_P_H
#define QPLACESUPPLIER_P_H

#include <QString>
#include <QSharedData>

#include "qplacesupplier.h"

QTM_BEGIN_NAMESPACE

class QPlaceSupplierPrivate : public QSharedData
{
public:
    QPlaceSupplierPrivate();
    QPlaceSupplierPrivate(const QPlaceSupplierPrivate &other);

    ~QPlaceSupplierPrivate();

    bool operator==(const QPlaceSupplierPrivate &other) const;

    QString name;
    QString supplierId;
    QString url;
    QString supplierIconURL;
};

QTM_END_NAMESPACE

#endif // QPLACESUPPLIER_P_H
