#ifndef QDECLARATIVESUPPLIER_P_H
#define QDECLARATIVESUPPLIER_P_H

#include <QObject>
#include <QtDeclarative/qdeclarative.h>
#include <qplacesupplier.h>

QTM_BEGIN_NAMESPACE

class QDeclarativeSupplier : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged);
    Q_PROPERTY(QString supplierId READ supplierId WRITE setSupplierId NOTIFY supplierIdChanged);
    Q_PROPERTY(QString URL READ URL WRITE setURL NOTIFY URLChanged);
    Q_PROPERTY(QString supplierIconURL READ supplierIconURL WRITE setSupplierIconURL NOTIFY supplierIconURLChanged);

public:
    explicit QDeclarativeSupplier(QObject* parent = 0);
    explicit QDeclarativeSupplier(const QPlaceSupplier &src, QObject* parent = 0);
    ~QDeclarativeSupplier();

    QPlaceSupplier supplier() const;
    void setSupplier(const QPlaceSupplier &src);

    QString name() const;
    void setName(const QString &data);
    QString supplierId() const;
    void setSupplierId(const QString &data);
    QString URL() const;
    void setURL(const QString &data);
    QString supplierIconURL() const;
    void setSupplierIconURL(const QString &data);

signals:
    void nameChanged();
    void supplierIdChanged();
    void URLChanged();
    void supplierIconURLChanged();

private:
    QPlaceSupplier m_src;
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeSupplier));

#endif // QDECLARATIVESUPPLIER_P_H
