#ifndef QDECLARATIVERECOMMENDATIONMODEL_P_H
#define QDECLARATIVERECOMMENDATIONMODEL_P_H

#include <QtCore/QObject>
#include <QtCore/QAbstractListModel>
#include <QtLocation/QPlaceManager>
#include <QtLocation/QPlaceSearchRequest>
#include <QtLocation/QPlaceSearchResult>

#include "qdeclarativegeoserviceprovider_p.h"
#include "qdeclarativecoordinate_p.h"
#include "qdeclarativegeoboundingbox_p.h"
#include "qdeclarativeplace_p.h"

QT_BEGIN_NAMESPACE

class QDeclarativeRecommendationModel : public QAbstractListModel, public QDeclarativeParserStatus
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeGeoServiceProvider *plugin READ plugin WRITE setPlugin NOTIFY pluginChanged)
    Q_PROPERTY(QString placeId READ placeId WRITE setPlaceId NOTIFY placeIdChanged)
    Q_PROPERTY(QDeclarativeGeoBoundingArea* searchArea READ searchArea WRITE setSearchArea NOTIFY searchAreaChanged)
    Q_PROPERTY(int offset READ offset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(int limit READ limit WRITE setLimit NOTIFY limitChanged)
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

    explicit QDeclarativeRecommendationModel(QObject *parent = 0);
    ~QDeclarativeRecommendationModel();

    // From QDeclarativeParserStatus
    virtual void classBegin() {}
    virtual void componentComplete();

    void setPlugin(QDeclarativeGeoServiceProvider *plugin);
    QDeclarativeGeoServiceProvider* plugin() const;

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

    QString placeId() const;
    void setPlaceId(const QString &placeId);
    QDeclarativeGeoBoundingArea *searchArea() const;
    void setSearchArea(QDeclarativeGeoBoundingArea *area);
    int offset() const;
    void setOffset(const int &offset);
    int limit() const;
    void setLimit(const int &limit);

    bool executing() const;

    Q_INVOKABLE void executeQuery();
    Q_INVOKABLE void cancelRequest();

signals:
    void pluginChanged();
    void queryFinished(const int &error);

    void placeIdChanged();
    void searchAreaChanged();
    void offsetChanged();
    void limitChanged();
    void executingChanged();

private slots:
    void replyFinished();
    void replyError(QPlaceReply::Error error, const QString &errorString);

private:
    void cancelPreviousRequest();
    void connectNewResponse(QPlaceSearchReply *newResponse);

private:
    QDeclarativeGeoBoundingArea *m_searchArea;

    QList<QPlaceSearchResult> m_results;
    QMap<QString, QDeclarativePlace *> m_places;

    QPlaceSearchRequest m_queryParameters;

    QPlaceSearchReply *m_response;

    QDeclarativeGeoServiceProvider *m_plugin;
    bool m_complete;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeRecommendationModel));

#endif // QDECLARATIVERECOMMENDATIONMODEL_P_H
