#ifndef QDECLARATIVESUPPLIER_P_H
#define QDECLARATIVESUPPLIER_P_H

#include <QObject>
#include <QtCore/QUrl>
#include <QtDeclarative/qdeclarative.h>
#include <qplacesupplier.h>

QT_BEGIN_NAMESPACE

class QDeclarativeSupplier : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged);
    Q_PROPERTY(QString supplierId READ supplierId WRITE setSupplierId NOTIFY supplierIdChanged);
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(QUrl supplierIconUrl READ supplierIconUrl WRITE setSupplierIconUrl NOTIFY supplierIconUrlChanged)

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
    QUrl url() const;
    void setUrl(const QUrl &data);
    QUrl supplierIconUrl() const;
    void setSupplierIconUrl(const QUrl &data);

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
