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

#ifndef QGEOROUTEREPLY_H
#define QGEOROUTEREPLY_H

#include "qgeoroute.h"

#include <QList>
#include <QObject>

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QGeoRouteRequest;
class QGeoRouteReplyPrivate;

class Q_LOCATION_EXPORT QGeoRouteReply : public QObject
{
    Q_OBJECT
public:
    enum Error {
        NoError,
        EngineNotSetError,
        CommunicationError,
        ParseError,
        UnsupportedOptionError,
        UnknownError
    };

    QGeoRouteReply(Error error, const QString &errorString, QObject *parent = 0);
    virtual ~QGeoRouteReply();

    bool isFinished() const;
    Error error() const;
    QString errorString() const;

    QGeoRouteRequest request() const;
    QList<QGeoRoute> routes() const;

    virtual void abort();

Q_SIGNALS:
    void finished();
    void error(QGeoRouteReply::Error error, const QString &errorString = QString());

protected:
    QGeoRouteReply(const QGeoRouteRequest &request, QObject *parent = 0);

    void setError(Error error, const QString &errorString);
    void setFinished(bool finished);

    void setRoutes(const QList<QGeoRoute> &routes);

private:
    QGeoRouteReplyPrivate *d_ptr;
    Q_DISABLE_COPY(QGeoRouteReply)
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif
