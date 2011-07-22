#ifndef QDECLARATIVESUPPORTEDCATEGORIESMODEL_H
#define QDECLARATIVESUPPORTEDCATEGORIESMODEL_H

#include "qdeclarativegeoserviceprovider_p.h"

#include <QObject>
#include <QAbstractListModel>
#include <QDeclarativeListProperty>

#include <qplacecategory.h>
#include <qplacemanager.h>
#include <qplacereply.h>

#include "qdeclarativecategory_p.h"

QT_BEGIN_NAMESPACE

class QGeoServiceProvider;

class QDeclarativeSupportedCategoriesModel : public QAbstractListModel, public QDeclarativeParserStatus
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeGeoServiceProvider *plugin READ plugin WRITE setPlugin NOTIFY pluginChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeCategory> categories READ categories NOTIFY categoriesChanged)

    Q_INTERFACES(QDeclarativeParserStatus)

public:
    explicit QDeclarativeSupportedCategoriesModel(QObject *parent = 0);
    virtual ~QDeclarativeSupportedCategoriesModel();

    // From QDeclarativeParserStatus
    virtual void classBegin() {}
    virtual void componentComplete();

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

    void setPlugin(QDeclarativeGeoServiceProvider *plugin);
    QDeclarativeGeoServiceProvider* plugin() const;

signals:
    void categoriesChanged();
    void pluginChanged();

private slots:
    void replyFinished();
    void replyError(QPlaceReply::Error error, const QString &errorString);

private:
    void convertCategoriesToDeclarative();

    QPlaceReply *m_response;
    QList<QPlaceCategory> m_categories;
    QMap<QString, QDeclarativeCategory*> m_categoryMap;

    QDeclarativeGeoServiceProvider *m_plugin;
    bool m_complete;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeSupportedCategoriesModel));

#endif // QDECLARATIVESUPPORTEDCATEGORIESMODEL_H
