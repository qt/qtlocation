#ifndef QPLACETEXTPREDICTIONREPLY_H
#define QPLACETEXTPREDICTIONREPLY_H

#include "qplacereply.h"

#include <QStringList>

namespace QT_PLACES_NAMESPACE {

class QPlaceTextPredictionReplyPrivate;

class Q_PLACES_EXPORT QPlaceTextPredictionReply : public QPlaceReply
{
    Q_OBJECT
public:
    QPlaceTextPredictionReply(QObject *parent =0);
    virtual ~QPlaceTextPredictionReply();

    QStringList textPredictions() const;
    Type type() const;

protected:
    void setTextPredictions(const QStringList &predictions);

private:
    QPlaceTextPredictionReplyPrivate *d;
};

} // QT_PLACES_NAMESPACE

#endif
