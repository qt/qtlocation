/****************************************************************************
**
** Copyright (C) 2016 Aaron McCarthy <mccarthy.aaron@gmail.com>
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

#ifndef QGEOTILEFETCHEROSM_H
#define QGEOTILEFETCHEROSM_H

#include "qgeotileproviderosm.h"
#include <QtLocation/private/qgeotilefetcher_p.h>
#include <QVector>

QT_BEGIN_NAMESPACE

class QNetworkAccessManager;
class QGeoTileFetcherOsmPrivate;

class QGeoTileFetcherOsm : public QGeoTileFetcher
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGeoTileFetcherOsm)

    friend class QGeoMapReplyOsm;
    friend class QGeoTiledMappingManagerEngineOsm;
public:
    QGeoTileFetcherOsm(const QVector<QGeoTileProviderOsm *> &providers,
                       QNetworkAccessManager *nm,
                       QGeoMappingManagerEngine *parent);

    void setUserAgent(const QByteArray &userAgent);

Q_SIGNALS:
    void providerDataUpdated(const QGeoTileProviderOsm *provider);

protected:
    bool initialized() const override;

protected Q_SLOTS:
    void onProviderResolutionFinished(const QGeoTileProviderOsm *provider);
    void onProviderResolutionError(const QGeoTileProviderOsm *provider);
    void restartTimer();

private:
    QGeoTiledMapReply *getTileImage(const QGeoTileSpec &spec) override;
    void readyUpdated();

    QByteArray m_userAgent;
    QVector<QGeoTileProviderOsm *> m_providers;
    QNetworkAccessManager *m_nm;
    bool m_ready;
};

QT_END_NAMESPACE

#endif // QGEOTILEFETCHEROSM_H

