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

#include "qdeclarativeplaceeditorialmodel.h"

#include <QtCore/QUrl>
#include <QtLocation/QPlaceEditorial>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass PlaceEditorialModel QDeclarativePlaceEditorialModel

    \brief The PlaceEditorialModel element provides access to place editorials.
    \ingroup qml-places
    \since 5.0

    The PlaceEditorialModel is a read-only model used to fetch editorials related to a Place.  The
    model performs fetches incrementally.  The number of editorials which are fetched at a time is
    specified by the batchSize property.  The total number of editorials available can be accessed
    via the totalCount property and the number of fetch editorials via the count property.

    The model returns data for the following roles:

    \table
        \header
            \o Role
            \o Type
            \o Description
        \row
            \o content
            \o string
            \o The editorial content.
        \row
            \o title
            \o string
            \o The title of the editorial.
        \row
            \o supplier
            \o Supplier
            \o The supplier of the editorial.
        \row
            \o url
            \o url
            \o The source URL of the editorial.
        \row
            \o language
            \o string
            \o The language that the editorial is written in.
    \endtable
*/

/*!
    \qmlproperty Place PlaceEditorialModel::place

    This property holds the Place that the editorials are for.
*/

/*!
    \qmlproperty int PlaceEditorialModel::batchSize

    This property holds the batch size to use when fetching more editorials items.
*/

/*!
    \qmlproperty int PlaceEditorialModel::totalCount

    This property holds the total number of editorials items for the place.
*/

QDeclarativePlaceEditorialModel::QDeclarativePlaceEditorialModel(QObject *parent)
:   QDeclarativePlaceContentModel(QPlaceContent::EditorialType, parent)
{
    QHash<int, QByteArray> roleNames;
    roleNames.insert(ContentRole, "content");
    roleNames.insert(TitleRole, "title");
    roleNames.insert(SupplierRole, "supplier");
    roleNames.insert(UrlRole, "url");
    roleNames.insert(LanguageRole, "language");
    setRoleNames(roleNames);
}

QDeclarativePlaceEditorialModel::~QDeclarativePlaceEditorialModel()
{
}

QVariant QDeclarativePlaceEditorialModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= rowCount(index.parent()) || index.row() < 0)
        return QVariant();

    const QPlaceEditorial &description = m_content.value(index.row());

    switch (role) {
    case ContentRole:
        return description.content();
    case TitleRole:
        return description.title();
    case UrlRole:
        return description.sourceUrl();
    case LanguageRole:
        return description.language();
    }

    return QDeclarativePlaceContentModel::data(index, role);
}

QT_END_NAMESPACE
