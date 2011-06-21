#ifndef QDECLARATIVEPLACEMANAGER_P_H
#define QDECLARATIVEPLACEMANAGER_P_H

#include <QObject>

#include <qplacemanager.h>
#include <qplacedetailsreply.h>

#include "qdeclarativeplace_p.h"

QTM_BEGIN_NAMESPACE

class QDeclarativePlaceManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativePlace* place READ place NOTIFY placeChanged);

public:
    explicit QDeclarativePlaceManager(QObject *parent = 0);
    ~QDeclarativePlaceManager();

    QDeclarativePlace *place();

    Q_INVOKABLE void getPlaceDetails(QDeclarativePlace *place);
    Q_INVOKABLE void getPlaceMedia(QDeclarativePlace *place);
    Q_INVOKABLE void getPlaceMedia(QDeclarativePlace *place, int offset, int limit);
    Q_INVOKABLE void getPlaceReviews(QDeclarativePlace *place);
    Q_INVOKABLE void getPlaceReviews(QDeclarativePlace *place, int offset, int limit);

    Q_INVOKABLE void ratePlace(QDeclarativePlace *place, qreal rateValue, const QString &userName);

    Q_INVOKABLE void cancelRequest();

signals:
    void requestFinished(const int &error);
    void placeChanged();

private slots:
    void replyFinished();
    void replyError(QPlaceReply::Error error, const QString &errorString);

private:
    void cancelPreviousRequest();
    void connectNewResponse(QPlaceReply *newResponse);

private:
    QDeclarativePlace *m_place;

    QPlaceManager *m_manager;
    QPlaceReply *m_response;
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativePlaceManager));

#endif // QDECLARATIVETEXTPREDICTIONMODEL_P_H
