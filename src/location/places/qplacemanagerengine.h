#ifndef QPLACEMANAGERENGINE_H
#define QPLACEMANAGERENGINE_H

#include "qplacemanager.h"

QTM_BEGIN_NAMESPACE

class Q_LOCATION_EXPORT QPlaceManagerEngine : public QObject
{
    Q_OBJECT
public:

    QPlaceManagerEngine(QObject *parent = 0);
    virtual ~QPlaceManagerEngine();

    virtual QString managerName() const = 0;

    virtual QPlaceDetailsReply *getPlaceDetails(const QString &placeId) = 0;

    virtual QPlaceMediaReply *getMedia(const QPlace &place, const QPlaceQuery &query) = 0;

    virtual QPlaceReply *postRating(const QPlace &place, qreal value) = 0;

    virtual QPlaceReviewReply *getReviews(const QPlace &place, const QPlaceQuery &query) = 0;

    virtual QPlaceSearchReply *searchForPlaces(const QPlaceSearchQuery &query, QPlaceManager::VisibilityScope scope) = 0;
    virtual QPlaceManager::VisibilityScopes supportedSearchVisibilityScopes() const = 0;

    virtual QPlaceSearchReply *recommendations(const QPlace &place, const QPlaceSearchQuery &query) = 0;
    virtual QPlaceTextPredictionReply *textPredictions(const QPlaceSearchQuery &query) = 0;

    virtual QPlaceManager::ConnectivityModes connectivityMode() const = 0;
    virtual void setConnectivityMode(QPlaceManager::ConnectivityModes mode) = 0;
    virtual QPlaceManager::ConnectivityModes supportedConnectivityModes() const = 0;

    virtual QPlaceReply *savePlace(QPlace *place, QPlaceManager::VisibilityScope scope) = 0;
    virtual QPlaceManager::VisibilityScopes supportedSaveVisibilityScopes() const = 0;

    virtual QPlaceReply *removePlace(const QPlace &place) = 0;

    virtual QPlaceReply *initializeCategories(const QString &categorySystemId) = 0;
    virtual QList<QPlaceCategory> categories() const = 0;

Q_SIGNALS:
    void finished(QPlaceReply *reply);
    void error(QPlaceReply *, QPlaceReply::Error error, QString errorString = QString());

};

QTM_END_NAMESPACE

#endif
