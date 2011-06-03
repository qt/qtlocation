#ifndef QPLACEREPLY_H
#define QPLACEREPLY_H

#include "qmobilityglobal.h"
#include <QObject>

QTM_BEGIN_NAMESPACE

class QPlaceReplyPrivate;
class Q_LOCATION_EXPORT QPlaceReply : public QObject
{
    Q_OBJECT
public:
    enum Error {
        NoError,
        DoesNotExistError,
        PlaceDoesNotExistError,
        CategoryDoesNotExistError,
        CommunicationError,
        ParseError,
        PermissionsError,
        UnsupportedError,
        CancelError,
        UnknownError
    };

    enum Type {
        Reply,
        PlaceDetailsReply,
        PlaceSearchReply,
        TextPredictionReply,
        ReviewReply,
        MediaReply
    };

    virtual ~QPlaceReply();

    bool isFinished() const;

    virtual Type type() const;

    QString errorString() const;
    QPlaceReply::Error error() const;

public Q_SLOTS:
    virtual void abort();

Q_SIGNALS:
    void finished();
    void error(QPlaceReply::Error error, const QString &errorString = QString());

protected:
    QPlaceReply(QObject *parent = 0);
    void setFinished(bool finished);
    void setError(QPlaceReply::Error error, const QString &errorString);

private:
    QPlaceReplyPrivate *d;
};

QTM_END_NAMESPACE

#endif // QPLACEREPLY_H
