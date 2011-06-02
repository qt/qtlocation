#ifndef QPLACEDETAILSREPLY_H
#define QPLACEDETAILSREPLY_H

#include "qplacereply.h"
#include "qplace.h"

namespace QT_PLACES_NAMESPACE {

class QPlaceDetailsReplyPrivate;
class Q_PLACES_EXPORT QPlaceDetailsReply : public QPlaceReply
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
    QPlaceDetailsReplyPrivate *d;
};

} // QT_PLACES_NAMESPACE

#endif // QPLACEDETAILSREPLY_H
