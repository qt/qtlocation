#ifndef QPLACESUPPLIER_H
#define QPLACESUPPLIER_H

#include <QSharedDataPointer>
#include "qmobilityglobal.h"

QTM_BEGIN_NAMESPACE

class QPlaceSupplierPrivate;

class Q_LOCATION_EXPORT QPlaceSupplier
{
public:
    QPlaceSupplier();
    QPlaceSupplier(const QPlaceSupplier &other);

    virtual ~QPlaceSupplier();

    QPlaceSupplier &operator=(const QPlaceSupplier &other);

    bool operator==(const QPlaceSupplier &other) const;
    bool operator!=(const QPlaceSupplier &other) const {
        return !(other == *this);
    }

    QString name() const;
    void setName(const QString &data);
    QString supplierId() const;
    void setSupplierId(const QString &data);
    QString URL() const;
    void setURL(const QString &data);
    QString supplierIconURL() const;
    void setSupplierIconURL(const QString &data);

private:
    QSharedDataPointer<QPlaceSupplierPrivate> d;
};

QTM_END_NAMESPACE

#endif // QPLACESUPPLIER_H
