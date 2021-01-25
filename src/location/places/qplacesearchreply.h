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

#ifndef QPLACESEARCHREPLY_H
#define QPLACESEARCHREPLY_H

#include <QtLocation/QPlaceReply>
#include <QtLocation/QPlaceSearchResult>

QT_BEGIN_NAMESPACE

class QPlaceSearchResult;
class QPlaceSearchReplyPrivate;

class Q_LOCATION_EXPORT QPlaceSearchReply : public QPlaceReply
{
    Q_OBJECT
public:
    explicit QPlaceSearchReply(QObject *parent = nullptr);
    ~QPlaceSearchReply();

    QPlaceReply::Type type() const;

    QList<QPlaceSearchResult> results() const;
    QPlaceSearchRequest request() const;

    QPlaceSearchRequest previousPageRequest() const;
    QPlaceSearchRequest nextPageRequest() const;

protected:
    void setResults(const QList<QPlaceSearchResult> &results);
    void setRequest(const QPlaceSearchRequest &request);
    void setPreviousPageRequest(const QPlaceSearchRequest &previous);
    void setNextPageRequest(const QPlaceSearchRequest &next);

private:
    Q_DISABLE_COPY(QPlaceSearchReply)
    Q_DECLARE_PRIVATE(QPlaceSearchReply)
};

QT_END_NAMESPACE

#endif
