/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
***************************************************************************/

#include "qdeclarativelandmarkfilters_p.h"
#include "qdeclarativeposition_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass LandmarkNameFilter QDeclarativeLandmarkNameFilter
    \brief The LandmarkNameFilter element specifies a name filter for landmark model.
    \ingroup qml-location-landmarks

    The LandmarkNameFilter element specifies a name filter for landmark model.
    Logical combinations of this and other landmark filters can be
    formed using \l LandmarkIntersectionFilter and \l LandmarkUnionFilter.
    Currently wild card matching is not supported (only exact matches).

    This element is part of the \bold{QtMobility.location 1.1} module.

    \snippet doc/src/snippets/declarative/declarative-landmark.qml Landmark name filter

    \sa LandmarkModel, {QLandmarkNameFilter}

*/

QDeclarativeLandmarkNameFilter::QDeclarativeLandmarkNameFilter(QObject *parent) :
        QDeclarativeLandmarkFilterBase(parent)
{
}

QDeclarativeLandmarkNameFilter::~QDeclarativeLandmarkNameFilter()
{
}

QString QDeclarativeLandmarkNameFilter::name() const
{
    return m_name;
}

/*!
    \qmlproperty QString LandmarkNameFilter::name
    The name string to match landmark names against.
    Currently regular expression (/wildcard) matching is not supported.
*/

void QDeclarativeLandmarkNameFilter::setName(const QString& name)
{
    if (name == m_name)
        return;
    m_name = name;
    m_filter.setName(m_name);
    emit nameChanged();
    emit filterContentChanged();
}

QLandmarkFilter* QDeclarativeLandmarkNameFilter::filter()
{
    if (m_name.isEmpty())
        return 0;
    return &m_filter;
}

/*!
    \qmlclass LandmarkCategoryFilter QDeclarativeLandmarkCategoryFilter
    \brief The LandmarkCategoryFilter element specifies a category filter for landmark model.
    \ingroup qml-location-landmarks

    This element is part of the \bold{QtMobility.location 1.1} module.

    Logical combinations of this and other landmark filters can be
    formed using \l LandmarkIntersectionFilter and \l LandmarkUnionFilter.

    \sa LandmarkModel, LandmarkCategoryModel, {QLandmarkCategoryFilter}
*/

QDeclarativeLandmarkCategoryFilter::QDeclarativeLandmarkCategoryFilter(QObject* parent) :
        QDeclarativeLandmarkFilterBase(parent), m_category(0)
{
}

QDeclarativeLandmarkCategoryFilter::~QDeclarativeLandmarkCategoryFilter()
{
}

QDeclarativeLandmarkCategory* QDeclarativeLandmarkCategoryFilter::category() const
{
    return m_category;
}

/*!
    \qmlproperty LandmarkCategory LandmarkCategoryFilter::category
    Category for whose landmarks should be filtered.
*/

void QDeclarativeLandmarkCategoryFilter::setCategory(QDeclarativeLandmarkCategory* category)
{
    m_category = category;
    m_filter.setCategoryId(m_category->category().categoryId());
    emit categoryChanged();
    emit filterContentChanged();
}

QLandmarkFilter* QDeclarativeLandmarkCategoryFilter::filter()
{
    if (!m_category)
        return 0;
    return &m_filter;
}


/*!
    \qmlclass LandmarkBoxFilter QDeclarativeLandmarkBoxFilter
    \brief The LandmarkBoxFilter element specifies a box (rectangle) filter for landmark model.
    \ingroup qml-location-landmarks

    This element is part of the \bold{QtMobility.location 1.1} module.

    The LandmarkBoxFilter element specifies a box (rectangle) filter for landmark model.
    Logical combinations of this and other landmark filters can be
    formed using \l LandmarkIntersectionFilter and \l LandmarkUnionFilter.

    The following example creates a filter which filters for landmarks located within
    the given box (rectangle between top left and bottom right coordinates).

    \snippet doc/src/snippets/declarative/declarative-landmark.qml Box filter

    \sa LandmarkModel, {QLandmarkBoxFilter}

*/

QDeclarativeLandmarkBoxFilter::QDeclarativeLandmarkBoxFilter(QObject* parent) :
        QDeclarativeLandmarkFilterBase(parent), m_topLeft(0), m_bottomRight(0)
{
}

QDeclarativeLandmarkBoxFilter::~QDeclarativeLandmarkBoxFilter()
{
}

QDeclarativeCoordinate* QDeclarativeLandmarkBoxFilter::topLeft() const
{
    return m_topLeft;
}

/*!
    \qmlproperty Coordinate LandmarkBoxFilter::topLeft
    Top left coordinate of the box to filter (landmarks within the boundaries).
*/

