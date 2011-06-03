#include "qplacetextpredictionreply.h"

QTM_BEGIN_NAMESPACE

class QPlaceTextPredictionReplyPrivate
{
public:
    QPlaceTextPredictionReplyPrivate(){};
    QStringList textPredictions;
};

QTM_END_NAMESPACE

QTM_USE_NAMESPACE

/*!
    \class QPlaceTextPredictionReply

    \brief The QPlaceTextPredictionReply class manages a text prediction operation started by an
    instance of QPlaceManager.

    \inmodule QtLocation

    \ingroup places-main
*/

/*!
    Constructs a text prediction reply with a given \a parent.
*/
QPlaceTextPredictionReply::QPlaceTextPredictionReply(QObject *parent)
    : QPlaceReply(parent),
      d(new QPlaceTextPredictionReplyPrivate)
{
}

/*!
    Destroys the reply.
*/
QPlaceTextPredictionReply::~QPlaceTextPredictionReply()
{
    delete d;
}

/*!
    Returns the text predictions.
*/
QStringList QPlaceTextPredictionReply::textPredictions() const
{
    return d->textPredictions;
}

/*!
   Returns type of reply.
*/
QPlaceReply::Type QPlaceTextPredictionReply::type() const
{
    return QPlaceReply::TextPredictionReply;
}

/*!
    Sets the text \a predictions.
*/
void QPlaceTextPredictionReply::setTextPredictions(const QStringList &predictions)
{
    d->textPredictions = predictions;
}
