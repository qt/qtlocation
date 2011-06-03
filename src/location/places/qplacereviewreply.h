#ifndef QPLACEREVIEWREPLY_H
#define QPLACEREVIEWREPLY_H

#include "qplacereply.h"
#include "qplacereview.h"

#include <QStringList>

QTM_BEGIN_NAMESPACE

class QPlaceReviewReplyPrivate;
class Q_LOCATION_EXPORT QPlaceReviewReply : public QPlaceReply
{
    Q_OBJECT
public:
    QPlaceReviewReply(QObject *parent = 0);
    virtual ~QPlaceReviewReply();

    QPlaceReply::Type type() const;

    QList<QPlaceReview> reviews() const;

    int totalCount();

protected:
    void setReviews(const QList<QPlaceReview> &objects);
    void setTotalCount(int total);

private:
    QPlaceReviewReplyPrivate *d;
};

QTM_BEGIN_NAMESPACE

#endif
