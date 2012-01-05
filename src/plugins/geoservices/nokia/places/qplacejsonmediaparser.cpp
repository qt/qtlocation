/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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
** This file is part of the Ovi services plugin for the Maps and
** Navigation API.  The use of these services, whether by use of the
** plugin or by other means, is governed by the terms and conditions
** described by the file OVI_SERVICES_TERMS_AND_CONDITIONS.txt in
** this package, located in the directory containing the Ovi services
** plugin source code.
**
****************************************************************************/

#include "qplacejsonmediaparser.h"

#include <QtCore/QUrl>
#include <QJSEngine>
#include <QJSValue>
#include <QJSValueIterator>

#include <qplacesupplier.h>
#include "qplacesuppliersrepository.h"

#include <QtCore/QDebug>

#if defined(QT_PLACE_LIBRARY)
    #include <QDebug>
#endif

static const char *media_objects_element = "images";
static const char *media_element = "image";
static const char *media_count = "totalNumberOfImages";
static const char *media_url = "url";
static const char *media_provider_id = "provider";
static const char *media_provider_name = "providerDisplayName";

QT_USE_NAMESPACE

QPlaceJSonMediaParser::QPlaceJSonMediaParser(QObject *parent) :
    QPlaceJSonParser(parent),
    allMedia(0)
{
}

QPlaceJSonMediaParser::~QPlaceJSonMediaParser()
{
}

QList<QPlaceImage> QPlaceJSonMediaParser::resultMedia()
{
    return media;
}

int QPlaceJSonMediaParser::allMediaCount()
{
    return allMedia;
}

QPlaceImage QPlaceJSonMediaParser::buildMediaObject(const QJSValue &media)
{
    QPlaceImage newMedia;
    QJSValue value = media.property(media_url);
    if (value.isValid() && !value.toString().isEmpty()) {
        newMedia.setUrl(QUrl::fromEncoded(value.toString().toAscii()));
        newMedia.setImageId(value.toString());
    }

    QString name, id, icon;
    value = media.property(media_provider_name);
    if (value.isValid() && !value.toString().isEmpty()) {
        name = value.toString();
    }
    value = media.property(media_provider_id);
    if (value.isValid() && !value.toString().isEmpty()) {
        id = value.toString();
    }
    if (!name.isEmpty() || !id.isEmpty()) {
        QPlaceSupplier sup;
        sup.setName(name);
        sup.setSupplierId(id);
        newMedia.setSupplier(QPlaceSuppliersRepository::instance()->addSupplier(sup));
    }

    return newMedia;
}

void QPlaceJSonMediaParser::processJSonData(const QJSValue &sv)
{
    media.clear();

    if (sv.isValid()) {
        QJSValue sv2 = sv.property(media_objects_element);
        if (sv2.isValid()) {
            processMedia(sv2);
            emit finished(NoError, QString());
        } else {
            emit finished(ParsingError, QString("JSON data are invalid"));
        }
    } else {
        emit finished(ParsingError, QString("JSON data are invalid"));
    }
}

void QPlaceJSonMediaParser::processMedia(const QJSValue &mediaElement)
{
    QJSValue value = mediaElement.property(media_element);
    if (value.isValid()) {
        if (value.isArray()) {
            QJSValueIterator it(value);
            while (it.hasNext()) {
                it.next();
                // array contains count as last element
                if (it.name() != "length") {
                    media.append(buildMediaObject(it.value()));
                }
            }
        } else {
            media.append(buildMediaObject(value));
        }
    }
    value = mediaElement.property(media_count);
    if (value.isValid()) {
        allMedia = value.toInt32();
    }
}
