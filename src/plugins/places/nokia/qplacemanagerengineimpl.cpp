#include "qplacemanagerengineimpl.h"

#include "qplacecategoriesrepository.h"
#include "qplacetextpredictionreplyimpl.h"
#include "qplacesearchreplyimpl.h"
#include "qplacereviewreplyimpl.h"
#include "qplacemediareplyimpl.h"
#include "qplacerecommendationreplyimpl.h"
#include "qplacedetailsreplyimpl.h"
#include "qplaceratingreplyimpl.h"
#include "qplacerestmanager.h"
#include "qplacerestreply.h"

QTM_USE_NAMESPACE

const char *MANAGER_NAME = "com.nokia.places/0.9";

QPlaceManagerEngineImpl::QPlaceManagerEngineImpl(QObject *parent)
    : QPlaceManagerEngine(parent)
{
}

QPlaceManagerEngineImpl::~QPlaceManagerEngineImpl()
{
}

QString QPlaceManagerEngineImpl::managerName() const
{
    return MANAGER_NAME;
}

QPlaceDetailsReply *QPlaceManagerEngineImpl::getPlaceDetails(const QString &placeId)
{
    QPlaceDetailsReplyImpl *reply = NULL;
    QPlaceRestReply *restReply = QPlaceRestManager::instance()->sendPlaceRequest(placeId);
    if (restReply) {
        reply = new QPlaceDetailsReplyImpl(restReply, this);
        connect(reply, SIGNAL(processingError(QPlaceReply*,QPlaceReply::Error,QString)),
                this, SLOT(processingError(QPlaceReply*,QPlaceReply::Error,QString)));
        connect(reply, SIGNAL(processingFinished(QPlaceReply*)),
                this, SLOT(processingFinished(QPlaceReply*)));
    }
    return reply;
}

QPlaceMediaReply *QPlaceManagerEngineImpl::getMedia(const QPlace &place, const QPlaceQuery &query)
{
    QPlaceMediaReplyImpl *reply = NULL;
    QPlaceRestReply *restReply = QPlaceRestManager::instance()->sendPlaceImagesRequest(place.placeId(),
                                                                                       query);
    if (restReply) {
        reply = new QPlaceMediaReplyImpl(restReply, this);
        reply->setStartNumber(query.offset());
        connect(reply, SIGNAL(processingError(QPlaceReply*,QPlaceReply::Error,QString)),
                this, SLOT(processingError(QPlaceReply*,QPlaceReply::Error,QString)));
        connect(reply, SIGNAL(processingFinished(QPlaceReply*)),
                this, SLOT(processingFinished(QPlaceReply*)));
    }
    return reply;
}

QPlaceReply *QPlaceManagerEngineImpl::postRating(const QPlace &place, qreal value)
{
    QPlaceRatingReplyImpl *reply = NULL;
    QPlaceRestReply *restReply = QPlaceRestManager::instance()->postRatingRequest(place.placeId(),
                                                                                  QString(),
                                                                                  value);
    if (restReply) {
        reply = new QPlaceRatingReplyImpl(restReply, this);
        connect(reply, SIGNAL(processingError(QPlaceReply*,QPlaceReply::Error,QString)),
                this, SLOT(processingError(QPlaceReply*,QPlaceReply::Error,QString)));
        connect(reply, SIGNAL(processingFinished(QPlaceReply*)),
                this, SLOT(processingFinished(QPlaceReply*)));
    }
    return reply;
}

QPlaceReviewReply *QPlaceManagerEngineImpl::getReviews(const QPlace &place, const QPlaceQuery &query)
{
    QPlaceReviewReplyImpl *reply = NULL;
    QPlaceRestReply *restReply = QPlaceRestManager::instance()->sendPlaceReviewRequest(place.placeId(),
                                                                                       query);
    if (restReply) {
        reply = new QPlaceReviewReplyImpl(restReply, this);
        reply->setStartNumber(query.offset());
        connect(reply, SIGNAL(processingError(QPlaceReply*,QPlaceReply::Error,QString)),
                this, SLOT(processingError(QPlaceReply*,QPlaceReply::Error,QString)));
        connect(reply, SIGNAL(processingFinished(QPlaceReply*)),
                this, SLOT(processingFinished(QPlaceReply*)));
    }
    return reply;
}

