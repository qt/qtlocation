/****************************************************************************
**
** Copyright (C) 2013-2018 Esri <contracts@esri.com>
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
**
**
**
****************************************************************************/

#ifndef PLACESEARCHREPLYESRI_H
#define PLACESEARCHREPLYESRI_H

#include <QtLocation/QPlaceSearchReply>
#include <QNetworkReply>

QT_BEGIN_NAMESPACE

class PlaceManagerEngineEsri;
class QNetworkReply;
class QPlaceResult;

class PlaceSearchReplyEsri : public QPlaceSearchReply
{
    Q_OBJECT

public:
    PlaceSearchReplyEsri(const QPlaceSearchRequest &request, QNetworkReply *reply,
                         const QHash<QString, QString> &candidateFields,
                         const QHash<QString, QString> &countries, PlaceManagerEngineEsri *parent);
    ~PlaceSearchReplyEsri();

    QString requestUrl;

private slots:
    void setError(QPlaceReply::Error errorCode, const QString &errorString);
    void replyFinished();
    void networkError(QNetworkReply::NetworkError error);

private:
    QPlaceResult parsePlaceResult(const QJsonObject &item) const;

    const QHash<QString, QString> &m_candidateFields;
    const QHash<QString, QString> &m_countries;
};

QT_END_NAMESPACE

#endif // PLACESEARCHREPLYESRI_H
