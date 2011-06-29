#ifndef QDECLARATIVESEARCHRESULTMODEL_P_H
#define QDECLARATIVESEARCHRESULTMODEL_P_H

#include <QObject>
#include <QAbstractListModel>
#include <QDeclarativeListProperty>

#include <qplacemanager.h>

#include "qdeclarativecoordinate_p.h"
#include "qdeclarativegeoboundingbox_p.h"
#include "qdeclarativesearchresult_p.h"

QTM_BEGIN_NAMESPACE

class QDeclarativeSearchResultModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString searchTerm READ searchTerm WRITE setSearchTerm NOTIFY searchTermChanged);
    Q_PROPERTY(QDeclarativeCategory* searchCategory READ searchCategory WRITE setSearchCategory NOTIFY searchCategoryChanged);
    Q_PROPERTY(QDeclarativeCoordinate* searchCenter READ searchCenter WRITE setSearchCenter NOTIFY searchCenterChanged);
    Q_PROPERTY(QDeclarativeGeoBoundingBox* boundingBox READ boundingBox WRITE setBoundingBox NOTIFY boundingBoxChanged);
    Q_PROPERTY(int offset READ offset WRITE setOffset NOTIFY offsetChanged);
    Q_PROPERTY(int limit READ limit WRITE setLimit NOTIFY limitChanged);
    Q_PROPERTY(int didYouMean READ didYouMean WRITE setDidYouMean NOTIFY didYouMeanChanged);

    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeSearchResult> results READ results NOTIFY resultsChanged)
public:
    explicit QDeclarativeSearchResultModel(QObject *parent = 0);
    ~QDeclarativeSearchResultModel();

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

    QString searchTerm() const;
    void setSearchTerm(const QString &searchTerm);
    QDeclarativeCategory *searchCategory();
    void setSearchCategory(QDeclarativeCategory *searchCategory);
    QDeclarativeCoordinate *searchCenter();
    void setSearchCenter(QDeclarativeCoordinate *searchCenter);
    QDeclarativeGeoBoundingBox *boundingBox();
    void setBoundingBox(QDeclarativeGeoBoundingBox *boundingBox);
    int offset() const;
    void setOffset(const int &offset);
    int limit() const;
    void setLimit(const int &limit);
    int didYouMean() const;
    void setDidYouMean(const int &dym);

    Q_INVOKABLE void executeQuery();
    Q_INVOKABLE void cancelRequest();

    Q_INVOKABLE void clearCategories();
    Q_INVOKABLE void clearSearchTerm();

signals:
    void queryFinished(const int &error);

    void searchTermChanged();
    void searchCategoryChanged();
    void searchCenterChanged();
    void boundingBoxChanged();
    void offsetChanged();
    void limitChanged();
    void didYouMeanChanged();
    void resultsChanged();

private slots:
    void replyFinished();
    void replyError(QPlaceReply::Error error, const QString &errorString);

private:
    void cancelPreviousRequest();
    void connectNewResponse(QPlaceSearchReply *newResponse);
    void convertResultsToDeclarative();

private:
    QDeclarativeCoordinate m_center;
    QDeclarativeGeoBoundingBox m_boundingBox;
    QDeclarativeCategory m_category;
    QList<QDeclarativeSearchResult*> m_results;

    QPlaceSearchQuery m_queryParameters;

    QPlaceManager *m_manager;
    QPlaceSearchReply *m_response;
};

QTM_END_NAMESPACE

#endif // QDECLARATIVESEARCHRESULTMODEL_P_H
