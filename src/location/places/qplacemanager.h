#ifndef QPLACEMANAGER_H
#define QPLACEMANAGER_H

#include "qmobilityglobal.h"
#include "qplacemediaobject.h"
#include "qplacemediareply.h"
#include "qplacereply.h"
#include "qplacesearchquery.h"
#include "qplacesearchreply.h"
#include "qplacedetailsreply.h"
#include "qplacereviewreply.h"
#include "qplacetextpredictionreply.h"

#include <QVector>
#include <QString>
#include <QObject>

QTM_BEGIN_NAMESPACE

class QPlaceManagerEngine;

class QPlaceManagerPrivate;
class Q_LOCATION_EXPORT QPlaceManager : public QObject
{
    Q_OBJECT
public:
    enum ConnectivityMode {
        NoConnectivity,
        OnlineMode,
        OfflineMode,
        HybridMode
    };

    enum SearchVisibilityScope {
        PublicSearch,
        PrivateSearch,
        PublicAndPrivateSearch
    };

    enum ManagerFeature {
        ImportFeature,
        ExportFeature,
        CheckInFeature,
        PostMediaFeature,
        PostRatingFeature,
        SuggestionFeature,
        ReportPlaceFeature,
        AuthenticationFeature,
        CreatePlaceFeature,
        UpdatePlaceFeature
    };

    QPlaceManager(QObject *parent = 0);
    ~QPlaceManager();
    QString managerName() const;

    QPlaceDetailsReply *getPlaceDetails(const QString &placeId) const;
    QPlaceReply *postRating(const QPlace &place, qreal value);

    QPlaceReviewReply *getReviews(const QPlace &place, const QPlaceQuery &query) const;

    QPlaceMediaReply *getMedia(const QPlace &place, const QPlaceQuery &query) const;

    QPlaceSearchReply *searchForPlaces(const QPlaceSearchQuery &query) const;
    QPlaceSearchReply *recommendations(const QPlace &place, const QPlaceSearchQuery &query) const;

    QPlaceTextPredictionReply *textPredictions(const QPlaceSearchQuery &query) const;

    ConnectivityMode connectivityMode() const;
    void setConnectivityMode(ConnectivityMode connectivityMode);
    QList<ConnectivityMode> supportedConnectivityModes() const;

    SearchVisibilityScope searchVisibilityScope() const;
    void setSearchVisbilityScopes(SearchVisibilityScope scope);
    QList<SearchVisibilityScope> supportedSearchVisibilityScopes() const;

    QPlaceReply *initializeCategories(const QString &categorySystemId = QString());
    QList<QPlaceCategory> categories() const;
    QPlaceCategory createCategory() const;

Q_SIGNALS:
    void finished(QPlaceReply *reply);
    void error(QPlaceReply *, QPlaceReply::Error error, const QString &errorString = QString());

private:
    Q_DISABLE_COPY(QPlaceManager)
    QPlaceManagerPrivate* d;
};

QTM_END_NAMESPACE

#endif // QPLACEMANAGER_H
