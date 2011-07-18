#ifndef QDECLARATIVEREVIEW_P_H
#define QDECLARATIVEREVIEW_P_H

#include <QObject>
#include <QDeclarativeListProperty>
#include <qplacereview.h>
#include "qdeclarativesupplier_p.h"

QT_BEGIN_NAMESPACE

class QDeclarativeReview : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString date READ date WRITE setDate NOTIFY dateChanged);
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged);
    Q_PROPERTY(QDeclarativeSupplier* supplier READ supplier WRITE setSupplier NOTIFY supplierChanged);
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged);
    Q_PROPERTY(QString reviewId READ reviewId WRITE setReviewId NOTIFY reviewIdChanged);
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged);
    Q_PROPERTY(QString userId READ userId WRITE setUserId NOTIFY userIdChanged);
    Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged);
    Q_PROPERTY(QString originatorUrl READ originatorUrl WRITE setOriginatorUrl NOTIFY originatorUrlChanged);
    Q_PROPERTY(int helpfulVotings READ helpfulVotings WRITE setHelpfulVotings NOTIFY helpfulVotingsChanged);
    Q_PROPERTY(int unhelpfulVotings READ unhelpfulVotings WRITE setUnhelpfulVotings NOTIFY unhelpfulVotingsChanged);
    Q_PROPERTY(qreal rating READ rating WRITE setRating NOTIFY ratingChanged);
    Q_PROPERTY(QStringList mediaIds READ mediaIds WRITE setMediaIds NOTIFY mediaIdsChanged);

public:
    explicit QDeclarativeReview(QObject* parent = 0);
    explicit QDeclarativeReview(const QPlaceReview &src, QObject* parent = 0);
    ~QDeclarativeReview();

    QPlaceReview review();
    void setReview(const QPlaceReview &src);

    QString date() const;
    void setDate(const QString &data);
    QString description() const;
    void setDescription(const QString &data);
    QString language() const;
    void setLanguage(const QString &data);
    int helpfulVotings() const;
    void setHelpfulVotings(const int &data);
    QStringList mediaIds() const;
    void setMediaIds(const QStringList &data);
    int unhelpfulVotings() const;
    void setUnhelpfulVotings(const int &data);
    qreal rating() const;
    void setRating(const qreal &data);
    QString reviewId() const;
    void setReviewId(const QString &data);
    QString title() const;
    void setTitle(const QString &data);
    QString userId() const;
    void setUserId(const QString &data);
    QString userName() const;
    void setUserName(const QString &data);
    QString originatorUrl() const;
    void setOriginatorUrl(const QString &data);
    QDeclarativeSupplier *supplier();
    void setSupplier(QDeclarativeSupplier *data);

signals:
    void dateChanged();
    void descriptionChanged();
    void reviewIdChanged();
    void supplierChanged();
    void languageChanged();
    void userIdChanged();
    void userNameChanged();
    void titleChanged();
    void originatorUrlChanged();
    void helpfulVotingsChanged();
    void unhelpfulVotingsChanged();
    void ratingChanged();
    void mediaIdsChanged();

private:
    QDeclarativeSupplier m_declarativeSupplier;
    QPlaceReview m_src;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeReview));

#endif // QDECLARATIVEREVIEW_P_H
