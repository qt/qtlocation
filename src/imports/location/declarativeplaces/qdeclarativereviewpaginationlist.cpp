#include "qdeclarativereviewpaginationlist_p.h"

QT_USE_NAMESPACE

/*!
    \qmlclass ReviewPaginationList

    \brief The ReviewPaginationList element holds review objects list.
    \inherits QObject

    ReviewPaginationList cointains list of review objects on curent page.
    As list of items might be vary big user might view it page by page.

    \ingroup qml-places
*/

QDeclarativeReviewPaginationList::QDeclarativeReviewPaginationList(QObject* parent)
        : QObject(parent)
{
}

QDeclarativeReviewPaginationList::QDeclarativeReviewPaginationList(
    const QPlacePaginationList<QPlaceReview> &src,
    QObject *parent)
        : QObject(parent),
          m_src(src)
{
    synchronizeReview();
}

QDeclarativeReviewPaginationList::~QDeclarativeReviewPaginationList()
{
}

void QDeclarativeReviewPaginationList::setPaginationList(const QPlacePaginationList<QPlaceReview> &src)
{
    QPlacePaginationList<QPlaceReview> previous = m_src;
    m_src = src;

    if (previous.items() != m_src.items()) {
        emit itemsChanged();
    }
    if (previous.data() != m_src.data()) {
        synchronizeReview();
        emit dataChanged();
    }
    if (previous.start() != m_src.start()) {
        emit startChanged();
    }
    if (previous.stop() != m_src.stop()) {
        emit stopChanged();
    }
}

QPlacePaginationList<QPlaceReview> QDeclarativeReviewPaginationList::paginationList()
{
    return m_src;
}

/*!
    \qmlproperty int ReviewPaginationList::items

    This property holds number of items on current page.
*/
int QDeclarativeReviewPaginationList::items()
{
    return m_src.items();
}

/*!
    \qmlproperty int ReviewPaginationList::start

    This property holds number of first item from current page
    on all items list.
*/

void QDeclarativeReviewPaginationList::setStart(const int &data)
{
    if (m_src.start() != data) {
        m_src.setStart(data);
        emit startChanged();
    }
}

int QDeclarativeReviewPaginationList::start()
{
    return m_src.start();
}

/*!
    \qmlproperty int ReviewPaginationList::stop

    This property holds number of last item from current page
    on all items list.
*/
int QDeclarativeReviewPaginationList::stop()
{
    return m_src.stop();
}

/*!
    \qmlproperty QDeclarativeListProperty<QDeclarativeReviewObject> ReviewPaginationList::data

    This property contains of list of items.
*/
QDeclarativeListProperty<QDeclarativeReview> QDeclarativeReviewPaginationList::data()
{
    return QDeclarativeListProperty<QDeclarativeReview>(this,
                                                          0, // opaque data parameter
                                                          data_append,
                                                          data_count,
                                                          data_at,
                                                          data_clear);
}

void QDeclarativeReviewPaginationList::data_append(QDeclarativeListProperty<QDeclarativeReview> *prop,
                                                  QDeclarativeReview *value)
{
    QDeclarativeReviewPaginationList* object = static_cast<QDeclarativeReviewPaginationList*>(prop->object);
    QDeclarativeReview *altValue = new QDeclarativeReview(object);
    altValue->setReview(value->review());
    object->m_objects.append(altValue);
    QList<QPlaceReview> list = object->m_src.data();
    list.append(altValue->review());
    object->m_src.setData(list);
    emit object->dataChanged();
    emit object->stopChanged();
}

int QDeclarativeReviewPaginationList::data_count(QDeclarativeListProperty<QDeclarativeReview> *prop)
{
    return static_cast<QDeclarativeReviewPaginationList*>(prop->object)->m_objects.count();
}

QDeclarativeReview* QDeclarativeReviewPaginationList::data_at(QDeclarativeListProperty<QDeclarativeReview> *prop,
                                                                          int index)
{
    QDeclarativeReviewPaginationList* object = static_cast<QDeclarativeReviewPaginationList*>(prop->object);
    QDeclarativeReview *res = NULL;
    if (object->m_objects.count() > index && index > -1) {
        res = object->m_objects[index];
    }
    return res;
}

void QDeclarativeReviewPaginationList::data_clear(QDeclarativeListProperty<QDeclarativeReview> *prop)
{
    QDeclarativeReviewPaginationList* object = static_cast<QDeclarativeReviewPaginationList*>(prop->object);
    qDeleteAll(object->m_objects);
    object->m_objects.clear();
    object->m_src.setData(QList<QPlaceReview>());
    emit object->dataChanged();
    emit object->stopChanged();
}

void QDeclarativeReviewPaginationList::synchronizeReview()
{
    qDeleteAll(m_objects);
    m_objects.clear();
    foreach (QPlaceReview value, m_src.data()) {
        QDeclarativeReview* declarativeValue = new QDeclarativeReview(value, this);
        m_objects.append(declarativeValue);
    }
}
