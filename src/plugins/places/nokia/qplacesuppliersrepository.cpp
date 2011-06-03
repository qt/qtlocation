#include "qplacesuppliersrepository.h"

#include <qplacesupplier.h>

QTM_USE_NAMESPACE

QPlaceSuppliersRepository *QPlaceSuppliersRepository::instance()
{
    static QPlaceSuppliersRepository instance;
    return &instance;
}

QPlaceSuppliersRepository::QPlaceSuppliersRepository(QObject *parent)
    : QObject(parent)
{
}

QPlaceSuppliersRepository::~QPlaceSuppliersRepository()
{
    suppliers.clear();
}

QPlaceSupplier QPlaceSuppliersRepository::addSupplier(const QPlaceSupplier &src)
{
    QPlaceSupplier res;
    QPlaceSupplier tmp;

    foreach (tmp, suppliers) {
        if ((!src.supplierId().isEmpty() && src.supplierId() == tmp.supplierId())
                || (!src.name().isEmpty() && src.name() == tmp.name())) {
            copyMissingData(src, tmp);
            res = tmp;
            break;
        }
    }
    if (res.supplierId().isEmpty() && res.name().isEmpty()) {
        res = src;
        suppliers.append(res);
    }
    return res;
}

void QPlaceSuppliersRepository::copyMissingData(const QPlaceSupplier &src,
                                                QPlaceSupplier &target)
{
    if (target.name().isEmpty() && !src.name().isEmpty()) {
        target.setName(src.name());
    }
    if (target.supplierId().isEmpty() && !src.supplierId().isEmpty()) {
        target.setSupplierId(src.supplierId());
    }
    if (target.URL().isEmpty() && !src.URL().isEmpty()) {
        target.setURL(src.URL());
    }
    if (target.supplierIconURL().isEmpty() && !src.supplierIconURL().isEmpty()) {
        target.setSupplierIconURL(src.supplierIconURL());
    }
}
