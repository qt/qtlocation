/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
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

#ifndef ICON_H
#define ICON_H

#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtGui/QImage>
#include <QtLocation/QPlaceReply>

QT_BEGIN_NAMESPACE

class IconHandler;

class JsonDbIcon : public QObject
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

    static const QLatin1String NokiaIcon;
    static const QLatin1String NokiaIconGenerated;

    static const QLatin1String DefaultIcon;

    JsonDbIcon(IconHandler *parent);
    ~JsonDbIcon();

    void initialize();

    bool copy() const;

    QSize size() const;

    QSize specifiedSize() const;
    void setSpecifiedSize(const QSize &size);

    QUrl sourceUrl() const;
    void setSourceUrl(const QUrl &url);

    QUrl destinationUrl() const;
    void setDestinationUrl(const QUrl &url);
    void setDestinationDataUrl();

    QString destination() const;
    void setDestination(const QString &destination);

    QPlaceReply::Error error() const;
    QString errorString() const;

Q_SIGNALS:
    void initializationFinished();

private slots:
    void iconFetchFinished();

private:
    static QString imageFormatToMimeType(const QByteArray &format);

    QByteArray m_inputFormat;
    QUrl m_sourceUrl;
    QUrl m_destinationUrl;
    QString m_destination;
    QSize m_specifiedSize;
    QSize m_size;
    QByteArray m_payload;
    IconHandler *m_iconHandler;

    mutable QPlaceReply::Error m_error;
    mutable QString m_errorString;
};

QT_END_NAMESPACE

#endif
