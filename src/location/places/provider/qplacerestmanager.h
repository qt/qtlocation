#ifndef QPLACERESTMANAGER_H
#define QPLACERESTMANAGER_H

#include <QObject>

#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include "../qmobilityglobal.h"
#include "../qplacesearchquery.h"
#include "qplacerestreply.h"

QTM_BEGIN_NAMESPACE

class QPlaceRestManager : public QObject
{
    Q_OBJECT

public:
    static QPlaceRestManager *instance();

    QPlaceRestReply *sendPlaceRequest(const QString &placeId);
    QPlaceRestReply *sendPlaceImagesRequest(const QString &placeId, const QPlaceQuery &params);
    QPlaceRestReply *sendPlaceReviewRequest(const QString &placeId, const QPlaceQuery &params);
    QPlaceRestReply *sendRecommendationRequest(const QPlaceSearchQuery &query, const QString &userId);
    QPlaceRestReply *sendCategoriesTreeRequest();
    QPlaceRestReply *sendSearchRequest(const QPlaceSearchQuery &query);
    QPlaceRestReply *sendSearchByCategoryRequest(const QPlaceSearchQuery &query);
    QPlaceRestReply *sendSuggestionRequest(const QPlaceSearchQuery &query);

    QPlaceRestReply *postRatingRequest(const QString &placeId, const QString &userId, const int &value);

private:
    explicit QPlaceRestManager(QObject *parent = 0);
    QPlaceRestReply *sendGeneralRequest(const QUrl &url);
    //TODO: remove when engine is refactored out
    QPlaceRestReply *sendGeneralRequest(const QString &url) {
        return sendGeneralRequest(QUrl(url));
    }

    QString prepareSearchRequest(const QPlaceSearchQuery &query);

private:
    QNetworkAccessManager *mManager;
    static QPlaceRestManager *mInstance;
};

QTM_END_NAMESPACE

#endif // QPLACERESTMANAGER_H
