#ifndef QPLACESEARCHREPLY_H
#define QPLACESEARCHREPLY_H

#include "qplacereply.h"
#include "qplacesearchresult.h"

QTM_BEGIN_NAMESPACE

class QPlaceSearchReplyPrivate;
class Q_LOCATION_EXPORT QPlaceSearchReply : public QPlaceReply
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

QTM_END_NAMESPACE

#endif
