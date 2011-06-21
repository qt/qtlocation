#ifndef QDECLARATIVECONTACT_P_H
#define QDECLARATIVECONTACT_P_H

#include <qplacecontact.h>
#include <QtDeclarative/qdeclarative.h>

#include <QObject>

QTM_BEGIN_NAMESPACE

class QDeclarativeContact : public QObject
{
    Q_OBJECT

    Q_ENUMS(ContactType)

    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged);
    Q_PROPERTY(QString value READ value WRITE setValue NOTIFY valueChanged);
    Q_PROPERTY(ContactType type READ type WRITE setType NOTIFY typeChanged);

public:
    enum ContactType {
        Phone,
        Email,
        URL,
        Fax,
        IM,
        Undefined
    };

    explicit QDeclarativeContact(QObject* parent = 0);
    explicit QDeclarativeContact(const QPlaceContact &value, QObject* parent = 0);
    ~QDeclarativeContact();

    QPlaceContact contact() const;
    void setContact(const QPlaceContact &value);

    QString description() const;
    void setDescription(const QString& data);
    ContactType type() const;
    void setType(const ContactType &data);
    QString value() const;
    void setValue(const QString& data);

signals:
    void descriptionChanged();
    void valueChanged();
    void typeChanged();

private:
    QPlaceContact m_value;
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeContact));

#endif // QDECLARATIVECONTACT_P_H
