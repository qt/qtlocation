#ifndef QPLACEREVIEW_H
#define QPLACEREVIEW_H

#include <QSharedDataPointer>
#include <QStringList>
#include "qmobilityglobal.h"
#include "qplacesupplier.h"

QTM_BEGIN_NAMESPACE

class QPlaceReviewPrivate;

class Q_LOCATION_EXPORT QPlaceReview
{
public:
    QPlaceReview();
    QPlaceReview(const QPlaceReview &other);

    virtual ~QPlaceReview();

    QPlaceReview &operator=(const QPlaceReview &other);

    bool operator==(const QPlaceReview &other) const;
    bool operator!=(const QPlaceReview &other) const {
        return !(other == *this);
    }

    QString date() const;
    void setDate(const QString &data);
    QString description() const;
    void setDescription(const QString &data);
    QString language() const;
    void setLanguage(const QString &data);
    uint helpfulVotings() const;
    void setHelpfulVotings(const uint &data);
    QStringList mediaIds() const;
    void setMediaIds(const QStringList &data);
    uint notHelpfulVotings() const;
    void setNotHelpfulVotings(const uint &data);
    double rating() const;
    void setRating(const double &data);
    QString reviewId() const;
    void setReviewId(const QString &data);
    QPlaceSupplier supplier() const;
    void setSupplier(const QPlaceSupplier &data);
    QString title() const;
    void setTitle(const QString &data);
    QString userId() const;
    void setUserId(const QString &data);
    QString userName() const;
    void setUserName(const QString &data);
    QString originatorURL() const;
    void setOriginatorURL(const QString &data);

private:
    QSharedDataPointer<QPlaceReviewPrivate> d;
};

QTM_END_NAMESPACE

#endif // QPLACEREVIEW_H
