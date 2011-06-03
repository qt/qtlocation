#ifndef QPLACEMANAGERENGINEIMPL_H
#define QPLACEMANAGERENGINEIMPL_H

#include "../qplacemanagerengine.h"

QTM_BEGIN_NAMESPACE

class QPlaceManagerEngineImpl : public QPlaceManagerEngine
{
    Q_OBJECT
public:

    QPlaceManagerEngineImpl(QObject *parent = 0);
    ~QPlaceManagerEngineImpl();

    QString managerName() const;

    QPlaceDetailsReply *getPlaceDetails(const QString &placeId);

    QPlaceMediaReply * getMedia(const QPlace &place, const QPlaceQuery &query);

    QPlaceReply *postRating(const QPlace &place, qreal value);

    QPlaceReviewReply *getReviews(const QPlace &place, const QPlaceQuery &query);

    QPlaceSearchReply *searchForPlaces(const QPlaceSearchQuery &query);
    QPlaceSearchReply *recommendations(const QPlace &place, const QPlaceSearchQuery &query);
    QPlaceTextPredictionReply *textPredictions(const QPlaceSearchQuery &query);

    QPlaceManager::ConnectivityMode connectivityMode() const;
    void setConnectivityMode(QPlaceManager::ConnectivityMode connectivityMode);
    QList<QPlaceManager::ConnectivityMode> supportedConnectivityModes() const;

    QPlaceManager::SearchVisibilityScope searchVisibilityScope() const;
    void setSearchVisbilityScope(QPlaceManager::SearchVisibilityScope scope);
    QList<QPlaceManager::SearchVisibilityScope> supportedSearchVisibilityScopes() const;

    QPlaceReply *initializeCategories(const QString &categorySystemId);
    QList<QPlaceCategory> categories() const;

private slots:
    void processingError(QPlaceReply *reply, const QPlaceReply::Error &error, const QString &errorMessage);
    void processingFinished(QPlaceReply *reply);
};

QTM_END_NAMESPACE

#endif // QPLACEMANAGERENGINEIMPL_H
