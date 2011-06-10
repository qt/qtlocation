#ifndef QPLACEREVIEWREPLY_H
#define QPLACEREVIEWREPLY_H

#include "qplacereply.h"
#include "qplacereview.h"
#include "qplacepaginationlist.h"

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

    QPlacePaginationList<QPlaceReview> reviews() const;

    int totalCount() const;

protected:
    void setOffset(int offset);
    void setReviews(const QList<QPlaceReview> &objects);
    void setTotalCount(int total);
private:
    Q_DISABLE_COPY(QPlaceReviewReply)
    Q_DECLARE_PRIVATE(QPlaceReviewReply)
};

QTM_BEGIN_NAMESPACE

#endif
