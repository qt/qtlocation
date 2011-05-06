/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeomapoverlay.h"
#include "qgeomapoverlay_p.h"

#include "qgeomapdata.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoMapOverlay
    \brief The QGeoMapOverlay class is used to draw overlays on the map.

    \inmodule QtLocation
    \since 1.1

    \ingroup maps-mapping

    This allows for general drawing to occur in overlays above the map.
*/

/*!
    Constructs a new map overlay object.
*/
QGeoMapOverlay::QGeoMapOverlay()
    : d_ptr(new QGeoMapOverlayPrivate()) {}

/*!
    Destroys this map overlay object.
*/
QGeoMapOverlay::~QGeoMapOverlay()
{
    delete d_ptr;
}

/*!
\fn void QGeoMapOverlay::paint(QPainter *painter, const QStyleOptionGraphicsItem *option)

Paints the overlay on \a painter, using the options \a option.
*/

/*!
    \internal
*/
void QGeoMapOverlay::setMapData(QGeoMapData * mapData)
{
    d_ptr->mapData = mapData;
}

/*!
    Returns the QGeoMapData instance that this overlay is associated, or 0
    if there is not such instance.

    The QGeoMapData instance provides access to information such as the
    zoom level and viewport position as well as methods to convert
    screen positions to coordinates and vice-versa.
*/
QGeoMapData const * QGeoMapOverlay::mapData() const
{
    return d_ptr->mapData;
}

/*******************************************************************************
*******************************************************************************/

QGeoMapOverlayPrivate::QGeoMapOverlayPrivate()
    : mapData(0) {}


QGeoMapOverlayPrivate::~QGeoMapOverlayPrivate() {}

/*******************************************************************************
*******************************************************************************/

QTM_END_NAMESPACE
