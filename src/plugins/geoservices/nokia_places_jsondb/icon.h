/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef ICON_H
#define ICON_H

#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtGui/QImage>
#include <QtLocation/QPlaceReply>

class Icon : public QObject
{
    Q_OBJECT
public:

    enum ErrorCode {
        NoError,
        Error
    };

    static const QSize SmallSize;
    static const QSize MediumSize;
    static const QSize LargeSize;
    static const QSize FullscreenSize;

    static const QLatin1String SmallSource;
    static const QLatin1String MediumSource;
    static const QLatin1String LargeSource;
    static const QLatin1String FullscreenSource;

    static const QLatin1String SmallDestination;
    static const QLatin1String MediumDestination;
    static const QLatin1String LargeDestination;
    static const QLatin1String FullscreenDestination;

    static const QLatin1String SmallDestinationSize;
    static const QLatin1String MediumDestinationSize;
    static const QLatin1String LargeDestinationSize;
    static const QLatin1String FullscreenDestinationSize;

    Icon(const QUrl &sourceUrl, const QUrl &destinationUrl, const QString &destination);

    void initialize();

    bool copy() const;

    QSize size() const;

    QSize specifiedSize() const;
    void setSpecifiedSize(const QSize &size);

    QUrl sourceUrl() const;

    QUrl destinationUrl() const;
    void setDestinationDataUrl();

    QString destination() const;
    void setDestination(const QString &destination);

    QPlaceReply::Error error() const;
    QString errorString() const;

Q_SIGNALS:
    void initializationFinished();

private:
    bool initImage(const QUrl &url);
    static QString imageFormatToMimeType(const QByteArray &format);

    QByteArray m_inputFormat;
    QUrl m_sourceUrl;
    QUrl m_destinationUrl;
    QString m_destination;
    QSize m_specifiedSize;
    QSize m_size;
    QByteArray m_payload;

    mutable QPlaceReply::Error m_error;
    mutable QString m_errorString;
};

#endif
