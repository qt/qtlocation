#ifndef QPLACEPAGINATIONLIST_H
#define QPLACEPAGINATIONLIST_H

#include <QSharedDataPointer>
#include <QList>
#include "qplaceglobal.h"
#include "qplacesupplier.h"

namespace QT_PLACES_NAMESPACE {

/*!
    \class QPlacePaginationList

    \inmodule QPlaces

    \brief The QPlacePaginationList class represents a list object.

    Each QPlacePaginationList represents a pagination-enabled list. It contais list
    of items on current page and ifnormations about indexes of those items.

*/
template <typename T>
class Q_PLACES_EXPORT QPlacePaginationList
{
public:
    QPlacePaginationList() : startIndx(0) {};
    QPlacePaginationList(const QPlacePaginationList<T> &other) {
        this->startIndx = other.startIndx;
        this->dataList = other.dataList;
    }

    ~QPlacePaginationList() {};

    QPlacePaginationList<T> &operator=(const QPlacePaginationList<T> &other) {
        this->startIndx = other.startIndx;
        this->dataList = other.dataList;
        return *this;
    }

    bool operator==(const QPlacePaginationList<T> &other) const {
        return (
                this->startIndx == other.startIndx
                && this->dataList == other.dataList
        );
    }
    bool operator!=(const QPlacePaginationList<T> &other) const {
        return !(other == *this);
    }

    /*!
        Returns items count on current page.
    */
    int items() { return dataList.count(); }
    /*!
        Returns index of first item on current page.
    */
    int start() { return startIndx; }
    /*!
        Returns index of last item on current page.
    */
    int stop() { return startIndx + dataList.count(); }
    /*!
        Returns items list on current page.
    */
    QList<T> data() { return dataList; }
    /*!
        Sets index of first item on current page.
    */
    void setStart(const int &start) { startIndx = start; }
    /*!
        Sets list of items on current page.
    */
    void setData(const QList<T> &data) { dataList = data; }
    /*!
        Append item to the list.
    */
    void addItem(const T &item) { dataList.append(item); }
private:
    int startIndx;
    QList<T> dataList;
};

} // QT_PLACES_NAMESPACE

#endif // QPLACEPAGINATIONLIST_H
