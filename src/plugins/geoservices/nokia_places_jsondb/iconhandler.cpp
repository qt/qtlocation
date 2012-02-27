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

#include "iconhandler.h"
#include "idreply.h"

#include <QtCore/QUrl>
#include <QtCore/QVariantMap>

IconHandler::IconHandler(const QPlaceIcon &inputIcon, const QJsonObject &thumbnailsJson,
                         IdReply *parent)
    : QObject(parent), m_thumbnailsJson(thumbnailsJson),
      m_error(QPlaceReply::NoError), m_currIconIndex(0), m_reply(parent)
{
    QStringList prefixes;
    prefixes << QStringLiteral("small") << QStringLiteral("medium") << QStringLiteral("large") << QStringLiteral("fullscreen");

    QList<QUrl> uniqueInputUrls; //unique source urls that have been supplied without associated destinations
    foreach (const QString &prefix, prefixes) {
        bool ok;
        QUrl sourceUrl = convertToUrl(inputIcon.parameters().value(prefix + QLatin1String("SourceUrl"), QUrl()), &ok);
        if (!ok) {
            triggerDone(QPlaceReply::BadArgumentError, QString::fromLatin1("icon parameter for key: ") + prefix + QLatin1String("SourceUrl")
                        + QLatin1String(" was not a QUrl object"));
            return;
        }
        QUrl destinationUrl = convertToUrl(inputIcon.parameters().value(prefix + QLatin1String("Url"), QUrl()), &ok);
        if (!ok) {
            triggerDone(QPlaceReply::BadArgumentError, QString::fromLatin1("icon parameter for key: ") + prefix + QLatin1String("Url")
                        + QLatin1String(" was not a QUrl object"));
            return;
        }

        if (destinationUrl.isEmpty()) {
            if (sourceUrl.isEmpty() || uniqueInputUrls.contains(sourceUrl))
                continue;
            else
                uniqueInputUrls.append(sourceUrl);
        }

        QString destination;
        if (!destinationUrl.isEmpty())
            destination = prefix + QLatin1String("Url");

        Icon *icon = new Icon(this);
        icon->setSourceUrl(sourceUrl);
        icon->setDestinationUrl(destinationUrl);
        icon->setDestination(destination);
        if (inputIcon.parameters().contains(prefix + QLatin1String("Size")))
            icon->setSpecifiedSize(inputIcon.parameters().value(prefix + QLatin1String("Size")).toSize());

        m_icons.append(icon);
    }

    processIcons();
}

IconHandler::~IconHandler()
{
    qDeleteAll(m_icons);
}

QString IconHandler::errorString() const
{
    return m_errorString;
}

QPlaceReply::Error IconHandler::error() const
{
    return m_error;
}

QNetworkAccessManager *IconHandler::networkAccessManager()
{
    return m_reply->engine()->networkAccessManager();
}

