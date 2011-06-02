#ifndef QPLACEMEDIAREPLY_H
#define QPLACEMEDIAREPLY_H

#include "qplacereply.h"
#include "qplacemediaobject.h"

#include <QStringList>

namespace QT_PLACES_NAMESPACE {

class QPlaceMediaReplyPrivate;
class Q_PLACES_EXPORT QPlaceMediaReply : public QPlaceReply
{
    Q_OBJECT
public:
    QPlaceMediaReply(QObject *parent =0);
    virtual ~QPlaceMediaReply();

    QPlaceReply::Type type() const;

    QList<QPlaceMediaObject> mediaObjects() const;

    int totalCount();

protected:
    void setMediaObjects(const QList<QPlaceMediaObject> &objects);
    void setTotalCount(int total);

private:
    QPlaceMediaReplyPrivate *d;
};

} // QT_PLACES_NAMESPACE

#endif
