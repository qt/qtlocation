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

#include "qdeclarativesearchsuggestionmodel_p.h"
#include "qdeclarativegeoserviceprovider_p.h"

#include <QtDeclarative/QDeclarativeInfo>
#include <QtLocation/QGeoServiceProvider>

#include <qplacemanager.h>
#include <qplacesearchrequest.h>

QT_USE_NAMESPACE

/*!
    \qmlclass PlaceSearchSuggestionModel QDeclarativeSearchSuggestionModel
    \inqmlmodule QtLocation 5
    \ingroup qml-QtLocation5-places
    \ingroup qml-QtLocation5-places-models
    \since QtLocation 5.0

    \brief The PlaceSearchSuggestionModel element provides access to search term suggestions.

    The PlaceSearchSuggestionModel can be used to provide search term suggestions as the user enters their
    search term.  The properties of this model should match that of the \l PlaceSearchModel, which
    will be used to perform the actual search query, to ensure that the search suggestion results are
    relevant.

    There are two ways of accessing the data provided by this model, either through the
    \l suggestions property or through views and delegates.  The latter is the preferred
    method.

    The \l offset and \l limit properties can be used to access paged suggestions.  When the
    \l offset and \l limit properties are set the suggestions between \l offset and
    (\l offset + \l limit - 1) will be returned.  Support for paging may vary
    from plugin to plugin.

    The model returns data for the following roles:

    \table
        \header
            \o Role
            \o Type
            \o Description
        \row
            \o suggestion
            \o string
            \o Suggested search term.
    \endtable

    The following example shows how to use the PlaceSearchSuggestionModel to get suggested search terms
    from a partial search term.  The \l searchArea is set to match what would be used to perform the
    actual place search with \l PlaceSearchModel.

    \snippet snippets/declarative/places.qml QtQuick import
    \snippet snippets/declarative/places.qml QtLocation import
    \codeline
    \snippet snippets/declarative/places.qml SearchSuggestionModel

    \sa PlaceSearchModel, {QPlaceManager}
*/

/*!
    \qmlproperty Plugin PlaceSearchSuggestionModel::plugin

    This property holds the provider \l Plugin which will be used to perform the search.
*/

/*!
    \qmlproperty BoundingArea PlaceSearchSuggestionModel::searchArea

    This property holds the search area.  Search suggestion results returned by the model will be
    relevant to the given search area.

    If this property is set to a \l BoundingCircle its \l {BoundingCircle::radius}{radius} property
    may be left unset, in which case the \l Plugin will choose an appropriate radius for the
    search.
*/

/*!
    \qmlproperty int PlaceSearchSuggestionModel::offset

    This property holds the index of the first item in the model.

    \sa limit
*/

/*!
    \qmlproperty int PlaceSearchSuggestionModel::limit

    This property holds the limit of the number of items that will be returned.

    \sa offset
*/

/*!
    \qmlproperty enum PlaceSearchSuggestionModel::status

    This property holds the status of the model.  It can be one of:

    \table
        \row
            \o SearchSuggestionModel.Ready
            \o The search query has completed and the result are available.
        \row
            \o SearchSuggestionModel.Executing
            \o A search query is currently being executed.
        \row
            \o SearchSuggestionModel.Error
            \o An error occurred when executing the previous search query.
    \endtable
*/

/*!
    \qmlmethod PlaceSearchSuggestionModel::execute()

    Executes a search suggestion query for the partial \l searchTerm and \l searchArea.  Once the
    query completes the model items are updated with the suggestions.

    \sa cancel(), status
*/

/*!
    \qmlmethod PlaceSearchSuggestionModel::cancel()

    Cancels an ongoing search suggestion operation.

    \sa execute(), status
*/

/*!
    \qmlmethod PlaceSearchSuggestionModel::clear()

    Clears the current search suggestions stored in the model.
*/

QDeclarativeSearchSuggestionModel::QDeclarativeSearchSuggestionModel(QObject *parent)
:   QDeclarativeSearchModelBase(parent)
{
    QHash<int, QByteArray> roleNames;
    roleNames = QAbstractItemModel::roleNames();
    roleNames.insert(SearchSuggestionRole, "suggestion");
    setRoleNames(roleNames);
}

QDeclarativeSearchSuggestionModel::~QDeclarativeSearchSuggestionModel()
{
}

/*!
    \qmlproperty string PlaceSearchSuggestionModel::searchTerm

    This property holds the partial search term used in query.
*/
QString QDeclarativeSearchSuggestionModel::searchTerm() const
{
    return m_request.searchTerm();
}

void QDeclarativeSearchSuggestionModel::setSearchTerm(const QString &searchTerm)
{
    if (m_request.searchTerm() == searchTerm)
        return;

    m_request.setSearchTerm(searchTerm);
    emit searchTermChanged();
}

/*!
    \qmlproperty QStringList PlaceSearchSuggestionModel::suggestions

    This property holds the list of predicted search terms that the model currently has.
*/
QStringList QDeclarativeSearchSuggestionModel::suggestions() const
{
    return m_suggestions;
}

void QDeclarativeSearchSuggestionModel::clearData()
{
    QDeclarativeSearchModelBase::clearData();

    m_suggestions.clear();
}

void QDeclarativeSearchSuggestionModel::updateSearchRequest()
{
    QDeclarativeSearchModelBase::updateSearchRequest();
}

int QDeclarativeSearchSuggestionModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)

    return m_suggestions.count();
}

QVariant QDeclarativeSearchSuggestionModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= rowCount(index.parent()) || index.row() < 0)
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
    case SearchSuggestionRole:
        return m_suggestions.at(index.row());
    }

    return QVariant();
}

void QDeclarativeSearchSuggestionModel::queryFinished()
{
    if (!m_reply)
        return;

    QPlaceReply *reply = m_reply;
    m_reply = 0;

    beginResetModel();

    clearData();

    QPlaceSearchSuggestionReply *suggestionReply = qobject_cast<QPlaceSearchSuggestionReply *>(reply);
    m_suggestions = suggestionReply->suggestions();
    emit suggestionsChanged();

    endResetModel();

    if (suggestionReply->error() != QPlaceReply::NoError)
        setStatus(Error, suggestionReply->errorString());
    else
        setStatus(Ready);


    reply->deleteLater();
}

QPlaceReply *QDeclarativeSearchSuggestionModel::sendQuery(QPlaceManager *manager,
                                                        const QPlaceSearchRequest &request)
{
    return manager->searchSuggestions(request);
}
