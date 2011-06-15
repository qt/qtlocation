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
        OfflineMode = 0x0001,
        OnlineMode = 0x0002,
        HybridMode = 0x0003
    };

    Q_DECLARE_FLAGS(ConnectivityModes, ConnectivityMode)

    enum VisibilityScope {
        NoScope = 0x0000,
        PublicScope = 0x0001,
        PrivateScope = 0x0002,
        PublicAndPrivateScope = 0x0007
    };

    Q_DECLARE_FLAGS(VisibilityScopes, VisibilityScope)

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

    enum Error {
        NoError,
        NotSupportedError
    };

    QPlaceManager(QObject *parent = 0);
    ~QPlaceManager();
    QString managerName() const;

    QPlaceDetailsReply *getPlaceDetails(const QString &placeId) const;
    QPlaceReply *postRating(const QPlace &place, qreal value);

    QPlaceReviewReply *getReviews(const QPlace &place, const QPlaceQuery &query) const;

    QPlaceMediaReply *getMedia(const QPlace &place, const QPlaceQuery &query) const;

    QPlaceSearchReply *searchForPlaces(const QPlaceSearchQuery &query, VisibilityScope scope) const;
    VisibilityScopes supportedSearchVisibilityScopes() const;

    QPlaceSearchReply *recommendations(const QPlace &place, const QPlaceSearchQuery &query) const;

    QPlaceTextPredictionReply *textPredictions(const QPlaceSearchQuery &query) const;

    ConnectivityModes connectivityMode() const;
    void setConnectivityMode(ConnectivityModes connectivityMode);
    ConnectivityModes supportedConnectivityModes() const;

    QPlaceReply *savePlace(QPlace *place, VisibilityScope scope);
    VisibilityScopes supportedSaveVisibilityScopes();

    QPlaceReply *removePlace(const QPlace &place);

    QPlaceReply *initializeCategories(const QString &categorySystemId = QString());
    QList<QPlaceCategory> categories() const;
    QPlaceCategory createCategory() const;

    static QStringList availableManagers();

Q_SIGNALS:
    void finished(QPlaceReply *reply);
    void error(QPlaceReply *, QPlaceReply::Error error, const QString &errorString = QString());

private:
    Q_DISABLE_COPY(QPlaceManager)
    QPlaceManagerPrivate* d;
    Q_DECLARE_PRIVATE(QPlaceManager)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QPlaceManager::VisibilityScopes);
Q_DECLARE_OPERATORS_FOR_FLAGS(QPlaceManager::ConnectivityModes);

QTM_END_NAMESPACE

#endif // QPLACEMANAGER_H
