/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef QGEOAREAMONITOR_H
#define QGEOAREAMONITOR_H

#include <QtLocation/QGeoCoordinate>

#include <QObject>

QT_BEGIN_NAMESPACE

class QGeoPositionInfo;
class QGeoAreaMonitorPrivate;
class Q_LOCATION_EXPORT QGeoAreaMonitor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QGeoCoordinate center READ center WRITE setCenter)
    Q_PROPERTY(qreal radius READ radius WRITE setRadius)

public:
    explicit QGeoAreaMonitor(QObject *parent);
    virtual ~QGeoAreaMonitor() = 0;

    virtual void setCenter(const QGeoCoordinate &coordinate);
    QGeoCoordinate center() const;

    virtual void setRadius(qreal radius);
    qreal radius() const;

    static QGeoAreaMonitor *createDefaultMonitor(QObject *parent);

Q_SIGNALS:
    void areaEntered(const QGeoPositionInfo &update);
    void areaExited(const QGeoPositionInfo &update);

private:
    Q_DISABLE_COPY(QGeoAreaMonitor)
    QGeoAreaMonitorPrivate *d;
};


QT_END_NAMESPACE

#endif
