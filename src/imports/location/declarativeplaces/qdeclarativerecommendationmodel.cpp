/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtLocation module of the Qt Toolkit.
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativerecommendationmodel_p.h"
#include "qdeclarativeplace_p.h"

#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativeInfo>
#include <QtLocation/QGeoServiceProvider>
#include <QtLocation/QPlaceManager>

#include <QtLocation/QPlaceSearchReply>
#include <QtLocation/QPlaceMatchRequest>
#include <QtLocation/QPlaceMatchReply>

QT_USE_NAMESPACE

/*!
    \qmlclass PlaceRecommendationModel QDeclarativeRecommenadationModel
    \inqmlmodule QtLocation 5
    \ingroup qml-QtLocation5-places
    \ingroup qml-QtLocation5-places-models
    \since QtLocation 5.0

    \brief The PlaceRecommendationModel element provides a model of place recommendations.

    PlaceRecommendationModel provides a model of place recommendation results within the \l searchArea,
    that are similar to the place identified by the \l placeId property.

    The \l offset and \l limit properties can be used to access paged search results.  When the
    \l offset and \l limit properties are set the search results between \l offset and
    (\l offset + \l limit - 1) will be returned.

    The model returns data for the following roles:

    \table
        \header
            \o Role
            \o Type
            \o Description
        \row
            \o distance
            \o real
            \o The distance to the place.
        \row
            \o place
            \o \l Place
            \o The place.
    \endtable

    The following example shows how to use the PlaceRecommendationModel to search for recommendations in
    close proximity of a given position.

    \snippet snippets/declarative/places.qml QtQuick import
    \snippet snippets/declarative/places.qml QtLocation import
    \codeline
    \snippet snippets/declarative/places.qml RecommendationModel

    \sa PlaceSearchModel, CategoryModel, {QPlaceManager}
*/

/*!
    \qmlproperty Plugin PlaceRecommendationModel::plugin

    This property holds the \l Plugin which will be used to perform the search.
*/

/*!
    \qmlproperty Plugin PlaceRecommendationModel::favoritesPlugin

    This property holds the \l Plugin which will be used to look for favorites.
    Any places from the recommendation search which can be cross-referenced/matched
    in the favorites \l Plugin will have their \l {Place::favorite}{favorite} property
    set with the \l Place from the favorites \l Plugin.

    If the favoritesPlugin is not set, the \l {Place::favorite}{favorite} property
    of the places in the results will always be null.

    \sa Favorites
*/

/*!
    \qmlproperty VariantMap PlaceRecommendationModel::favoritesMatchParameters

    This property holds a set of parameters used to specify how recommended places
    are matched to favorites in the favoritesPlugin.

    By default the parameter map is empty and implies that the favorites plugin
    matches by \l {Alternative Id cross-referencing}{alternative ids}.  Generally,
    an application developer will not need to set this property.

    In cases where the favorites plugin does not support matching by alternative ids,
    then the \l {Information about plugins} {backend plugin documentation} should be consulted
    to see precisely what key-value parameters to set.
*/

/*!
    \qmlproperty BoundingArea PlaceRecommendationModel::searchArea

    This property holds the search area.  Search results returned by the model will be within the
    search area.

    If this property is set to a \l BoundingCircle its \l {BoundingCircle::radius}{radius} property
    may be left unset, in which case the \l Plugin will choose an appropriate radius for the
    search.
*/

/*!
    \qmlproperty int PlaceRecommendationModel::offset

    This property holds the index of the first search result in the model.

    \sa limit
*/

/*!
    \qmlproperty int PlaceRecommendationModel::limit

    This property holds the limit of the number of items that will be returned.

    \sa offset
*/

/*!
    \qmlproperty enum PlaceRecommendationModel::status

    This property holds the status of the model.  It can be one of:

    \table
        \row
            \o PlaceRecommendationModel.Ready
            \o The search query has completed and the result are available.
        \row
            \o PlaceRecommendationModel.Executing
            \o A search query is currently being executed.
        \row
            \o PlaceRecommendationModel.Error
            \o An error occurred when executing the previous search query.
    \endtable
*/

/*!
    \qmlmethod PlaceRecommendationModel::execute()

    Executes a recommendation search query for places similar to the place identified by the
    \l placeId property.  Once the query completes the model items are updated with the search
    results.

    \sa cancel(), status
*/

/*!
    \qmlmethod PlaceRecommendationModel::cancel()

    Cancels an ongoing search query.

    \sa execute(), status
*/

/*!
    \qmlmethod PlaceRecommendationModel::clear()

    Clears the current place recommendations stored in the model.
*/

QDeclarativeRecommendationModel::QDeclarativeRecommendationModel(QObject *parent)
:   QDeclarativeResultModelBase(parent)
{
}

QDeclarativeRecommendationModel::~QDeclarativeRecommendationModel()
{
}

/*!
    \qmlproperty string PlaceRecommendationModel::placeId

    This property holds place id used in the recommendation search query.
*/
QString QDeclarativeRecommendationModel::placeId() const
{
    return m_placeId;
}

void QDeclarativeRecommendationModel::setPlaceId(const QString &placeId)
{
    if (m_placeId == placeId)
        return;

    m_placeId = placeId;
    emit placeIdChanged();
}

/*!
    \qmlproperty string PlaceRecommendationModel::count

    This property holds the number of results the model has.
*/

/*!
    \qmlmethod PlaceRecommendationModel::data(int index, string role)

    Returns the data for a given \a role at the specified row \a index.
*/

QPlaceReply *QDeclarativeRecommendationModel::sendQuery(QPlaceManager *manager,
                                                        const QPlaceSearchRequest &request)
{
    return manager->recommendations(m_placeId, request);
}
