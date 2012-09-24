/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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

#include "icon.h"
#include "iconhandler.h"

#include <QtCore/QBuffer>
#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtGui/QImageReader>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

const QSize JsonDbIcon::SmallSize(QSize(20,20));
const QSize JsonDbIcon::MediumSize(QSize(30,30));
const QSize JsonDbIcon::LargeSize(QSize(50,50));
const QSize JsonDbIcon::FullscreenSize(QSize(320,480));

const QLatin1String JsonDbIcon::SmallSource("smallSourceUrl");
const QLatin1String JsonDbIcon::MediumSource("mediumSourceUrl");
const QLatin1String JsonDbIcon::LargeSource("largeSourceUrl");
const QLatin1String JsonDbIcon::FullscreenSource("fullscreenSourceUrl");

const QLatin1String JsonDbIcon::SmallDestination("smallUrl");
const QLatin1String JsonDbIcon::MediumDestination("mediumUrl");
const QLatin1String JsonDbIcon::LargeDestination("largeUrl");
const QLatin1String JsonDbIcon::FullscreenDestination("fullscreenUrl");

const QLatin1String JsonDbIcon::SmallDestinationSize("smallSize");
const QLatin1String JsonDbIcon::MediumDestinationSize("mediumSize");
const QLatin1String JsonDbIcon::LargeDestinationSize("largeSize");
const QLatin1String JsonDbIcon::FullscreenDestinationSize("fullscreenSize");

const QLatin1String JsonDbIcon::NokiaIcon("nokiaIcon");
const QLatin1String JsonDbIcon::NokiaIconGenerated("nokiaIconGenerated");

const QLatin1String JsonDbIcon::DefaultIcon("/icons/categories/06.icon");

JsonDbIcon::JsonDbIcon(IconHandler *parent)
    : QObject(parent), m_iconHandler(parent), m_error(QPlaceReply::NoError)
{
    Q_ASSERT(parent);
}

JsonDbIcon::~JsonDbIcon()
{
}

//initialization in the case of having a source icon url involves fetching the icon
//data and determining its size
//initialization in the case of having a destination icon url only involves _attempting_
//to fetch it in order to calculate its size.  If this cannot be done, the
//user specified size is used.
void JsonDbIcon::initialize()
{
    QNetworkAccessManager *netManager = m_iconHandler->networkAccessManager();
    QNetworkRequest request;

    if (m_sourceUrl.isEmpty() && !m_destinationUrl.isEmpty())
        request.setUrl(m_destinationUrl);
    else
        request.setUrl(m_sourceUrl);
    QNetworkReply *reply = netManager->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(iconFetchFinished()));
}

bool JsonDbIcon::copy() const
{
    QFile file(m_destinationUrl.toLocalFile());
    if (!file.open(QIODevice::WriteOnly)) {
        if (file.error() == QFile::PermissionsError) {
            m_error = QPlaceReply::PermissionsError;
            m_errorString = QString::fromLatin1("Insufficient permissions to write icon to ") + m_destinationUrl.toLocalFile();
        } else {
            m_error = QPlaceReply::UnknownError;
            m_errorString = QString::fromLatin1("Could not write icon to ") + m_destinationUrl.toLocalFile();
        }
        return false;
    }

    if (file.write(m_payload) == -1) {
        m_error = QPlaceReply::UnknownError;
        m_errorString = QString::fromLatin1("Unable to save to icon to ") + m_destinationUrl.toLocalFile();

        return false;
    }

    return true;
}

QSize JsonDbIcon::size() const
{
    return m_size;
}

QSize JsonDbIcon::specifiedSize() const
{
    return m_specifiedSize;
}

void JsonDbIcon::setSpecifiedSize(const QSize &size)
{
    m_specifiedSize = size;
}

QUrl JsonDbIcon::sourceUrl() const
{
    return m_sourceUrl;
}

void JsonDbIcon::setSourceUrl(const QUrl &url)
{
    m_sourceUrl = url;
}

QUrl JsonDbIcon::destinationUrl() const
{
    return m_destinationUrl;
}

void JsonDbIcon::setDestinationUrl(const QUrl &url)
{
    m_destinationUrl = url;
}

void JsonDbIcon::setDestinationDataUrl()
{
    QString mimeType = imageFormatToMimeType(m_inputFormat);
    m_destinationUrl = QUrl(QString::fromLatin1("data:") + mimeType + QLatin1String(";base64,") + m_payload.toBase64());
}

QString JsonDbIcon::destination() const
{
    return m_destination;
}

void JsonDbIcon::setDestination(const QString &destination)
{
    m_destination = destination;
}

QPlaceReply::Error JsonDbIcon::error() const
{
    return m_error;
}

QString JsonDbIcon::errorString() const
{
    return m_errorString;
}

void JsonDbIcon::iconFetchFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());

    if (reply->error() == QNetworkReply::NoError) {
        m_payload = reply->readAll();
        m_size = QImage::fromData(m_payload).size();
        QBuffer buffer(&m_payload);
        buffer.open(QIODevice::ReadOnly);
        m_inputFormat = QImageReader::imageFormat(&buffer);

        if (m_inputFormat.isEmpty() && !m_sourceUrl.isEmpty()) {
            m_error = QPlaceReply::UnsupportedError;
            m_errorString = QString::fromLatin1("Format of source icon file could not be determined, url:") + m_sourceUrl.toString();
        }
    } else {
        m_error = QPlaceReply::CommunicationError;
        m_errorString = reply->errorString();
    }

    //we've tried to retrieve the destination url to try automatically calculate its size
    // this was not possible so set the size to the user specified size.
    if (m_sourceUrl.isEmpty() && !m_destinationUrl.isEmpty()) {
        if (m_specifiedSize == QSize() && m_size == QSize()) {
            m_error = QPlaceReply::BadArgumentError;
            m_errorString = QLatin1String("Cannot obtain size of destination icon url");
        } else {
            if (m_size.isEmpty())
                m_size = m_specifiedSize;
            m_error = QPlaceReply::NoError; //reset error back to empty because the inability
            m_errorString = QString();      //to access the destination url is not an error.
        }
    }

    reply->deleteLater();
    reply = 0;

    emit initializationFinished();
}

QString JsonDbIcon::imageFormatToMimeType(const QByteArray &format)
{
    if (format == "bmp")
        return QLatin1String("image/bmp");
    else if (format == "gif")
        return QLatin1String("image/gif");
    else if (format == "jpg" || format == "jpeg")
        return QLatin1String("image/jpeg");
    else if (format == "mng")
        return QLatin1String("video/x-mng");
    else if (format == "png")
        return QLatin1String("image/png");
    else if (format == "pbm")
        return QLatin1String("image/x-portable-bitmap");
    else if (format == "pgm")
        return QLatin1String("image/x-portable-graymap");
    else if (format == "ppm")
        return QLatin1String("image/x-portable-pixmap");
    else if (format == "tiff")
        return QLatin1String("image/tiff");
    else if (format == "xbm")
        return QLatin1String("image/x-xbitmap");
    else if (format == "xpm")
        return QLatin1String("image/x-xpixmap");
    else if (format == "svg")
        return QLatin1String("image/svg+xml");
    else
        return QString();
}
