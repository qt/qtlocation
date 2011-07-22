#include "qdeclarativesupportedcategoriesmodel_p.h"
#include "qgeoserviceprovider.h"

#include <QtDeclarative/QDeclarativeInfo>

#include <qplacemanager.h>

QT_USE_NAMESPACE

/*!
    \qmlclass SupportedCategoryModel QDeclarativeSupportedCategoriesModel
    \brief The SupportedCategoryModel element provides access to suported categories list.
    \inherits QAbstractListModel
    \ingroup qml-places

    SupportedCategoryModel provides a model of categories from the supported categories list.
    The contents of the model can contains parent categories. All categories might have childrens.

    There are two ways of accessing the category data: through model by using views and delegates,
    or alternatively via \l category list property. Of the two, the model access is preferred.

    At the moment only data role provided by the model is \c category (\l Category).
    Through that one can access any data provided by the Category element.

    To use SupportedCategoryModel user need to create it in qml file and connect it to some view
    \code
    import places 1.0

    SupportedCategoriesModel {
        id: categoriesModel
    }

    ListView {
        id: categoriesList
        model: categoriesModel
        delegate: Text {
            text: '<b>name:</b> ' + category.name }
        }
    }
    \endcode

    \sa SearchResultModel, SuggestionModel, {QPlaceManager}
*/
QDeclarativeSupportedCategoriesModel::QDeclarativeSupportedCategoriesModel(QObject *parent)
:   QAbstractListModel(parent), m_plugin(0), m_complete(false)
{
#if defined(QT_PLACESPLUGIN_LOGGING)
    qDebug() << "QDeclarativeSupportedCategoriesModel::QDeclarativeSupportedCategoriesModel";
#endif
    QHash<int, QByteArray> roleNames;
    roleNames = QAbstractItemModel::roleNames();
    roleNames.insert(CategoryRole, "category");
    setRoleNames(roleNames);
}

QDeclarativeSupportedCategoriesModel::~QDeclarativeSupportedCategoriesModel()
{
#if defined(QT_PLACESPLUGIN_LOGGING)
    qDebug() << "QDeclarativeSupportedCategoriesModel::~QDeclarativeSupportedCategoriesModel";
#endif
    foreach (const QString id, m_categoryMap.keys()) {
        delete m_categoryMap.value(id);
        m_categoryMap.remove(id);
    }
}

// From QDeclarativeParserStatus
void QDeclarativeSupportedCategoriesModel::componentComplete()
{
    m_complete = true;
}

/*!
    \qmlsignal SupportedCategoryModel::categoriesChanged()

    This handler is called when the list of categories is changed and
    client should take new data.
*/

int QDeclarativeSupportedCategoriesModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_categories.count();
}

QVariant QDeclarativeSupportedCategoriesModel::data(const QModelIndex& index, int role) const
{
    QPlaceCategory item = m_categories[index.row()];
    QDeclarativeCategory *res = NULL;
    if (m_categoryMap.contains(item.categoryId())) {
        res = m_categoryMap[item.categoryId()];
    }
    switch (role) {
        case Qt::DisplayRole:
            if (res) {
                return res->name();
            } else {
                return QString();
            }
        case CategoryRole:
            if (res) {
                return QVariant::fromValue(res);
            } else {
                return QVariant();
            }
        }
    return QVariant();
}

void QDeclarativeSupportedCategoriesModel::setPlugin(QDeclarativeGeoServiceProvider *plugin)
{
    if (m_plugin == plugin)
        return;

    reset(); // reset the model
    m_plugin = plugin;
    if (m_complete)
        emit pluginChanged();
    QGeoServiceProvider *serviceProvider = m_plugin->sharedGeoServiceProvider();
    QPlaceManager *placeManager = serviceProvider->placeManager();
    if (!placeManager || serviceProvider->error() != QGeoServiceProvider::NoError) {
        qmlInfo(this) << tr("Warning: Plugin does not support places.");
        return;
    }

    m_categories = placeManager->categories();
    convertCategoriesToDeclarative();

    m_response = placeManager->initializeCategories();
    if (m_response) {
        connect(m_response, SIGNAL(finished()), this, SLOT(replyFinished()));
        connect(m_response, SIGNAL(error(QPlaceReply::Error,QString)),
                this, SLOT(replyError(QPlaceReply::Error,QString)));
    }
}

QDeclarativeGeoServiceProvider* QDeclarativeSupportedCategoriesModel::plugin() const
{
    return m_plugin;
}

/*!
    \qmlproperty QDeclarativeListProperty SupportedCategoryModel::categories

    This element holds the list of \l Category elements that the model currently has.
*/

QDeclarativeListProperty<QDeclarativeCategory> QDeclarativeSupportedCategoriesModel::categories()
{
    return QDeclarativeListProperty<QDeclarativeCategory>(this,
                                                          0, // opaque data parameter
                                                          categories_append,
                                                          categories_count,
                                                          categories_at,
                                                          categories_clear);
}

void QDeclarativeSupportedCategoriesModel::categories_append(QDeclarativeListProperty<QDeclarativeCategory> *prop,
                                                             QDeclarativeCategory* category)
{
    Q_UNUSED(prop);
    Q_UNUSED(category);
}

int QDeclarativeSupportedCategoriesModel::categories_count(QDeclarativeListProperty<QDeclarativeCategory> *prop)
{
    return static_cast<QDeclarativeSupportedCategoriesModel*>(prop->object)->m_categories.count();
}

QDeclarativeCategory* QDeclarativeSupportedCategoriesModel::categories_at(QDeclarativeListProperty<QDeclarativeCategory> *prop,
                                                                          int index)
{
    QDeclarativeSupportedCategoriesModel* model = static_cast<QDeclarativeSupportedCategoriesModel*>(prop->object);
    QPlaceCategory item = model->m_categories[index];
    QDeclarativeCategory *res = NULL;
    if (model->m_categoryMap.contains(item.categoryId())) {
        res = model->m_categoryMap[item.categoryId()];
    }
    return res;
}

void QDeclarativeSupportedCategoriesModel::categories_clear(QDeclarativeListProperty<QDeclarativeCategory> *prop)
{
    Q_UNUSED(prop)
}

void QDeclarativeSupportedCategoriesModel::replyFinished()
{
    if (!m_response)
        return;

    m_response->deleteLater();
    m_response = 0;

    if (!m_plugin) {
        qmlInfo(this) << "plugin not set.";
        return;
    }

    QGeoServiceProvider *serviceProvider = m_plugin->sharedGeoServiceProvider();
    if (!serviceProvider)
        return;

    QPlaceManager *placeManager = serviceProvider->placeManager();
    if (!placeManager) {
        qmlInfo(this) << tr("Places not supported by %1 Plugin.").arg(m_plugin->name());
        return;
    }

    beginResetModel();
    m_categories = placeManager->categories();
    convertCategoriesToDeclarative();
    endResetModel();
    emit categoriesChanged();
}

void QDeclarativeSupportedCategoriesModel::replyError(QPlaceReply::Error error,
                                                 const QString &errorString)
{
    Q_UNUSED(error);
    Q_UNUSED(errorString);
}

void QDeclarativeSupportedCategoriesModel::convertCategoriesToDeclarative()
{
    foreach (const QString id, m_categoryMap.keys()) {
        delete m_categoryMap.value(id);
        m_categoryMap.remove(id);
    }
    m_categoryMap.clear();

    foreach (const QPlaceCategory& category, m_categories) {
        QDeclarativeCategory* declarativeCategory = new QDeclarativeCategory(category, this);
        m_categoryMap.insert(category.categoryId(), declarativeCategory);
    }
}
