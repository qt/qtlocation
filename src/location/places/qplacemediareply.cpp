#include "qplacemediareply.h"
#include "qplacereply_p.h"

#include "qplace.h"

QTM_BEGIN_NAMESPACE
class QPlaceMediaReplyPrivate : public QPlaceReplyPrivate
{
public:
    QPlaceMediaReplyPrivate() {}
    QList<QPlaceMediaObject> mediaObjects;
    int totalCount;
};

QTM_END_NAMESPACE

QTM_USE_NAMESPACE

/*!
    \class QPlaceMediaReply

    \brief The QPlaceMediaReply class manages a media retrieval operation started by an
    instance of QPlaceManager.

    \inmodule QtLocation

    \ingroup places-main
*/

/*!
    Constructs a media reply with a given \a parent.
*/
QPlaceMediaReply::QPlaceMediaReply(QObject *parent)
    : QPlaceReply(new QPlaceMediaReplyPrivate, parent)
{
}

/*!
    Destroys the reply.
*/
QPlaceMediaReply::~QPlaceMediaReply()
{
}

 /*!
    Returns the media objects.
*/
QList<QPlaceMediaObject> QPlaceMediaReply::mediaObjects() const
{
    Q_D(const QPlaceMediaReply);
    return d->mediaObjects;
}

/*!
    Returns the type of reply.
*/
QPlaceReply::Type QPlaceMediaReply::type() const
{
    return QPlaceReply::MediaReply;
}

/*!
    Sets the media \a objects.
*/
void QPlaceMediaReply::setMediaObjects(const QList<QPlaceMediaObject> &objects)
{
    Q_D(QPlaceMediaReply);
    d->mediaObjects = objects;
}

/*!
    Returns the total number of media objects for a place.  If the total number of
    media objects cannot be counted a value of -1 is returned.
*/
int QPlaceMediaReply::totalCount() const
{
    Q_D(const QPlaceMediaReply);
    return d->totalCount;
}

/*!
    Sets the \a total number of media objects for a place.
*/
void QPlaceMediaReply::setTotalCount(int total)
{
    Q_D(QPlaceMediaReply);
    d->totalCount = total;
}
