#ifndef QDECLARATIVEREVIEWPAGINATIONLIST_P_H
#define QDECLARATIVEREVIEWPAGINATIONLIST_P_H

#include <QObject>
#include <QDeclarativeListProperty>
#include <qplacepaginationlist.h>
#include "qdeclarativereview_p.h"

QT_BEGIN_NAMESPACE

class QDeclarativeReviewPaginationList : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int items READ items NOTIFY itemsChanged);
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeReview> data READ data NOTIFY dataChanged)
    Q_PROPERTY(int start READ start WRITE setStart NOTIFY startChanged);
    Q_PROPERTY(int stop READ stop NOTIFY stopChanged);

public:
    explicit QDeclarativeReviewPaginationList(QObject* parent = 0);
    explicit QDeclarativeReviewPaginationList(const QPlacePaginationList<QPlaceReview> &src, QObject* parent = 0);
    ~QDeclarativeReviewPaginationList();

    QPlacePaginationList<QPlaceReview> paginationList();
    void setPaginationList(const QPlacePaginationList<QPlaceReview> &src);

    QDeclarativeListProperty<QDeclarativeReview> data();
    static void data_append(QDeclarativeListProperty<QDeclarativeReview> *prop,
                                  QDeclarativeReview* value);
    static int data_count(QDeclarativeListProperty<QDeclarativeReview> *prop);
    static QDeclarativeReview* data_at(QDeclarativeListProperty<QDeclarativeReview> *prop, int index);
    static void data_clear(QDeclarativeListProperty<QDeclarativeReview> *prop);

    int items();
    int start();
    void setStart(const int &data);
    int stop();

signals:
    void itemsChanged();
    void dataChanged();
    void startChanged();
    void stopChanged();

private:
    void synchronizeReview();

private:
    QList<QDeclarativeReview*> m_objects;
    QPlacePaginationList<QPlaceReview> m_src;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeReviewPaginationList));

#endif // QDECLARATIVEREVIEWPAGINATIONLIST_P_H
