/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGEOCODEREPLY_H
#define QGEOCODEREPLY_H

#include <QtLocation/qlocationglobal.h>
#include <QtPositioning/QGeoShape>
#include <QtCore/QObject>
#include <QtCore/QList>

QT_BEGIN_NAMESPACE

class QGeoLocation;
class QGeoCodeReplyPrivate;

class Q_LOCATION_EXPORT QGeoCodeReply : public QObject
{
    Q_OBJECT

public:
    enum Error {
        NoError,
        EngineNotSetError,
        CommunicationError,
        ParseError,
        UnsupportedOptionError,
        CombinationError,
        UnknownError
    };

    explicit QGeoCodeReply(Error error, const QString &errorString, QObject *parent = nullptr);
    virtual ~QGeoCodeReply();

    bool isFinished() const;
    Error error() const;
    QString errorString() const;

    QGeoShape viewport() const;
    QList<QGeoLocation> locations() const;

    qsizetype limit() const;
    qsizetype offset() const;

    virtual void abort();

Q_SIGNALS:
    void finished();
    void aborted();
    void errorOccurred(QGeoCodeReply::Error error, const QString &errorString = QString());

protected:
    explicit QGeoCodeReply(QObject *parent = nullptr);

    void setError(Error error, const QString &errorString);
    void setFinished(bool finished);

    void setViewport(const QGeoShape &viewport);
    void addLocation(const QGeoLocation &location);
    void setLocations(const QList<QGeoLocation> &locations);

    void setLimit(qsizetype limit);
    void setOffset(qsizetype offset);

private:
    QGeoCodeReplyPrivate *d_ptr;
    Q_DISABLE_COPY(QGeoCodeReply)
    friend class QGeoCodeReplyPrivate;
};

QT_END_NAMESPACE

#endif
