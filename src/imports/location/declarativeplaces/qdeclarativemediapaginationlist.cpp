#include "qdeclarativemediapaginationlist_p.h"

QTM_USE_NAMESPACE

/*!
    \qmlclass MediaPaginationList

    \brief The MediaPaginationList element holds media objects list.
    \inherits QObject

    MediaPaginationList cointains list of media objects on curent page.
    As list of items might be vary big user might view it page by page.

    \ingroup qml-places
*/

QDeclarativeMediaPaginationList::QDeclarativeMediaPaginationList(QObject* parent)
        : QObject(parent)
{
}

QDeclarativeMediaPaginationList::QDeclarativeMediaPaginationList(
    const QPlacePaginationList<QPlaceMediaObject> &src,
    QObject *parent)
        : QObject(parent),
          m_src(src)
{
    synchronizeMediaObjects();
}

QDeclarativeMediaPaginationList::~QDeclarativeMediaPaginationList()
{
}

void QDeclarativeMediaPaginationList::setPaginationList(const QPlacePaginationList<QPlaceMediaObject> &src)
{
    QPlacePaginationList<QPlaceMediaObject> previous = m_src;
    m_src = src;

    if (previous.items() != m_src.items()) {
        emit itemsChanged();
    }
    if (previous.data() != m_src.data()) {
        synchronizeMediaObjects();
        emit dataChanged();
    }
    if (previous.start() != m_src.start()) {
        emit startChanged();
    }
    if (previous.stop() != m_src.stop()) {
        emit stopChanged();
    }
}

QPlacePaginationList<QPlaceMediaObject> QDeclarativeMediaPaginationList::paginationList()
{
    return m_src;
}

/*!
    \qmlproperty int MediaPaginationList::items

    This property holds number of items on current page.
*/
int QDeclarativeMediaPaginationList::items()
{
    return m_src.items();
}

/*!
    \qmlproperty int MediaPaginationList::start

    This property holds number of first item from current page
    on all items list.
*/

void QDeclarativeMediaPaginationList::setStart(const int &data)
{
    if (m_src.start() != data) {
        m_src.setStart(data);
        emit startChanged();
    }
}

int QDeclarativeMediaPaginationList::start()
{
    return m_src.start();
}

/*!
    \qmlproperty int MediaPaginationList::stop

    This property holds number of last item from current page
    on all items list.
*/
int QDeclarativeMediaPaginationList::stop()
{
    return m_src.stop();
}

/*!
    \qmlproperty QDeclarativeListProperty<QDeclarativeMediaObject> MediaPaginationList::data

    This property contains of list of items.
*/
QDeclarativeListProperty<QDeclarativeMediaObject> QDeclarativeMediaPaginationList::data()
{
    return QDeclarativeListProperty<QDeclarativeMediaObject>(this,
                                                          0, // opaque data parameter
                                                          data_append,
                                                          data_count,
                                                          data_at,
                                                          data_clear);
}

void QDeclarativeMediaPaginationList::data_append(QDeclarativeListProperty<QDeclarativeMediaObject> *prop,
                                                  QDeclarativeMediaObject *value)
{
    QDeclarativeMediaPaginationList* object = static_cast<QDeclarativeMediaPaginationList*>(prop->object);
    QDeclarativeMediaObject *altValue = new QDeclarativeMediaObject(object);
    altValue->setMediaObject(value->mediaObject());
    object->m_objects.append(altValue);
    QList<QPlaceMediaObject> list = object->m_src.data();
    list.append(altValue->mediaObject());
    object->m_src.setData(list);
    emit object->dataChanged();
    emit object->stopChanged();
}

int QDeclarativeMediaPaginationList::data_count(QDeclarativeListProperty<QDeclarativeMediaObject> *prop)
{
    return static_cast<QDeclarativeMediaPaginationList*>(prop->object)->m_objects.count();
}

QDeclarativeMediaObject* QDeclarativeMediaPaginationList::data_at(QDeclarativeListProperty<QDeclarativeMediaObject> *prop,
                                                                          int index)
{
    QDeclarativeMediaPaginationList* object = static_cast<QDeclarativeMediaPaginationList*>(prop->object);
    QDeclarativeMediaObject *res = NULL;
    if (object->m_objects.count() > index && index > -1) {
        res = object->m_objects[index];
    }
    return res;
}

void QDeclarativeMediaPaginationList::data_clear(QDeclarativeListProperty<QDeclarativeMediaObject> *prop)
{
    QDeclarativeMediaPaginationList* object = static_cast<QDeclarativeMediaPaginationList*>(prop->object);
    qDeleteAll(object->m_objects);
    object->m_objects.clear();
    object->m_src.setData(QList<QPlaceMediaObject>());
    emit object->dataChanged();
    emit object->stopChanged();
}

void QDeclarativeMediaPaginationList::synchronizeMediaObjects()
{
    qDeleteAll(m_objects);
    m_objects.clear();
    foreach (QPlaceMediaObject value, m_src.data()) {
        QDeclarativeMediaObject* declarativeValue = new QDeclarativeMediaObject(value, this);
        m_objects.append(declarativeValue);
    }
}
