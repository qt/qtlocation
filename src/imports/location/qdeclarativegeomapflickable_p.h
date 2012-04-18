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

#ifndef QDECLARATIVEGEOMAPFLICKABLE_H
#define QDECLARATIVEGEOMAPFLICKABLE_H


#include <QtQml/qqml.h>
#include <QtGui/qevent.h>
#include <QElapsedTimer>
#include <QVector>
#include <QObject>
#include <QDebug>
#include "qdeclarativegeomapgesturearea_p.h"

QT_BEGIN_NAMESPACE

class QGraphicsSceneMouseEvent;
class QPropertyAnimation;
class QGeoCameraData;
class QGeoMap;

// Note: this class will be deprecated in future versions, it remains as a wrapper
// Please use the gesture object instead.
class QDeclarativeGeoMapFlickable: public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal deceleration READ deceleration WRITE setDeceleration NOTIFY decelerationChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged())

public:
    QDeclarativeGeoMapFlickable(QObject *parent, QDeclarativeGeoMapGestureArea *gestureArea_);
    ~QDeclarativeGeoMapFlickable();

    qreal deceleration() const { return gestureArea_->flickDeceleration(); }
    void setDeceleration(qreal deceleration){ gestureArea_->setFlickDeceleration(deceleration); }

    bool enabled() const { return gestureArea_->panEnabled(); }
    void setEnabled(bool enabled){ gestureArea_->setPanEnabled(enabled); }

    void setMap(QGeoMap* map){ gestureArea_->setMap(map); }

signals:
    void decelerationChanged();
    void enabledChanged();
    void movementStarted();
    void movementEnded();
    void flickStarted();
    void flickEnded();

private:
    QDeclarativeGeoMapGestureArea *gestureArea_; // the destination for this wrapper class
};

QT_END_NAMESPACE
QML_DECLARE_TYPE(QDeclarativeGeoMapFlickable);

#endif
