#ifndef QDECLARATIVEMEDIAPAGINATIONLIST_P_H
#define QDECLARATIVEMEDIAPAGINATIONLIST_P_H

#include <QObject>
#include <QDeclarativeListProperty>
#include <qplacepaginationlist.h>
#include "qdeclarativemediaobject_p.h"

QTM_BEGIN_NAMESPACE

class QDeclarativeMediaPaginationList : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int items READ items NOTIFY itemsChanged);
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeMediaObject> data READ data NOTIFY dataChanged)
    Q_PROPERTY(int start READ start WRITE setStart NOTIFY startChanged);
    Q_PROPERTY(int stop READ stop NOTIFY stopChanged);

public:
    explicit QDeclarativeMediaPaginationList(QObject* parent = 0);
    explicit QDeclarativeMediaPaginationList(const QPlacePaginationList<QPlaceMediaObject> &src, QObject* parent = 0);
    ~QDeclarativeMediaPaginationList();

    QPlacePaginationList<QPlaceMediaObject> paginationList();
    void setPaginationList(const QPlacePaginationList<QPlaceMediaObject> &src);

    QDeclarativeListProperty<QDeclarativeMediaObject> data();
    static void data_append(QDeclarativeListProperty<QDeclarativeMediaObject> *prop,
                                  QDeclarativeMediaObject* value);
    static int data_count(QDeclarativeListProperty<QDeclarativeMediaObject> *prop);
    static QDeclarativeMediaObject* data_at(QDeclarativeListProperty<QDeclarativeMediaObject> *prop, int index);
    static void data_clear(QDeclarativeListProperty<QDeclarativeMediaObject> *prop);

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
    void synchronizeMediaObjects();

private:
    QList<QDeclarativeMediaObject*> m_objects;
    QPlacePaginationList<QPlaceMediaObject> m_src;
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeMediaPaginationList));

#endif // QDECLARATIVEMEDIAPAGINATIONLIST_P_H
