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

class PlaceCategoryTree
{
public:
    PlaceCategoryTree();
    ~PlaceCategoryTree();

    QSharedPointer<QDeclarativeCategory> category;
    QHash<QString, PlaceCategoryTree> subCategories;
};

class QDeclarativeSupportedCategoriesModel : public QAbstractItemModel, public QDeclarativeParserStatus
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeGeoServiceProvider *plugin READ plugin WRITE setPlugin NOTIFY pluginChanged)
    Q_PROPERTY(bool hierarchical READ hierarchical WRITE setHierarchical NOTIFY hierarchicalChanged)

    Q_INTERFACES(QDeclarativeParserStatus)

public:
    explicit QDeclarativeSupportedCategoriesModel(QObject *parent = 0);
    virtual ~QDeclarativeSupportedCategoriesModel();

    // From QDeclarativeParserStatus
    virtual void classBegin() {}
    virtual void componentComplete();

    // From QAbstractItemModel
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;

    QVariant data(const QModelIndex &index, int role) const;
    // Roles for exposing data via model. Only one role because
    // everything can be accessed via QDeclarativeLandmark
    enum Roles {
        CategoryRole = Qt::UserRole + 500
    };

    void setPlugin(QDeclarativeGeoServiceProvider *plugin);
    QDeclarativeGeoServiceProvider* plugin() const;

    void setHierarchical(bool hierarchical);
    bool hierarchical() const;

signals:
    void pluginChanged();
    void hierarchicalChanged();

private slots:
    void replyFinished();
    void replyError(QPlaceReply::Error error, const QString &errorString);

private:
    void updateCategories();
    QHash<QString, PlaceCategoryTree> populatedCategories(QPlaceManager *manager, const QPlaceCategory &parent = QPlaceCategory());
    PlaceCategoryTree findCategoryTreeByCategory(QDeclarativeCategory *category, const PlaceCategoryTree &tree) const;
    QDeclarativeCategory *findParentCategoryByCategory(QDeclarativeCategory *category, const PlaceCategoryTree &tree) const;

    QPlaceReply *m_response;
    PlaceCategoryTree m_categoryTree;

    QDeclarativeGeoServiceProvider *m_plugin;
    bool m_hierarchical;
    bool m_complete;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeSupportedCategoriesModel));

#endif // QDECLARATIVESUPPORTEDCATEGORIESMODEL_H