void QDeclarativeLandmarkBoxFilter::setTopLeft(QDeclarativeCoordinate* coordinate)
{
    if (m_topLeft == coordinate)
        return;

    m_topLeft = coordinate;

    if (m_topLeft && m_bottomRight)
        m_filter.setBoundingBox(QGeoBoundingBox(m_topLeft->coordinate(), m_bottomRight->coordinate()));
    emit topLeftChanged();
    emit filterContentChanged();
}

/*!
    \qmlproperty Coordinate LandmarkBoxFilter::bottomRight
    Bottom right coordinate of the box to filter (landmarks within the boundaries).
*/

QDeclarativeCoordinate* QDeclarativeLandmarkBoxFilter::bottomRight() const
{
    return m_bottomRight;
}

void QDeclarativeLandmarkBoxFilter::setBottomRight(QDeclarativeCoordinate* coordinate)
{
    if (m_bottomRight == coordinate)
        return;

    m_bottomRight = coordinate;
    if (m_topLeft && m_bottomRight)
        m_filter.setBoundingBox(QGeoBoundingBox(m_topLeft->coordinate(), m_bottomRight->coordinate()));
    emit bottomRightChanged();
    emit filterContentChanged();
}

QLandmarkFilter* QDeclarativeLandmarkBoxFilter::filter()
{
    if (!m_topLeft || !m_bottomRight)
        return 0;
    return &m_filter;
}

/*!
    \qmlclass LandmarkProximityFilter QDeclarativeLandmarkProximityFilter
    \brief The LandmarkProximityFilter element specifies a proximity filter for landmark model.
    \ingroup qml-location-landmarks

    The LandmarkProximityFilter element specifies a proximity filter for landmark model.
    Logical combinations of this and other landmark filters can be
    formed using \l LandmarkIntersectionFilter and \l LandmarkUnionFilter.

    This element is part of the \bold{QtMobility.location 1.1} module.

    The following example creates a filter which filters for landmarks located within
    1500 metres of the current position.

    \snippet doc/src/snippets/declarative/declarative-landmark.qml Landmark proximity filter

    \sa LandmarkModel, {QLandmarkProximityFilter}
*/

QDeclarativeLandmarkProximityFilter::QDeclarativeLandmarkProximityFilter(QObject *parent) :
        QDeclarativeLandmarkFilterBase(parent), m_radius(0), m_coordinate(0)
{
}

QDeclarativeLandmarkProximityFilter::~QDeclarativeLandmarkProximityFilter()
{
}

double QDeclarativeLandmarkProximityFilter::radius() const
{
    return m_radius;
}

/*!
    \qmlproperty double LandmarkProximityFilter::radius
    Radius (in metres) from the \l center coordinate to filter
    (landmarks within that area).

    \sa center
*/

void QDeclarativeLandmarkProximityFilter::setRadius(const double radius)
{
    if (radius == m_radius)
        return;
    m_radius = radius;
    emit radiusChanged();
    emit filterContentChanged();
}

QDeclarativeCoordinate* QDeclarativeLandmarkProximityFilter::center() const
{
    return m_coordinate;
}

/*!
    \qmlproperty Coordinate LandmarkProximityFilter::center
    Center coordinate in whose proximity landmarks should be filtered.

    \sa radius
*/

void QDeclarativeLandmarkProximityFilter::setCenter(QDeclarativeCoordinate* coordinate)
{
    if (m_coordinate == coordinate)
        return;
    m_coordinate = coordinate;

    if (m_coordinate) {
        QObject::connect(m_coordinate, SIGNAL(latitudeChanged(double)), this, SIGNAL(filterContentChanged()));
        QObject::connect(m_coordinate, SIGNAL(longitudeChanged(double)), this, SIGNAL(filterContentChanged()));
    }
    emit centerChanged();
    emit filterContentChanged();
}

QLandmarkFilter* QDeclarativeLandmarkProximityFilter::filter()
{
    if (!m_coordinate)
        return 0;
    // Populate filter only now in case their contents have changed.
    m_filter.setRadius(m_radius);
    m_filter.setCenter(m_coordinate->coordinate());
    return &m_filter;
}


QDeclarativeLandmarkCompoundFilter::QDeclarativeLandmarkCompoundFilter(QObject* parent)
        : QDeclarativeLandmarkFilterBase(parent)
{
}

/*!
    \qmlproperty list<Landmark*Filter> LandmarkUnionFilter::filters
    \default

    The filters to OR together.
*/

/*!
    \qmlproperty list<Landmark*Filter> LandmarkIntersectionFilter::filters
    \default

    The filters to AND together.
*/

