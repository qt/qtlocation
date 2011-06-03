#ifndef QPLACESUPPLIERSREPOSITORY_H
#define QPLACESUPPLIERSREPOSITORY_H

#include <QObject>
#include <QList>

#include <qplacesupplier.h>
#include <qmobilityglobal.h>

QTM_BEGIN_NAMESPACE

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

QTM_END_NAMESPACE

#endif // QPLACESUPPLIERSREPOSITORY_H
