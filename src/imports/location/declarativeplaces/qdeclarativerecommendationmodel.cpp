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

#include <QtQml/QQmlEngine>
#include <QtQml/QQmlInfo>
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
    \since Qt Location 5.0

    \brief The PlaceRecommendationModel element provides a model of place recommendations.

    PlaceRecommendationModel provides a model of place recommendation results that are
    similar to the place identified by the \l placeId property.

    The following roles can be used to access each recommendation result:
    \table
        \header
            \li Role
            \li Type
            \li Description
        \row
            \li distance
            \li real
            \li If a search area has been specified and is supported by the \l plugin,
                the distance is from the recommended place to the search center, otherwise the distance is
                from the recommended place to the place identified by \l placeId.  If no area has been
                specified, the distance is NaN.
        \row
            \li place
            \li \l Place
            \li The recommended place.
    \endtable

    The following example shows how to use the PlaceRecommendationModel to search for recommendations in
    similar to a given place by providing a place identifier.  Note that model does not incrementally fetch
    recommendations but rather performs a single fetch when \l execute() is run.  The \l count is set to the
    number of recommendations returned during the fetch.

    \snippet snippets/declarative/places.qml QtQuick import
    \snippet snippets/declarative/places.qml QtLocation import
    \codeline
    \snippet snippets/declarative/places.qml RecommendationModel

    \sa PlaceSearchModel, CategoryModel, {QPlaceManager}

    \section1 Paging
    The PlaceRecommendationModel API has some limited support
    for paging. The \l offset and \l limit properties can be used to access
    paged search results. When the \l offset and \l limit properties are set
    the search results between \l offset and (\l offset + \l limit - 1) will be
    returned. For example, if the backend has 5 recommendations in total
    [a,b,c,d,e], an offset of 0 specifies that the first item returned in the
    model will be 'a'. An offset of 1 secifies that the first item in the model
    will be 'b' and so on. The limit specifies the maximum number of items to
    be returned. For example, assuming an offset of 0 and limit of 3 then a,b,c is
    returned. If the offset exceeds (or equals) the total number of items, then
    0 results are returned in the model. Note that the API currently does not
    support a means to retrieve the total number of items available from the
    backed. Also note that support for \l offset and \l limit can vary
    according to the \l plugin.
*/

/*!
    \qmlproperty Plugin PlaceRecommendationModel::plugin

    This property holds the \l Plugin which will be used to perform the search.
*/

/*!
    \qmlproperty Plugin PlaceRecommendationModel::favoritesPlugin

    This property holds the \l Plugin which will be used to look for favorites.
    Any places from the recommendation search which can be cross-referenced or matched
    in the favoritesPlugin will have their \l {Place::favorite}{favorite} property
    set to the corresponding \l Place from the favoritesPlugin.

    If the favoritesPlugin is not set, the \l {Place::favorite}{favorite} property
    of the places in the results will always be null.

    \sa Favorites
*/

/*!
    \qmlproperty VariantMap PlaceRecommendationModel::favoritesMatchParameters

    This property holds a set of parameters used to specify how recommended places
    are matched to favorites in the favoritesPlugin.

    By default the parameter map is empty and implies that the favorites plugin
    matches by \l {Alternative Identifier Cross-Referencing}{alternative identifiers}.  Generally,
    an application developer will not need to set this property.

    In cases where the favorites plugin does not support matching by alternative identifiers,
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

    Support for specifying a search area can vary according according to the \l plugin backend
    implementation. For example, some may support a search center only while others may only support
    bounding boxes.
*/

/*!
    \qmlproperty int PlaceRecommendationModel::offset

    This property holds the index of the first search result in the model.

    \sa limit
*/

/*!
    \qmlproperty int PlaceRecommendationModel::limit

    This property holds the maximum number of items that will be returned.

    \sa offset
*/

/*!
    \qmlproperty enum PlaceRecommendationModel::status

    This property holds the status of the model.  It can be one of:

    \table
        \row
            \li PlaceRecommendationModel.Null
            \li No search query has been executed.  The model is empty.
        \row
            \li PlaceRecommendationModel.Ready
            \li The search query has completed and the result are available.
        \row
            \li PlaceRecommendationModel.Loading
            \li A search query is currently being executed.
        \row
            \li PlaceRecommendationModel.Error
            \li An error occurred when executing the previous search query.
    \endtable
*/

/*!
    \qmlmethod PlaceRecommendationModel::update()

    Updates the model based on the provided query parameters.  The model will be populated with a
    list of places similar to the place identified by the \l placeId property.  If the \l plugin
    supports it, additional parameters such as \l limit and \l offset may be specified and then
    \c update() submits the set of criteria to the \l plugin to process.

    While the model is updating the \l status of the model is set to
    \c PlaceRecommendationModel.Loading.  If the model is successfully updated the \l status is set
    to \c PlaceRecommendationModel.Ready, while if it unsuccessfully completes, the \l status is
    set to \c PlaceRecommendationModel.Error and the model is cleared.

    \code
    PlaceRecomendationModel {
        id: model
        plugin:  backendPlugin
        ...
    }

    MouseArea {
        ...
        onClicked: {
            model.placeId = place.placeId
            limit = -1;
            offset = 0;
            model.update();
        }
    }
    \endcode

    \sa cancel(), status
*/

/*!
    \qmlmethod PlaceRecommendationModel::cancel()

    Cancels an ongoing search query.

    \sa execute(), status
*/

/*!
    \qmlmethod PlaceRecommendationModel::reset()

    Resets the model.  All place recommendations are cleared, any outstanding requests are aborted
    and possible errors are cleared.  Model status will be set to PlaceRecommendationModel.Null.
*/

/*!
    \qmlproperty string QtLocation5::PlaceRecommendationModel::errorString

    This read-only property holds the textual presentation of latest place recommendation model error.
    If no error has occurred or if the model was cleared, an empty string is returned.

    An empty string may also be returned if an error occurred which has no associated
    textual representation.
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

    This property holds place identifier used in the recommendation search query.
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
    \qmlproperty int PlaceRecommendationModel::count

    This property holds the number of results the model has.

    Note that it does not refer to the total number of recommendations
    available in the backend.  The total number of recommendations
    is not currently supported by the API.
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

/*!
    \qmlsignal PlaceRecommendationModel::dataChanged()

   Indicates that significant changes have been made to the underlying datastore.

   Applications should act on this signal at their own discretion.  The data
   provided by the model could be out of date and so the model should be reupdated
   sometime, however an immediate reupdate may be disconcerting to users if the results
   change without any action on their part.
*/