void IconHandler::processIcons()
{
    if (Icon *senderIcon = qobject_cast<Icon *>(sender())) {
        if (senderIcon->error() != QPlaceReply::NoError) {
            triggerDone(senderIcon->error(), senderIcon->errorString());
            return;
        }
    }

    if (m_currIconIndex < m_icons.count()) {
        Icon *icon = m_icons.at(m_currIconIndex);
        connect(icon, SIGNAL(initializationFinished()), this, SLOT(processIcons()), Qt::QueuedConnection);
        icon->initialize();
        m_currIconIndex++;
        return;
    } else {
        bool error = false;

        //try to set destinations for icons which were not already set
        QStringList specifiedDestinations;
        foreach (Icon *icon, m_icons) {
            if (!icon->destination().isEmpty())
                specifiedDestinations.append(icon->destination());
        }

        //try to set small,medium and large destinations if they haven't already been explicitly specified
        //and there are icons with unspecified destinations. (essentially we are creating data urls if necessary)
        if (!specifiedDestinations.contains(Icon::SmallDestination))
            trySetDestination(Icon::SmallDestination);

        if (!specifiedDestinations.contains(Icon::MediumDestination))
            trySetDestination(Icon::MediumDestination);

        if (!specifiedDestinations.contains(Icon::LargeDestination))
            trySetDestination(Icon::LargeDestination);

        //Note that we don't try and set the destination for full screen thumbnails
        //since data urls are meant to be just for small images

        //if we have an existing place, we try to preserve existing properties
        //of the thumbnail objects, but we completely remove/replace the fields
        //that are relevant for the QtLocation API.
        m_thumbnailsJson.remove(JsonDb::Small);
        m_thumbnailsJson.remove(JsonDb::Medium);
        m_thumbnailsJson.remove(JsonDb::Large);
        m_thumbnailsJson.remove(JsonDb::Fullscreen);

        foreach (Icon *icon, m_icons) {
            QJsonObject thumbnailJson;
            if (icon->error() != QPlaceReply::NoError) {
                triggerDone(icon->error(), icon->errorString());
                error = true;
                break;
            }

            if (!icon->sourceUrl().isEmpty()
                    && icon->destinationUrl().scheme().compare(QLatin1String("file")) == 0) {
                if (!icon->copy()) {
                    triggerDone(icon->error(), icon->errorString());
                    error = true;
                    break;
                }
            }

            thumbnailJson.insert(JsonDb::Url, icon->destinationUrl().toString());
            if (icon->size().isValid()) {
                thumbnailJson.insert(JsonDb::Height, icon->size().height());
                thumbnailJson.insert(JsonDb::Width, icon->size().width());
            } else {
                //size of icon could not be calculated, therefore rely on manually specified size
                if (!icon->specifiedSize().isValid()) {
                    triggerDone(QPlaceReply::BadArgumentError, QLatin1String("Size of icon could not be generated nor was it validly specified"));
                    error = true;
                    break;
                }
                thumbnailJson.insert(JsonDb::Height, icon->specifiedSize().height());
                thumbnailJson.insert(JsonDb::Width, icon->specifiedSize().width());
            }

            if (icon->destination() == Icon::SmallDestination)
                m_thumbnailsJson.insert(JsonDb::Small, thumbnailJson);
            else if (icon->destination() == Icon::MediumDestination)
                m_thumbnailsJson.insert(JsonDb::Medium, thumbnailJson);
            else if (icon->destination() == Icon::LargeDestination)
                m_thumbnailsJson.insert(JsonDb::Large, thumbnailJson);
            else
                m_thumbnailsJson.insert(JsonDb::Fullscreen, thumbnailJson);
        }

        if (!error) {
            QMetaObject::invokeMethod(this, "finished", Qt::QueuedConnection,
                                      Q_ARG(QJsonObject,m_thumbnailsJson));
        }

        qDeleteAll(m_icons);
        m_icons.clear();
    }
}

void IconHandler::trySetDestination(const QString &destination)
{
    static int threshold;
    int height;

    //assumption is that icons are squarish
    //so we can rely on height as a means to detect which size bucket
    //the icon belongs to
    if (destination == Icon::SmallDestination) {
        threshold = (Icon::SmallSize.height() + Icon::MediumSize.height()) / 2;
        height = Icon::SmallSize.height();
    } else if (destination == Icon::MediumDestination) {
        threshold = (Icon::MediumSize.height() + Icon::LargeSize.height()) / 2;
        height = Icon::MediumSize.height();
    } else if (destination == Icon::LargeDestination) {
        threshold = Icon::LargeSize.height() * 2;
        height = Icon::LargeSize.height();
    } //note fullscreen thumbnails should not be set as data urls.

    Icon *currIcon = 0;
    foreach (Icon *icon, m_icons) {
        if (icon->destination().isEmpty()
                && icon->size().height() < threshold
                && (currIcon == 0 ||  qAbs(icon->size().height() - height) < qAbs(currIcon->size().height() - height))) {
            if (currIcon)
                currIcon->setDestination(QString());
            currIcon = icon;
            currIcon->setDestination(destination);
        }
    }

    if (currIcon)
        currIcon->setDestinationDataUrl();
}

void IconHandler::triggerDone(const QPlaceReply::Error error, const QString &errorString)
{
    m_error = error;
    m_errorString = errorString;
    QMetaObject::invokeMethod(this, "finished", Qt::QueuedConnection,
                              Q_ARG(QJsonObject,m_thumbnailsJson));
}

QUrl IconHandler::convertToUrl(const QVariant &var, bool *ok)
{
    if (ok)
        *ok = false;

    switch (var.type()) {
    case (QVariant::Url): {
        if (ok)
            *ok = true;
        QUrl url = var.toUrl();
        if (url.scheme().isEmpty())
            return QUrl::fromUserInput(url.toString());
        else
            return url;
        break;
    }
    default:
        break;
    }
    return QUrl();
}
