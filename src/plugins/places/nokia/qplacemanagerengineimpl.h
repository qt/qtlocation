#ifndef QPLACEMANAGERENGINEIMPL_H
#define QPLACEMANAGERENGINEIMPL_H

#include <qplacemanagerengine.h>

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

    QPlaceSearchReply *searchForPlaces(const QPlaceSearchQuery &query, QPlaceManager::VisibilityScope scope);
    QPlaceManager::VisibilityScopes supportedSearchVisibilityScopes() const;

    QPlaceSearchReply *recommendations(const QPlace &place, const QPlaceSearchQuery &query);
    QPlaceTextPredictionReply *textPredictions(const QPlaceSearchQuery &query);

    QPlaceManager::ConnectivityModes connectivityMode() const;
    void setConnectivityMode(QPlaceManager::ConnectivityModes connectivityMode);
    QPlaceManager::ConnectivityModes supportedConnectivityModes() const;

    QPlaceReply *savePlace(QPlace *place, QPlaceManager::VisibilityScope scope);
    QPlaceManager::VisibilityScopes supportedSaveVisibilityScopes() const;

    QPlaceReply *removePlace(const QPlace &place);

    QPlaceReply *initializeCategories(const QString &categorySystemId);
    QList<QPlaceCategory> categories() const;

private slots:
    void processingError(QPlaceReply *reply, const QPlaceReply::Error &error, const QString &errorMessage);
    void processingFinished(QPlaceReply *reply);
};

QTM_END_NAMESPACE

#endif // QPLACEMANAGERENGINEIMPL_H
