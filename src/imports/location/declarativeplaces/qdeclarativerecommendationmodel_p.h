#ifndef QDECLARATIVERECOMMENDATIONMODEL_P_H
#define QDECLARATIVERECOMMENDATIONMODEL_P_H

#include <QObject>
#include <QAbstractListModel>
#include <QDeclarativeListProperty>

#include <qplacemanager.h>

#include "qdeclarativecoordinate_p.h"
#include "qdeclarativegeoboundingbox_p.h"
#include "qdeclarativesearchresult_p.h"

QT_BEGIN_NAMESPACE

class QDeclarativeRecommendationModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString placeId READ placeId WRITE setPlaceId NOTIFY placeIdChanged);
    Q_PROPERTY(QDeclarativeGeoBoundingArea* searchArea READ searchArea WRITE setSearchArea NOTIFY searchAreaChanged);
    Q_PROPERTY(int offset READ offset WRITE setOffset NOTIFY offsetChanged);
    Q_PROPERTY(int limit READ limit WRITE setLimit NOTIFY limitChanged);

    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeSearchResult> results READ results NOTIFY resultsChanged)
public:
    explicit QDeclarativeRecommendationModel(QObject *parent = 0);
    ~QDeclarativeRecommendationModel();

    QDeclarativeListProperty<QDeclarativeSearchResult> results();
    static void results_append(QDeclarativeListProperty<QDeclarativeSearchResult> *prop,
                                  QDeclarativeSearchResult* result);
    static int results_count(QDeclarativeListProperty<QDeclarativeSearchResult> *prop);
    static QDeclarativeSearchResult* results_at(QDeclarativeListProperty<QDeclarativeSearchResult> *prop, int index);
    static void results_clear(QDeclarativeListProperty<QDeclarativeSearchResult> *prop);

    // From QAbstractListModel
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    enum Roles {
        SearchResultRole = Qt::UserRole + 500
    };

    QString placeId() const;
    void setPlaceId(const QString &placeId);
    QDeclarativeGeoBoundingArea *searchArea() const;
    void setSearchArea(QDeclarativeGeoBoundingArea *area);
    int offset() const;
    void setOffset(const int &offset);
    int limit() const;
    void setLimit(const int &limit);

    Q_INVOKABLE void executeQuery();
    Q_INVOKABLE void cancelRequest();

signals:
    void queryFinished(const int &error);

    void placeIdChanged();
    void searchAreaChanged();
    void offsetChanged();
    void limitChanged();
    void resultsChanged();

private slots:
    void replyFinished();
    void replyError(QPlaceReply::Error error, const QString &errorString);

private:
    void cancelPreviousRequest();
    void connectNewResponse(QPlaceSearchReply *newResponse);
    void convertResultsToDeclarative();

private:
    QDeclarativeGeoBoundingArea *m_searchArea;
    QList<QDeclarativeSearchResult*> m_results;

    QPlaceSearchQuery m_queryParameters;

    QPlaceManager *m_manager;
    QPlaceSearchReply *m_response;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeRecommendationModel));

#endif // QDECLARATIVERECOMMENDATIONMODEL_P_H
