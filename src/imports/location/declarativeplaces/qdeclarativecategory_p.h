#ifndef QDECLARATIVECATEGORY_P_H
#define QDECLARATIVECATEGORY_P_H

#include <QtDeclarative/qdeclarative.h>
#include <QObject>

#include <qplacecategory.h>

QT_BEGIN_NAMESPACE

class QDeclarativeCategory : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString categoryId READ categoryId WRITE setCategoryId NOTIFY categoryIdChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

public:
    explicit QDeclarativeCategory(QObject* parent = 0);
    explicit QDeclarativeCategory(const QPlaceCategory &category, QObject* parent = 0);
    ~QDeclarativeCategory();

    QPlaceCategory category();
    void setCategory(const QPlaceCategory &category);

    QString categoryId() const;
    void setCategoryId(const QString &catID);
    QString description() const;
    void setDescription(const QString &description);
    QString name() const;
    void setName(const QString &name);

signals:
    void categoryIdChanged();
    void descriptionChanged();
    void nameChanged();

private:
    QPlaceCategory m_category;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeCategory));

#endif // QDECLARATIVECATEGORY_P_H
