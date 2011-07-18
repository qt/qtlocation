#ifndef QDECLARATIVEDESCRIPTION_P_H
#define QDECLARATIVEDESCRIPTION_P_H

#include <QObject>
#include <QDeclarativeListProperty>
#include <qplacedescription.h>
#include "qdeclarativesupplier_p.h"

QT_BEGIN_NAMESPACE

class QDeclarativeDescription : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString content READ content WRITE setContent NOTIFY contentChanged);
    Q_PROPERTY(QString contentTitle READ contentTitle WRITE setContentTitle NOTIFY contentTitleChanged);
    Q_PROPERTY(QString contentType READ contentType WRITE setContentType NOTIFY contentTypeChanged);
    Q_PROPERTY(QDeclarativeSupplier* supplier READ supplier WRITE setSupplier NOTIFY supplierChanged);
    Q_PROPERTY(QString sourceUrl READ sourceUrl WRITE setSourceUrl NOTIFY sourceUrlChanged);
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged);

public:
    explicit QDeclarativeDescription(QObject* parent = 0);
    explicit QDeclarativeDescription(const QPlaceDescription &src, QObject* parent = 0);
    ~QDeclarativeDescription();

    QPlaceDescription description();
    void setDescription(const QPlaceDescription &src);

    QString content() const;
    void setContent(const QString &data);
    QString contentTitle() const;
    void setContentTitle(const QString &data);
    QString contentType() const;
    void setContentType(const QString &data);
    QDeclarativeSupplier *supplier();
    void setSupplier(QDeclarativeSupplier *data);
    QString sourceUrl() const;
    void setSourceUrl(const QString &data);
    QString language() const;
    void setLanguage(const QString &data);

signals:
    void contentChanged();
    void contentTitleChanged();
    void contentTypeChanged();
    void supplierChanged();
    void sourceUrlChanged();
    void languageChanged();

private:
    QDeclarativeSupplier m_declarativeSupplier;
    QPlaceDescription m_src;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeDescription));

#endif // QDECLARATIVEDESCRIPTION_P_H
