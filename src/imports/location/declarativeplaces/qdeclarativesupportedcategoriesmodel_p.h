#ifndef QDECLARATIVESUPPORTEDCATEGORIESMODEL_H
#define QDECLARATIVESUPPORTEDCATEGORIESMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QDeclarativeListProperty>

#include <qplacecategory.h>
#include <qplacemanager.h>
#include <qplacereply.h>

#include "qdeclarativecategory_p.h"

QT_BEGIN_NAMESPACE

class QDeclarativeSupportedCategoriesModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeCategory> categories READ categories NOTIFY categoriesChanged)
public:
    explicit QDeclarativeSupportedCategoriesModel(QObject *parent = 0);
    virtual ~QDeclarativeSupportedCategoriesModel();

    QDeclarativeListProperty<QDeclarativeCategory> categories();
    static void categories_append(QDeclarativeListProperty<QDeclarativeCategory> *prop,
                                  QDeclarativeCategory* category);
    static int categories_count(QDeclarativeListProperty<QDeclarativeCategory> *prop);
    static QDeclarativeCategory* categories_at(QDeclarativeListProperty<QDeclarativeCategory> *prop, int index);
    static void categories_clear(QDeclarativeListProperty<QDeclarativeCategory> *prop);

    // From QAbstractListModel
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    // Roles for exposing data via model. Only one role because
    // everything can be accessed via QDeclarativeLandmark
    enum Roles {
        CategoryRole = Qt::UserRole + 500
    };
signals:
    void categoriesChanged();

private slots:
    void replyFinished();
    void replyError(QPlaceReply::Error error, const QString &errorString);

private:
    void convertCategoriesToDeclarative();

    QPlaceManager *m_manager;
    QPlaceReply *m_response;
    QList<QPlaceCategory> m_categories;
    QMap<QString, QDeclarativeCategory*> m_categoryMap;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeSupportedCategoriesModel));

#endif // QDECLARATIVESUPPORTEDCATEGORIESMODEL_H