QDeclarativeListProperty<QDeclarativeLandmarkFilterBase> QDeclarativeLandmarkCompoundFilter::filters()
{
    return QDeclarativeListProperty<QDeclarativeLandmarkFilterBase>(this,
                                                          0, // opaque data parameter
                                                          filters_append,
                                                          filters_count,
                                                          filters_at,
                                                          filters_clear);
}

void QDeclarativeLandmarkCompoundFilter::filters_append(QDeclarativeListProperty<QDeclarativeLandmarkFilterBase>* prop, QDeclarativeLandmarkFilterBase* filter)
{
    QDeclarativeLandmarkCompoundFilter* compoundFilter = static_cast<QDeclarativeLandmarkCompoundFilter*>(prop->object);
    compoundFilter->m_filters.append(filter);
    QObject::connect(filter, SIGNAL(filterContentChanged()), compoundFilter, SIGNAL(filterContentChanged()));
    emit compoundFilter->filterContentChanged();
}

int QDeclarativeLandmarkCompoundFilter::filters_count(QDeclarativeListProperty<QDeclarativeLandmarkFilterBase>* prop)
{
    // The 'prop' is in a sense 'this' for this static function (as given in filters() function)
    return static_cast<QDeclarativeLandmarkCompoundFilter*>(prop->object)->m_filters.count();
}

QDeclarativeLandmarkFilterBase* QDeclarativeLandmarkCompoundFilter::filters_at(QDeclarativeListProperty<QDeclarativeLandmarkFilterBase>* prop, int index)
{
    return static_cast<QDeclarativeLandmarkCompoundFilter*>(prop->object)->m_filters.at(index);
}

void QDeclarativeLandmarkCompoundFilter::filters_clear(QDeclarativeListProperty<QDeclarativeLandmarkFilterBase>* prop)
{
    QDeclarativeLandmarkCompoundFilter* filter = static_cast<QDeclarativeLandmarkCompoundFilter*>(prop->object);
    qDeleteAll(filter->m_filters);
    filter->m_filters.clear();
}

template <class T>
        bool QDeclarativeLandmarkCompoundFilter::appendFilters(T* compoundFilter)
{
    // Creates a T type compound filter of all filters.
    if (m_filters.isEmpty())
        return false;
    compoundFilter->clear();
    for (int i = 0; i < m_filters.count(); i++) {
        compoundFilter->append(*m_filters.at(i)->filter());
    }
    return true;
}

/*!
    \qmlclass LandmarkUnionFilter QDeclarativeLandmarkUnionFilter
    \brief The LandmarkUnionFilter element specifies a union of landmark filters.
    \ingroup qml-location-landmarks

    This element is part of the \bold{QtMobility.location 1.1} module.

    Logical OR combinations of landmark filters can be formed using LandmarkUnionFilter.

    The following example creates a filter which filters for landmarks which are
    either named \c Nimbin, or located within 1500 metres from current location
    (or both).

    \snippet doc/src/snippets/declarative/declarative-landmark.qml LandmarkModel union filter

    \sa LandmarkIntersectionFilter LandmarkModel, {QLandmarkUnionFilter}

*/

QLandmarkFilter* QDeclarativeLandmarkUnionFilter::filter()
{
    return appendFilters<QLandmarkUnionFilter>(&m_filter) ? &m_filter : 0;
}

QDeclarativeLandmarkUnionFilter::QDeclarativeLandmarkUnionFilter(QObject* parent)
        : QDeclarativeLandmarkCompoundFilter(parent)
{
}

/*!
    \qmlclass LandmarkIntersectionFilter QDeclarativeLandmarkIntersectionFilter
    \brief The LandmarkIntersectionFilter element specifies an insection of landmark filters.
    \ingroup qml-location-landmarks

    This element is part of the \bold{QtMobility.location 1.1} module.

    The LandmarkIntersectionFilter element specifies an insection of landmark filters.
    Logical AND combinations of LandmarkFilters can be formed using LandmarkIntersectionFilter.

    The following example creates a filter which filters for landmarks named \c Darwin within
    500 metres of coordinate (20,10).

    \snippet doc/src/snippets/declarative/declarative-landmark.qml LandmarkModel intersection filter

    \sa LandmarkUnionFilter, LandmarkModel, {QLandmarkIntersectionFilter}
*/

QDeclarativeLandmarkIntersectionFilter::QDeclarativeLandmarkIntersectionFilter(QObject* parent)
        : QDeclarativeLandmarkCompoundFilter(parent)
{
}

QLandmarkFilter* QDeclarativeLandmarkIntersectionFilter::filter()
{
    return appendFilters<QLandmarkIntersectionFilter>(&m_filter) ? &m_filter : 0;
}

#include "moc_qdeclarativelandmarkfilters_p.cpp"

QTM_END_NAMESPACE
