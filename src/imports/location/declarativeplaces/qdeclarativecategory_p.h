#ifndef QDECLARATIVECATEGORY_P_H
#define QDECLARATIVECATEGORY_P_H

#include <QtDeclarative/qdeclarative.h>
#include <QObject>

#include <qplacecategory.h>
#include "qdeclarativealternativevalue_p.h"

QTM_BEGIN_NAMESPACE

class QDeclarativeCategory : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString categoryId READ categoryId WRITE setCategoryId NOTIFY categoryIdChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeAlternativeValue> alternativeNames READ alternativeNames NOTIFY alternativeNamesChanged)

public:
    explicit QDeclarativeCategory(QObject* parent = 0);
    explicit QDeclarativeCategory(const QPlaceCategory &category, QObject* parent = 0);
    ~QDeclarativeCategory();

    QPlaceCategory category() const;
    void setCategory(const QPlaceCategory &category);

    QDeclarativeListProperty<QDeclarativeAlternativeValue> alternativeNames();
    static void alternativeValue_append(QDeclarativeListProperty<QDeclarativeAlternativeValue> *prop,
                                  QDeclarativeAlternativeValue* value);
    static int alternativeValue_count(QDeclarativeListProperty<QDeclarativeAlternativeValue> *prop);
    static QDeclarativeAlternativeValue* alternativeValue_at(QDeclarativeListProperty<QDeclarativeAlternativeValue> *prop, int index);
    static void alternativeValue_clear(QDeclarativeListProperty<QDeclarativeAlternativeValue> *prop);

    QString categoryId() const;
    void setCategoryId(const QString &catID);
    QString description() const;
    void setDescription(const QString &description);
    QString name() const;
    void setName(const QString &name);

    Q_INVOKABLE bool isEmpty() const;

signals:
    void categoryIdChanged();
    void descriptionChanged();
    void nameChanged();
    void alternativeNamesChanged();

private:
    void synchronizeAlternativeValues();

private:
    QList<QDeclarativeAlternativeValue*> m_alternativeValues;
    QPlaceCategory m_category;
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeCategory));

#endif // QDECLARATIVECATEGORY_P_H
