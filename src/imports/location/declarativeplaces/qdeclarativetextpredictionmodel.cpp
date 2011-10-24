/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativetextpredictionmodel_p.h"
#include "qdeclarativegeoserviceprovider_p.h"

#include <QtDeclarative/QDeclarativeInfo>
#include <QtLocation/QGeoServiceProvider>

#include <qplacemanager.h>
#include <qplacesearchrequest.h>

QT_USE_NAMESPACE

/*!
    \qmlclass TextPredictionModel QDeclarativeTextPredictionModel
    \inqmlmodule QtLocation 5
    \ingroup qml-QtLocation5-places
    \since QtLocation 5.0

    \brief The TextPredictionModel element provides access to text predictions.

    The TextPredictionModel can be used to provide search term predictions as the user enters their
    search term.  The properties of this model should match that of the \l PlaceSearchModel which
    will be used to perform the actual search query to ensure that the text prediction results are
    relevant.

    There are two ways of accessing the data provided by this model, either through the
    \l textPredictions property or through views and delegates.  The latter is the preferred
    method.

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
            \o textPrediction
            \o string
            \o Suggested search term.
    \endtable

    The following example shows how to use the TextPredictionModel to get suggested search queries
    for a partial search term.  The \l searchArea is set to match what would be used to perform the
    actual place search with \l PlaceSearchModel.

    \snippet snippets/declarative/places.qml QtQuick import
    \snippet snippets/declarative/places.qml QtLocation import
    \codeline
    \snippet snippets/declarative/places.qml TextPredictionModel

    \sa PlaceSearchModel, {QPlaceManager}
*/

/*!
    \qmlproperty Plugin TextPredictionModel::plugin

    This property holds the provider \l Plugin which will be used to perform the search.
*/

/*!
    \qmlproperty BoundingArea TextPredictionModel::searchArea

    This property holds the search area.  Text prediction results returned by the model will be
    relevant to the given search area.

    If this property is set to a \l BoundingCircle its \l {BoundingCircle::radius}{radius} property
    may be left unset, in which case the \l Plugin will choose an appropriate radius for the
    search.
*/

/*!
    \qmlproperty int TextPredictionModel::offset

    This property holds the index of the first search result in the model.

    \sa limit
*/

/*!
    \qmlproperty int TextPredictionModel::limit

    This property holds the limit of the number of items that will be returned.

    \sa offset
*/

/*!
    \qmlproperty enum TextPredictionModel::status

    This property holds the status of the model.  It can be one of:

    \table
        \row
            \o TextPredictionModel.Ready
            \o The search query has completed and the result are available.
        \row
            \o TextPredictionModel.Executing
            \o A search query is currently being executed.
        \row
            \o TextPredictionModel.Error
            \o An error occurred when executing the previous search query.
    \endtable
*/

/*!
    \qmlmethod TextPredictionModel::execute()

    Executes a text prediction query for the partial \l searchTerm and \l searchArea.  Once the
    query completes the model items are updated with the text prediction results.

    \sa cancel(), status
*/

/*!
    \qmlmethod TextPredictionModel::cancel()

    Cancels an ongoing text prediction query.

    \sa execute(), status
*/

QDeclarativeTextPredictionModel::QDeclarativeTextPredictionModel(QObject *parent)
:   QDeclarativeSearchModelBase(parent)
{
    QHash<int, QByteArray> roleNames;
    roleNames = QAbstractItemModel::roleNames();
    roleNames.insert(TextPredictionRole, "textPrediction");
    setRoleNames(roleNames);
}

QDeclarativeTextPredictionModel::~QDeclarativeTextPredictionModel()
{
}

/*!
    \qmlproperty string TextPredictionModel::searchTerm

    This property holds the partial search term used in query.
*/
QString QDeclarativeTextPredictionModel::searchTerm() const
{
    return m_request.searchTerm();
}

void QDeclarativeTextPredictionModel::setSearchTerm(const QString &searchTerm)
{
    if (m_request.searchTerm() == searchTerm)
        return;

    m_request.setSearchTerm(searchTerm);
    emit searchTermChanged();
}

/*!
    \qmlproperty QStringList TextPredictionModel::textPredictions

    This property holds the list of predicted search terms that the model currently has.
*/
QStringList QDeclarativeTextPredictionModel::textPredictions() const
{
    return m_predictions;
}

void QDeclarativeTextPredictionModel::clearData()
{
    m_predictions.clear();
}

void QDeclarativeTextPredictionModel::updateSearchRequest()
{
    QDeclarativeSearchModelBase::updateSearchRequest();
}

void QDeclarativeTextPredictionModel::processReply(QPlaceReply *reply)
{
    QPlaceTextPredictionReply *predictionReply = qobject_cast<QPlaceTextPredictionReply *>(reply);
    m_predictions = predictionReply->textPredictions();
    emit textPredictionsChanged();
}

int QDeclarativeTextPredictionModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)

    return m_predictions.count();
}

QVariant QDeclarativeTextPredictionModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= rowCount(index.parent()) || index.row() < 0)
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
    case TextPredictionRole:
        return m_predictions.at(index.row());
    }

    return QVariant();
}

QPlaceReply *QDeclarativeTextPredictionModel::sendQuery(QPlaceManager *manager,
                                                        const QPlaceSearchRequest &request)
{
    return manager->textPredictions(request);
}