QPlaceSearchReply *QPlaceManagerEngineImpl::searchForPlaces(const QPlaceSearchQuery &query)
{
    QPlaceSearchReplyImpl *reply = NULL;
    QPlaceSearchQuery newQuery = query;
    // search by category
    if (newQuery.categories().count()) {
        newQuery.setSearchTerm(query.categories().at(0).categoryId());
    }
    QPlaceRestReply *restReply = QPlaceRestManager::instance()->sendSearchRequest(newQuery);

    if (restReply) {
        reply = new QPlaceSearchReplyImpl(restReply, this);
        connect(reply, SIGNAL(processingError(QPlaceReply*,QPlaceReply::Error,QString)),
                this, SLOT(processingError(QPlaceReply*,QPlaceReply::Error,QString)));
        connect(reply, SIGNAL(processingFinished(QPlaceReply*)),
                this, SLOT(processingFinished(QPlaceReply*)));
    }
    return reply;
}

QPlaceSearchReply *QPlaceManagerEngineImpl::recommendations(const QPlace &place, const QPlaceSearchQuery &query)
{
    QPlaceRecommendationReplyImpl *reply = NULL;
    QPlaceSearchQuery newQuery = query;
    newQuery.setSearchTerm(place.placeId());
    QPlaceRestReply *restReply = QPlaceRestManager::instance()->sendRecommendationRequest(newQuery, QString());
    if (restReply) {
        reply = new QPlaceRecommendationReplyImpl(restReply, this);
        connect(reply, SIGNAL(processingError(QPlaceReply*,QPlaceReply::Error,QString)),
                this, SLOT(processingError(QPlaceReply*,QPlaceReply::Error,QString)));
        connect(reply, SIGNAL(processingFinished(QPlaceReply*)),
                this, SLOT(processingFinished(QPlaceReply*)));
    }
    return reply;
}

QPlaceTextPredictionReply *QPlaceManagerEngineImpl::textPredictions(const QPlaceSearchQuery &query)
{
    QPlaceTextPreditionReplyImpl *reply = NULL;
    QPlaceRestReply *restReply = QPlaceRestManager::instance()->sendSuggestionRequest(query);
    if (restReply) {
        reply = new QPlaceTextPreditionReplyImpl(restReply, this);
        connect(reply, SIGNAL(processingError(QPlaceReply*,QPlaceReply::Error,QString)),
                this, SLOT(processingError(QPlaceReply*,QPlaceReply::Error,QString)));
        connect(reply, SIGNAL(processingFinished(QPlaceReply*)),
                this, SLOT(processingFinished(QPlaceReply*)));
    }
    return reply;
}

QPlaceManager::ConnectivityMode QPlaceManagerEngineImpl::connectivityMode() const
{
    return QPlaceManager::OnlineMode;
}

void QPlaceManagerEngineImpl::setConnectivityMode(QPlaceManager::ConnectivityMode connectivityMode)
{
    Q_UNUSED(connectivityMode)
    // only QPlaceManager::OnlineMode is suppoerted
}

QList<QPlaceManager::ConnectivityMode> QPlaceManagerEngineImpl::supportedConnectivityModes() const
{
    QList<QPlaceManager::ConnectivityMode> modes;
    modes.append(QPlaceManager::OnlineMode);
    return modes;
}

QPlaceManager::SearchVisibilityScope QPlaceManagerEngineImpl::searchVisibilityScope() const
{
    return QPlaceManager::PublicSearch;
}

void QPlaceManagerEngineImpl::setSearchVisbilityScope(QPlaceManager::SearchVisibilityScope scope)
{
    Q_UNUSED(scope)
    // only QPlaceManager::PublicSearch is supported
}

QList<QPlaceManager::SearchVisibilityScope> QPlaceManagerEngineImpl::supportedSearchVisibilityScopes() const
{
    QList<QPlaceManager::SearchVisibilityScope> modes;
    modes.append(QPlaceManager::PublicSearch);
    return modes;
}

QPlaceReply *QPlaceManagerEngineImpl::initializeCategories(const QString &categorySystemId)
{
    return QPlaceCategoriesRepository::instance()->initializeCategories(categorySystemId);
}

QList<QPlaceCategory> QPlaceManagerEngineImpl::categories() const
{
    return QPlaceCategoriesRepository::instance()->categories();
}

void QPlaceManagerEngineImpl::processingError(QPlaceReply *reply,
                                              const QPlaceReply::Error &errorId,
                                              const QString &errorMessage)
{
    emit error(reply, errorId, errorMessage);
}

void QPlaceManagerEngineImpl::processingFinished(QPlaceReply *reply)
{
    emit finished(reply);
}
