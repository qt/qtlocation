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

#include "qdeclarativeplaceimagemodel_p.h"
#include "qdeclarativesupplier_p.h"

#include <QtCore/QUrl>
#include <QtLocation/QPlaceImage>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass ImageModel QDeclarativePlaceImageModel
    \inqmlmodule QtLocation 5
    \ingroup qml-QtLocation5-places
    \ingroup qml-QtLocation5-places-models
    \since QtLocation 5.0

    \brief The ImageModel element provides a model of place images.

    The ImageModel is a read-only model used to fetch images related to a \l Place.  The model
    performs fetches incrementally.  The number of images which are fetched at a time is specified
    by the \l batchSize property.  The total number of images available can be accessed via the
    \l totalCount property.

    The model returns data for the following roles:

    \table
        \header
            \o Role
            \o Type
            \o Description
        \row
            \o url
            \o url
            \o The URL of the image.
        \row
            \o imageId
            \o string
            \o The id of the image.
        \row
            \o mimeType
            \o string
            \o The MIME type of the image.
        \row
            \o supplier
            \o \l Supplier
            \o The supplier of the editorial.
        \row
            \o user
            \o \l {QtLocation5::User}{User}
            \o The user who contributed the editorial.
        \row
            \o attribution
            \o string
            \o Attribution text which must be displayed when displaying the editorial.
    \endtable


    \section1 Example

    The following example shows how to display images for a place:

    \snippet snippets/declarative/places.qml QtQuick import
    \snippet snippets/declarative/places.qml QtLocation import
    \codeline
    \snippet snippets/declarative/places.qml ImageModel
*/

/*!
    \qmlproperty Place ImageModel::place

    This property holds the Place that the images are for.
*/

/*!
    \qmlproperty int ImageModel::batchSize

    This property holds the batch size to use when fetching more image items.
*/

/*!
    \qmlproperty int ImageModel::totalCount

    This property holds the total number of image items for the place.
*/

QDeclarativePlaceImageModel::QDeclarativePlaceImageModel(QObject* parent)
:   QDeclarativePlaceContentModel(QPlaceContent::ImageType, parent)
{
    QHash<int, QByteArray> roles = roleNames();
    roles.insert(UrlRole, "url");
    roles.insert(ImageIdRole, "imageId");
    roles.insert(MimeTypeRole, "mimeType");
    setRoleNames(roles);
}

QDeclarativePlaceImageModel::~QDeclarativePlaceImageModel()
{
    qDeleteAll(m_suppliers);
}

QVariant QDeclarativePlaceImageModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= rowCount(index.parent()) || index.row() < 0)
        return QVariant();

    const QPlaceImage &image = m_content.value(index.row());

    switch (role) {
    case UrlRole:
        return image.url();
    case ImageIdRole:
        return image.imageId();
    case MimeTypeRole:
        return image.mimeType();
    }

    return QDeclarativePlaceContentModel::data(index, role);
}

QT_END_NAMESPACE
