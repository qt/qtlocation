/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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
***************************************************************************/

#include <qnumeric.h>
#include "qdeclarativegeoboundingcircle_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass Yippiaiee! TODO
*/

QDeclarativeGeoBoundingCircle::QDeclarativeGeoBoundingCircle(QObject* parent) :
        QObject(parent)
{
}

QDeclarativeGeoBoundingCircle::QDeclarativeGeoBoundingCircle(const QGeoBoundingCircle& circle, QObject* parent) :
    QObject(parent),
    center_(0),
    circle_(circle)
{
}

void QDeclarativeGeoBoundingCircle::setCircle(const QGeoBoundingCircle& circle)
{
    circle_ = circle;
}

QGeoBoundingCircle QDeclarativeGeoBoundingCircle::circle()
{
    return circle_;
}

Q_INVOKABLE bool QDeclarativeGeoBoundingCircle::contains(QDeclarativeCoordinate* coordinate)
{
    if (!coordinate)
        return false;
    return circle_.contains(coordinate->coordinate());
}

QDeclarativeCoordinate* QDeclarativeGeoBoundingCircle::center()
{
    return center_;
}

void QDeclarativeGeoBoundingCircle::setCenter(QDeclarativeCoordinate *coordinate)
{
    if (coordinate == center_)
        return;
    center_ = coordinate;
    if (center_)
        circle_.setCenter(center_->coordinate());
    else
        circle_.setCenter(QGeoCoordinate());
    emit centerChanged();
}

qreal QDeclarativeGeoBoundingCircle::radius() const
{
    return circle_.radius();
}

void QDeclarativeGeoBoundingCircle::setRadius(const qreal radius)
{
    if (radius == circle_.radius())
        return;
    circle_.setRadius(radius);
    emit radiusChanged();
}

#include "moc_qdeclarativegeoboundingcircle_p.cpp"

QTM_END_NAMESPACE
