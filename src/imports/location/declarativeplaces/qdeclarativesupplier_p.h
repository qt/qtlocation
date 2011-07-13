#ifndef QDECLARATIVESUPPLIER_P_H
#define QDECLARATIVESUPPLIER_P_H

#include <QObject>
#include <QtDeclarative/qdeclarative.h>
#include <qplacesupplier.h>

QT_BEGIN_NAMESPACE

class QDeclarativeSupplier : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged);
    Q_PROPERTY(QString supplierId READ supplierId WRITE setSupplierId NOTIFY supplierIdChanged);
    Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY urlChanged);
    Q_PROPERTY(QString supplierIconUrl READ supplierIconUrl WRITE setSupplierIconUrl NOTIFY supplierIconUrlChanged);

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
    QString url() const;
    void setUrl(const QString &data);
    QString supplierIconUrl() const;
    void setSupplierIconUrl(const QString &data);

signals:
    void nameChanged();
    void supplierIdChanged();
    void urlChanged();
    void supplierIconUrlChanged();

private:
    QPlaceSupplier m_src;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeSupplier));

#endif // QDECLARATIVESUPPLIER_P_H
