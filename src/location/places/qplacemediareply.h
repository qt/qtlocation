#ifndef QPLACEMEDIAREPLY_H
#define QPLACEMEDIAREPLY_H

#include "qplacereply.h"
#include "qplacemediaobject.h"

#include <QStringList>

QTM_BEGIN_NAMESPACE

class QPlaceMediaReplyPrivate;
class Q_LOCATION_EXPORT QPlaceMediaReply : public QPlaceReply
{
    Q_OBJECT
public:
    QPlaceMediaReply(QObject *parent =0);
    virtual ~QPlaceMediaReply();

    QPlaceReply::Type type() const;

    QList<QPlaceMediaObject> mediaObjects() const;

    int totalCount() const;

protected:
    void setMediaObjects(const QList<QPlaceMediaObject> &objects);
    void setTotalCount(int total);

private:
    Q_DISABLE_COPY(QPlaceMediaReply)
    Q_DECLARE_PRIVATE(QPlaceMediaReply)
};

QTM_END_NAMESPACE

#endif
