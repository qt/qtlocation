#ifndef QPLACESUPPLIERSREPOSITORY_H
#define QPLACESUPPLIERSREPOSITORY_H

#include <QObject>
#include <QList>

#include "../qplaceglobal.h"
#include "../qplacesupplier.h"

namespace QT_PLACES_NAMESPACE {

class QPlaceSuppliersRepository : public QObject
{
public:
    static QPlaceSuppliersRepository *instance();
    ~QPlaceSuppliersRepository();

    QPlaceSupplier addSupplier(const QPlaceSupplier &src);
private:
    explicit QPlaceSuppliersRepository(QObject *parent = 0);

private:
    void copyMissingData(const QPlaceSupplier &src, QPlaceSupplier &target);

    QList<QPlaceSupplier> suppliers;
};

} // QT_PLACES_NAMESPACE

#endif // QPLACESUPPLIERSREPOSITORY_H
