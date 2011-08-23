#ifndef QDECLARATIVESEARCHRESULTMODEL_P_H
#define QDECLARATIVESEARCHRESULTMODEL_P_H

#include <QtCore/QMap>
#include <QtCore/QAbstractListModel>
#include <QtLocation/QPlaceManager>
#include <QtLocation/QPlaceSearchRequest>
#include <QtLocation/QPlaceSearchResult>

#include "qdeclarativegeoboundingarea_p.h"
#include "qdeclarativeplace_p.h"

QT_BEGIN_NAMESPACE

class QDeclarativeSearchResultModel : public QAbstractListModel, public QDeclarativeParserStatus
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeGeoServiceProvider *plugin READ plugin WRITE setPlugin NOTIFY pluginChanged)
    Q_PROPERTY(QString searchTerm READ searchTerm WRITE setSearchTerm NOTIFY searchTermChanged);
    Q_PROPERTY(QDeclarativeCategory* searchCategory READ searchCategory WRITE setSearchCategory NOTIFY searchCategoryChanged);
    Q_PROPERTY(QDeclarativeGeoBoundingArea *searchArea READ searchArea WRITE setSearchArea NOTIFY searchAreaChanged);
    Q_PROPERTY(int offset READ offset WRITE setOffset NOTIFY offsetChanged);
    Q_PROPERTY(int limit READ limit WRITE setLimit NOTIFY limitChanged);
    Q_PROPERTY(int didYouMean READ didYouMean WRITE setDidYouMean NOTIFY didYouMeanChanged);
    Q_PROPERTY(bool executing READ executing NOTIFY executingChanged)

    Q_ENUMS(SearchResultType LocationMatchType)

    Q_INTERFACES(QDeclarativeParserStatus)

public:
    enum SearchResultType {
        Place = QPlaceSearchResult::Place,
        DidYouMeanSuggestion = QPlaceSearchResult::DidYouMeanSuggestion,
        UnknownSearchResult = QPlaceSearchResult::UnknownSearchResult
    };

    enum LocationMatchType {
        PointAddress = QPlaceSearchResult::PointAddress,
        Interpolated = QPlaceSearchResult::Interpolated,
        UndefinedLocationMatch = QPlaceSearchResult::UndefinedLocationMatch
    };

    explicit QDeclarativeSearchResultModel(QObject *parent = 0);
    ~QDeclarativeSearchResultModel();

    // From QDeclarativeParserStatus
    virtual void classBegin() {}
    virtual void componentComplete();

    // From QAbstractListModel
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    enum Roles {
        SearchResultType = Qt::UserRole,
        SearchResultRelevance,
        SearchResultDistance,
        SearchResultHeading,
        SearchResultMatchType,
        SearchResultAdditionalData,
        SearchResultPlace,
        SearchResultDidYouMean
    };

    void setPlugin(QDeclarativeGeoServiceProvider *plugin);
    QDeclarativeGeoServiceProvider* plugin() const;

    QString searchTerm() const;
    void setSearchTerm(const QString &searchTerm);
    QDeclarativeCategory *searchCategory();
    void setSearchCategory(QDeclarativeCategory *searchCategory);
    QDeclarativeGeoBoundingArea *searchArea() const;
    void setSearchArea(QDeclarativeGeoBoundingArea *searchArea);
    int offset() const;
    void setOffset(const int &offset);
    int limit() const;
    void setLimit(const int &limit);
    int didYouMean() const;
    void setDidYouMean(const int &dym);

    bool executing() const;

    Q_INVOKABLE void executeQuery();
    Q_INVOKABLE void cancelRequest();

    Q_INVOKABLE void clearCategories();
    Q_INVOKABLE void clearSearchTerm();

signals:
    void pluginChanged();
    void queryFinished(const int &error);

    void searchTermChanged();
    void searchCategoryChanged();
    void searchAreaChanged();
    void offsetChanged();
    void limitChanged();
    void didYouMeanChanged();
    void executingChanged();

private slots:
    void replyFinished();
    void replyError(QPlaceReply::Error error, const QString &errorString);

private:
    void cancelPreviousRequest();
    void connectNewResponse(QPlaceSearchReply *newResponse);

private:
    QDeclarativeCoordinate m_center;
    QDeclarativeGeoBoundingArea *m_searchArea;
    QDeclarativeCategory m_category;

    QList<QPlaceSearchResult> m_results;
    QMap<QString, QDeclarativePlace *> m_places;

    QPlaceSearchRequest m_queryParameters;

    QPlaceSearchReply *m_response;

    QDeclarativeGeoServiceProvider *m_plugin;
    bool m_complete;
};

QT_END_NAMESPACE

#endif // QDECLARATIVESEARCHRESULTMODEL_P_H
