#ifndef QDECLARATIVEBUSINESSFEATURE_P_H
#define QDECLARATIVEBUSINESSFEATURE_P_H

#include <qplacebusinessfeature.h>
#include <QtDeclarative/qdeclarative.h>

#include <QObject>

QTM_BEGIN_NAMESPACE

class QDeclarativeBusinessFeature : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString key READ key WRITE setKey NOTIFY keyChanged);
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged);
    Q_PROPERTY(QString value READ value WRITE setValue NOTIFY valueChanged);

public:
    explicit QDeclarativeBusinessFeature(QObject* parent = 0);
    explicit QDeclarativeBusinessFeature(const QPlaceBusinessFeature &value, QObject* parent = 0);
    ~QDeclarativeBusinessFeature();

    QPlaceBusinessFeature businessFeature() const;
    void setBusinessFeature(const QPlaceBusinessFeature &value);

    QString key() const;
    void setKey(const QString &key);

    QString label() const;
    void setLabel(const QString &label);

    QString value() const;
    void setValue(const QString &value);


signals:
    void keyChanged();
    void labelChanged();
    void valueChanged();

private:
    QPlaceBusinessFeature m_value;
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeBusinessFeature));

#endif // QDECLARATIVEBUSINESSFEATURE_P_H
