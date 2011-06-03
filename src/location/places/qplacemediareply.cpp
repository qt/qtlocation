#include "qplacemediareply.h"

#include "qplace.h"

QTM_BEGIN_NAMESPACE
class QPlaceMediaReplyPrivate
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
    : QPlaceReply(parent)
{
    d = new QPlaceMediaReplyPrivate;
}

/*!
    Destroys the reply.
*/
QPlaceMediaReply::~QPlaceMediaReply()
{
    delete d;
}

 /*!
    Returns the media objects.
*/
QList<QPlaceMediaObject> QPlaceMediaReply::mediaObjects() const
{
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
    d->mediaObjects = objects;
}

/*!
    Returns the total number of media objects for a place.  If the total number of
    media objects cannot be counted a value of -1 is returned.
*/
int QPlaceMediaReply::totalCount()
{
    return d->totalCount;
}

/*!
    Sets the \a total number of media objects for a place.
*/
void QPlaceMediaReply::setTotalCount(int total)
{
    d->totalCount = total;
}
