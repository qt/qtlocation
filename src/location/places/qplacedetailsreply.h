#ifndef QPLACEDETAILSREPLY_H
#define QPLACEDETAILSREPLY_H

#include "qplacereply.h"
#include "qplace.h"

QTM_BEGIN_NAMESPACE

class QPlaceDetailsReplyPrivate;
class Q_LOCATION_EXPORT QPlaceDetailsReply : public QPlaceReply
{
    Q_OBJECT
public:
    QPlaceDetailsReply(QObject *parent =0);
    virtual ~QPlaceDetailsReply();

    QPlaceReply::Type type() const;

    QPlace result() const;

protected:
    void setResult(const QPlace &result);
private:
    Q_DISABLE_COPY(QPlaceDetailsReply)
    Q_DECLARE_PRIVATE(QPlaceDetailsReply)
};

QTM_END_NAMESPACE

#endif // QPLACEDETAILSREPLY_H
