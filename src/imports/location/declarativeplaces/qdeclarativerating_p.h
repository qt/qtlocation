#ifndef QDECLARATIVERATING_P_H
#define QDECLARATIVERATING_P_H

#include <qplacerating.h>
#include <QtDeclarative/qdeclarative.h>

#include <QObject>

QTM_BEGIN_NAMESPACE

class QDeclarativeRating : public QObject
{
    Q_OBJECT

    Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)

public:
    explicit QDeclarativeRating(QObject* parent = 0);
    explicit QDeclarativeRating(const QPlaceRating &src, QObject* parent = 0);
    ~QDeclarativeRating();

    QPlaceRating rating() const;
    void setRating(const QPlaceRating &src);

    double value() const;
    void setValue(const double &data);
    int count() const;
    void setCount(const int &data);

signals:
    void valueChanged();
    void countChanged();

private:
    QPlaceRating m_rating;
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeRating));

#endif // QDECLARATIVERATING_P_H
