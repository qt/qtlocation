#include "qplacetextpredictionreply.h"
#include "qplacereply_p.h"

QTM_BEGIN_NAMESPACE

class QPlaceTextPredictionReplyPrivate : public QPlaceReplyPrivate
{
public:
    QPlaceTextPredictionReplyPrivate(){}
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
    : QPlaceReply(new QPlaceTextPredictionReplyPrivate, parent)
{
}

/*!
    Destroys the reply.
*/
QPlaceTextPredictionReply::~QPlaceTextPredictionReply()
{
}

/*!
    Returns the text predictions.
*/
QStringList QPlaceTextPredictionReply::textPredictions() const
{
    Q_D(const QPlaceTextPredictionReply);
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
    Q_D(QPlaceTextPredictionReply);
    d->textPredictions = predictions;
}
