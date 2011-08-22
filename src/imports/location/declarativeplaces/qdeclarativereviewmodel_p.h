#ifndef QDECLARATIVEREVIEWMODEL_P_H
#define QDECLARATIVEREVIEWMODEL_P_H

#include <QtCore/QObject>
#include <QtCore/QAbstractListModel>
#include "qdeclarativereview_p.h"
#include <qplacemanager.h>

QT_BEGIN_NAMESPACE

class QDeclarativePlace;
class QDeclarativeGeoServiceProvider;
class QGeoServiceProvider;

class QDeclarativeReviewModel : public QAbstractListModel, public QDeclarativeParserStatus
{
    Q_OBJECT

    Q_INTERFACES(QDeclarativeParserStatus)

    Q_PROPERTY(QDeclarativePlace *place READ place WRITE setPlace NOTIFY placeChanged)
    Q_PROPERTY(int batchSize READ batchSize WRITE setBatchSize NOTIFY batchSizeChanged)
    Q_PROPERTY(int totalCount READ totalCount NOTIFY totalCountChanged)

public:
    explicit QDeclarativeReviewModel(QObject* parent = 0);
    ~QDeclarativeReviewModel();

    QDeclarativePlace *place() const;
    void setPlace(QDeclarativePlace *place);

    int batchSize() const;
    void setBatchSize(int batchSize);

    int totalCount() const;

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    enum Roles {
        ReviewRole = Qt::UserRole
    };

    bool canFetchMore(const QModelIndex &parent) const;
    void fetchMore(const QModelIndex &parent);

    void clear();

    void classBegin();
    void componentComplete();

signals:
    void placeChanged();
    void batchSizeChanged();
    void totalCountChanged();

private slots:
    void fetchFinished();

private:
    QDeclarativePlace *m_place;
    int m_batchSize;
    int m_reviewCount;
    QMap<int, QDeclarativeReview *> m_reviews;
    QPlaceContentReply *m_reply;

    bool m_complete;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeReviewModel));

#endif // QDECLARATIVEREVIEWMODEL_P_H
