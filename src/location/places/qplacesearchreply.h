#ifndef QPLACESEARCHREPLY_H
#define QPLACESEARCHREPLY_H

#include "qplacereply.h"
#include "qplacesearchresult.h"

namespace QT_PLACES_NAMESPACE {

class QPlaceSearchReplyPrivate;
class Q_PLACES_EXPORT QPlaceSearchReply : public QPlaceReply
{
    Q_OBJECT
public:
    QPlaceSearchReply(QObject *parent =0);
    virtual ~QPlaceSearchReply();

    QPlaceReply::Type type() const;

    QList<QPlaceSearchResult> results() const;

protected:
    void setResults(const QList<QPlaceSearchResult> &results);
private:
    QPlaceSearchReplyPrivate *d;
};

} // QT_PLACES_NAMESPACE

#endif
