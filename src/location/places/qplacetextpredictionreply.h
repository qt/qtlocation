#ifndef QPLACETEXTPREDICTIONREPLY_H
#define QPLACETEXTPREDICTIONREPLY_H

#include "qplacereply.h"

#include <QStringList>

QTM_BEGIN_NAMESPACE

class QPlaceTextPredictionReplyPrivate;

class Q_LOCATION_EXPORT QPlaceTextPredictionReply : public QPlaceReply
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
    Q_DISABLE_COPY(QPlaceTextPredictionReply)
    Q_DECLARE_PRIVATE(QPlaceTextPredictionReply)
};

QTM_END_NAMESPACE

#endif
