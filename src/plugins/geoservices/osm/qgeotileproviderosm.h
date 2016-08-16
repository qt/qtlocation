/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QTILEPROVIDEROSM_H
#define QTILEPROVIDEROSM_H

#include <QtLocation/private/qgeomaptype_p.h>

#include <QtCore/QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QPointer>
#include <QTimer>
#include <algorithm>

QT_BEGIN_NAMESPACE

class QGeoTileProviderOsm: public QObject
{
    Q_OBJECT

    friend class QGeoTileFetcherOsm;
    friend class QGeoMapReplyOsm;
    friend class QGeoTiledMappingManagerEngineOsm;
public:
    struct TileProvider {

        static inline void sort2(int &a, int &b)
        {
            if (a > b) {
                int temp=a;
                a=b;
                b=temp;
            }
        }

        TileProvider() : m_valid(false)
        {

        }

        TileProvider(const QString &urlTemplate,
                 const QString &format,
                 const QString &copyRightMap,
                 const QString &copyRightData,
                 int minimumZoomLevel = 0,
                 int maximumZoomLevel = 19) : m_valid(false)
        {
            if (urlTemplate.isEmpty())
                return;
            m_urlTemplate = urlTemplate;

            if (format.isEmpty())
                return;
            m_format = format;

            m_copyRightMap = copyRightMap;
            m_copyRightData = copyRightData;

            if (minimumZoomLevel < 0 || minimumZoomLevel > 30)
                return;
            m_minimumZoomLevel = minimumZoomLevel;

            if (maximumZoomLevel < 0 || maximumZoomLevel > 30 ||  maximumZoomLevel < minimumZoomLevel)
                return;
            m_maximumZoomLevel = maximumZoomLevel;

            // Currently supporting only %x, %y and &z
            int offset[3];
            offset[0] = m_urlTemplate.indexOf(QLatin1String("%x"));
            if (offset[0] < 0)
                return;

            offset[1] = m_urlTemplate.indexOf(QLatin1String("%y"));
            if (offset[1] < 0)
                return;

            offset[2] = m_urlTemplate.indexOf(QLatin1String("%z"));
            if (offset[2] < 0)
                return;

            int sortedOffsets[3];
            std::copy(offset, offset + 3, sortedOffsets);
            sort2(sortedOffsets[0] ,sortedOffsets[1]);
            sort2(sortedOffsets[1] ,sortedOffsets[2]);
            sort2(sortedOffsets[0] ,sortedOffsets[1]);

            int min = sortedOffsets[0];
            int max = sortedOffsets[2];
            int mid = sortedOffsets[1];

            // Initing LUT
            for (int i=0; i<3; i++) {
                if (offset[0] == sortedOffsets[i])
                    paramsLUT[i] = 0;
                else if (offset[1] == sortedOffsets[i])
                    paramsLUT[i] = 1;
                else
                    paramsLUT[i] = 2;
            }

            m_urlPrefix = m_urlTemplate.mid(0 , min);
            m_urlSuffix = m_urlTemplate.mid(max + 2, m_urlTemplate.size() - max - 2);

            paramsSep[0] = m_urlTemplate.mid(min + 2, mid - min - 2);
            paramsSep[1] = m_urlTemplate.mid(mid + 2, max - mid - 2);
            m_valid = true;
        }

        ~TileProvider()
        {
        }

        inline bool isValid() const
        {
            return m_valid;
        }

        inline QString mapCopyRight() const
        {
            return m_copyRightMap;
        }

        inline QString dataCopyRight() const
        {
            return m_copyRightData;
        }

        inline QString styleCopyRight() const
        {
            return m_copyRightStyle;
        }

        inline QString format() const
        {
            return m_format;
        }

        // Optional properties, not needed to construct a provider
        void setStyleCopyRight(const QString &copyright)
        {
            m_copyRightStyle = copyright;
        }

        QUrl tileAddress(int x, int y, int z) const
        {
            if (z < m_minimumZoomLevel || z > m_maximumZoomLevel)
                return QUrl();
            int params[3] = { x, y, z};
            QString url;
            url += m_urlPrefix;
            url += QString::number(params[paramsLUT[0]]);
            url += paramsSep[0];
            url += QString::number(params[paramsLUT[1]]);
            url += paramsSep[1];
            url += QString::number(params[paramsLUT[2]]);
            url += m_urlSuffix;
            return QUrl(url);
        }

        bool m_valid;
        QString m_urlTemplate;
        QString m_format;
        QString m_copyRightMap;
        QString m_copyRightData;
        QString m_copyRightStyle;
        QString m_urlPrefix;
        QString m_urlSuffix;
        int m_minimumZoomLevel;
        int m_maximumZoomLevel;

        int paramsLUT[3];  //Lookup table to handle possibly shuffled x,y,z
        QString paramsSep[2]; // what goes in between %x, %y and %z
    };

    enum Status {Idle,
                 Resolving,
                 Valid,
                 Invalid };

    QGeoTileProviderOsm(const QString &urlRedir,
                     QNetworkAccessManager *nm,
                     const QGeoMapType &mapType,
                     const TileProvider &providerFallback);

    ~QGeoTileProviderOsm();



    QUrl tileAddress(int x, int y, int z) const;
    QString mapCopyRight() const;
    QString dataCopyRight() const;
    QString styleCopyRight() const;
    QString format() const;
    const QGeoMapType &mapType() const;
    bool isValid() const;
    bool isResolved() const;

Q_SIGNALS:
    void resolutionFinished(const QGeoTileProviderOsm *provider);
    void resolutionError(const QGeoTileProviderOsm *provider, QNetworkReply::NetworkError error);

public Q_SLOTS:
    void onNetworkReplyFinished();
    void onNetworkReplyError(QNetworkReply::NetworkError error);
    void resolveProvider();

protected:
    void disableRedirection();
    void handleError(QNetworkReply::NetworkError error);

    QNetworkAccessManager *m_nm;
    QUrl m_urlRedirector;   // The URL from where to fetch the URL template
    TileProvider m_provider;
    TileProvider m_providerFallback;
    QGeoMapType m_mapType;
    Status m_status;
    QTimer m_retryTimer;
};

QT_END_NAMESPACE

#endif // QTILEPROVIDEROSM_H
