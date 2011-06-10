#include "qplacedetailsreply.h"
#include "qplacereply_p.h"

QTM_BEGIN_NAMESPACE
class QPlaceDetailsReplyPrivate : public QPlaceReplyPrivate
{
public:
    QPlaceDetailsReplyPrivate() {}
    ~QPlaceDetailsReplyPrivate() {}
    QPlace result;
};

QTM_END_NAMESPACE

QTM_USE_NAMESPACE

/*!
    \class QPlaceDetailsReply

    \brief The QPlaceDetailsReply class manages a place datails operation started by an
    instance of QPlaceManager.

    \inmodule QtPlaces

    \ingroup places-main
*/

/*!
    Constructs a search reply with a given \a parent.
*/
QPlaceDetailsReply::QPlaceDetailsReply(QObject *parent)
    : QPlaceReply(new QPlaceDetailsReplyPrivate, parent)
{
}

/*!
    Destroys the search reply.
*/
QPlaceDetailsReply::~QPlaceDetailsReply()
{
}

/*!
    Returns the type of reply.
*/
QPlaceReply::Type QPlaceDetailsReply::type() const
{
    return QPlaceReply::PlaceDetailsReply;
}

 /*!
    Returns a place result;
*/
QPlace QPlaceDetailsReply::result() const
{
    Q_D(const QPlaceDetailsReply);
    return d->result;
}

/*!
    Sets place \a result.
*/
void QPlaceDetailsReply::setResult(const QPlace &result)
{
    Q_D(QPlaceDetailsReply);
    d->result = result;
}
