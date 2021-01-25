/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtPositioning module of the Qt Toolkit.
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

#ifndef QNMESATELLITEINFOSOURCE_P_H
#define QNMESATELLITEINFOSOURCE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

//#include <QtPositioning/private/qpositioningglobal_p.h>
#include <QtPositioning/qgeosatelliteinfosource.h>
#include <QtPositioning/qgeosatelliteinfo.h>

#include <QObject>
#include <QQueue>
#include <QPointer>
#include <QtCore/qiodevice.h>
#include <QtCore/qtimer.h>

//QT_BEGIN_NAMESPACE

class QNmeaSatelliteInfoSourcePrivate;
class /*Q_POSITIONING_PRIVATE_EXPORT*/ QNmeaSatelliteInfoSource : public QGeoSatelliteInfoSource
{
    Q_OBJECT
public:
    explicit QNmeaSatelliteInfoSource( QObject *parent = nullptr);
    ~QNmeaSatelliteInfoSource() override;

    void setDevice(QIODevice *source);
    QIODevice *device() const;

    void setUpdateInterval(int msec) override;
    int minimumUpdateInterval() const override;
    Error error() const override;

public Q_SLOTS:
    void startUpdates() override;
    void stopUpdates() override;
    void requestUpdate(int timeout = 0) override;

protected:
    QNmeaSatelliteInfoSourcePrivate *d;
    void setError(QGeoSatelliteInfoSource::Error satelliteError);

    friend class QNmeaSatelliteInfoSourcePrivate;
    Q_DISABLE_COPY(QNmeaSatelliteInfoSource)
};

//QT_END_NAMESPACE

#endif
