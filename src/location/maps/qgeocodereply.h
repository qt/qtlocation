/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#ifndef QGEOCODEREPLY_H
#define QGEOCODEREPLY_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtPositioning/QGeoLocation>

#include <QtLocation/qlocationglobal.h>

QT_BEGIN_NAMESPACE

class QGeoShape;
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

    int limit() const;
    int offset() const;

    virtual void abort();

Q_SIGNALS:
    void finished();
    void aborted();
    void error(QGeoCodeReply::Error error, const QString &errorString = QString());

protected:
    explicit QGeoCodeReply(QObject *parent = nullptr);
    explicit QGeoCodeReply(QGeoCodeReplyPrivate &dd, QObject *parent = nullptr);

    void setError(Error error, const QString &errorString);
    void setFinished(bool finished);

    void setViewport(const QGeoShape &viewport);
    void addLocation(const QGeoLocation &location);
    void setLocations(const QList<QGeoLocation> &locations);

    void setLimit(int limit);
    void setOffset(int offset);

private:
    QGeoCodeReplyPrivate *d_ptr;
    Q_DISABLE_COPY(QGeoCodeReply)
    friend class QGeoCodeReplyPrivate;
};

QT_END_NAMESPACE

#endif
