#include "qplacesearchreply.h"

QTM_BEGIN_NAMESPACE
class QPlaceSearchReplyPrivate
{
public:
    QPlaceSearchReplyPrivate();
    QList<QPlaceSearchResult> results;
};

QTM_END_NAMESPACE

QTM_USE_NAMESPACE

/*!
    \class QPlaceSearchReply

    \brief The QPlaceSearchReply class manages a search operation started by an
    instance of QPlaceManager.

    \inmodule QtLocation

    \ingroup places-main
*/

/*!
    Constructs a search reply with a given \a parent.
*/
QPlaceSearchReply::QPlaceSearchReply(QObject *parent)
    : QPlaceReply(parent)
{
}

/*!
    Destroys the search reply.
*/
QPlaceSearchReply::~QPlaceSearchReply()
{
    delete d;
}

/*!
    Returns the type of reply.
*/
QPlaceReply::Type QPlaceSearchReply::type() const
{
    return QPlaceReply::PlaceSearchReply;
}

 /*!
    Returns a list of search results;
*/
QList<QPlaceSearchResult> QPlaceSearchReply::results() const
{
    return d->results;
}

/*!
    Sets the list of search \a results.
*/
void QPlaceSearchReply::setResults(const QList<QPlaceSearchResult> &results)
{
    d->results = results;
}
