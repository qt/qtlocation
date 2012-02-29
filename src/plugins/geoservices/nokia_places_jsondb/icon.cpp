/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "icon.h"

#include <QtCore/QBuffer>
#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtGui/QImageReader>

const QSize Icon::SmallSize(QSize(20,20));
const QSize Icon::MediumSize(QSize(30,30));
const QSize Icon::LargeSize(QSize(50,50));
const QSize Icon::FullscreenSize(QSize(320,480));

const QLatin1String Icon::SmallSource("smallSourceUrl");
const QLatin1String Icon::MediumSource("mediumSourceUrl");
const QLatin1String Icon::LargeSource("largeSourceUrl");
const QLatin1String Icon::FullscreenSource("fullscreenSourceUrl");

const QLatin1String Icon::SmallDestination("smallUrl");
const QLatin1String Icon::MediumDestination("mediumUrl");
const QLatin1String Icon::LargeDestination("largeUrl");
const QLatin1String Icon::FullscreenDestination("fullscreenUrl");

const QLatin1String Icon::SmallDestinationSize("smallSize");
const QLatin1String Icon::MediumDestinationSize("mediumSize");
const QLatin1String Icon::LargeDestinationSize("largeSize");
const QLatin1String Icon::FullscreenDestinationSize("fullscreenSize");

Icon::Icon(const QUrl &sourceUrl, const QUrl &destinationUrl, const QString &destination)
    : m_sourceUrl(sourceUrl), m_destinationUrl(destinationUrl),
      m_destination(destination), m_error(QPlaceReply::NoError)
{
}

void Icon::initialize()
{
    if (m_sourceUrl.scheme().compare(QLatin1String("file"), Qt::CaseInsensitive) == 0) {
        initImage(m_sourceUrl);
    } else if (m_sourceUrl.scheme().compare(QLatin1String("data"), Qt::CaseInsensitive) == 0) {
        initImage(m_sourceUrl);
    } else if (m_sourceUrl.isEmpty() && !m_destinationUrl.isEmpty()) {
        //see if we can get the image and hence size of the icon directly from the destinationUrl
        //if we can't then just set the size to the user specified size.
        if (!initImage(m_destinationUrl)) {
            if (m_specifiedSize == QSize()) {
                m_error = QPlaceReply::BadArgumentError;
                m_errorString = QLatin1String("Cannot obtain size of destination icon url");
            } else {
                m_error = QPlaceReply::NoError; //reset error back to empty because the inability
                m_errorString = QString();      //to access the destination url is not an error.

                m_size = m_specifiedSize;
            }
        }
    } else {
        m_error = QPlaceReply::BadArgumentError;
        m_errorString = QStringLiteral("Unrecognised source icon URL scheme:") + m_sourceUrl.toString();
    }

    emit initializationFinished();
}

bool Icon::copy() const
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

QSize Icon::size() const
{
    return m_size;
}

QSize Icon::specifiedSize() const
{
    return m_specifiedSize;
}

void Icon::setSpecifiedSize(const QSize &size)
{
    m_specifiedSize = size;
}

QUrl Icon::sourceUrl() const
{
    return m_sourceUrl;
}

QUrl Icon::destinationUrl() const
{
    return m_destinationUrl;
}

void Icon::setDestinationDataUrl()
{
    QString mimeType = imageFormatToMimeType(m_inputFormat);
    m_destinationUrl = QUrl(QString::fromLatin1("data:") + mimeType + QLatin1String(";base64,") + m_payload.toBase64());
}

QString Icon::destination() const
{
    return m_destination;
}

void Icon::setDestination(const QString &destination)
{
    m_destination = destination;
}

QPlaceReply::Error Icon::error() const
{
    return m_error;
}

QString Icon::errorString() const
{
    return m_errorString;
}

bool Icon::initImage(const QUrl &url)
{
    if (url.scheme().compare(QLatin1String("file"), Qt::CaseInsensitive) == 0) {
        QString fileName = url.toLocalFile();
        if (!fileName.isEmpty() && QFile::exists(fileName)) {
            QFile file(fileName);
            file.open(QIODevice::ReadOnly);
            m_payload = file.readAll();
            file.close();

            QImageReader imageReader(fileName);
            m_inputFormat = imageReader.format();

            if (m_inputFormat.isEmpty()) {
                m_error = QPlaceReply::UnsupportedError;
                m_errorString = QStringLiteral("Format of input file could not be detected");
                return false;
            }

            m_size = imageReader.size();
            return true;
        } else {
            m_error = QPlaceReply::BadArgumentError;
            m_errorString = QString::fromLatin1("Icon file does not exist:") +  url.toString();
            return false;
        }
    } else if (url.scheme().compare(QLatin1String("data"), Qt::CaseInsensitive) == 0) {
        QByteArray data = QByteArray::fromPercentEncoding(url.toEncoded());
        data.remove(0,5);
        int pos = data.indexOf(',');
        if (pos != -1) {
            m_payload = QByteArray::fromBase64(data.mid(pos + 1));
            data.truncate(pos);

            if (!data.endsWith(";base64")) {
                m_error = QPlaceReply::BadArgumentError;
                m_errorString = QStringLiteral("Icon data urls must be base 64 encoded");
                return false;
            }

            m_size = QImage::fromData(m_payload).size();
            QBuffer buffer(&m_payload);
            buffer.open(QIODevice::ReadOnly);
            m_inputFormat = QImageReader::imageFormat(&buffer);
            return true;
        } else {
            m_error = QPlaceReply::BadArgumentError;
            m_errorString = "Could not parse icon data url";
            return false;
        }
    } else {
        m_error = QPlaceReply::BadArgumentError;
        m_errorString = "Unsupported url scheme";
        return false;
    }
}

QString Icon::imageFormatToMimeType(const QByteArray &format)
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
