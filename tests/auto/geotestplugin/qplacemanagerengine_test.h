/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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

#ifndef QPLACEMANAGERENGINE_TEST_H
#define QPLACEMANAGERENGINE_TEST_H

#include <QtCore/QUuid>

#include <qplacemanager.h>
#include <qplacemanagerengine.h>
#include <qplacereply.h>
#include <QtLocation/QPlaceDetailsReply>
#include <QtLocation/QPlaceIdReply>

#include <QtCore/QDebug>

QT_USE_NAMESPACE

class PlaceReplyTest : public QPlaceReply
{
    Q_OBJECT
public:
    PlaceReplyTest(QObject *parent = 0)
        : QPlaceReply(parent) {}

    void abort() { emit aborted(); }

Q_SIGNALS:
    void aborted();
};

class DetailsReply : public QPlaceDetailsReply
{
    Q_OBJECT

    friend class QPlaceManagerEngineTest;

public:
    DetailsReply(QObject *parent = 0)
    :   QPlaceDetailsReply(parent)
    { }

    Q_INVOKABLE void emitError()
    {
        emit error(error(), errorString());
    }

    Q_INVOKABLE void emitFinished()
    {
        emit finished();
    }
};

class IdReply : public QPlaceIdReply
{
    Q_OBJECT

    friend class QPlaceManagerEngineTest;

public:
    IdReply(QPlaceIdReply::OperationType type, QObject *parent = 0)
    :   QPlaceIdReply(type, parent)
    { }

    Q_INVOKABLE void emitError()
    {
        emit error(error(), errorString());
    }

    Q_INVOKABLE void emitFinished()
    {
        emit finished();
    }
};

class QPlaceManagerEngineTest : public QPlaceManagerEngine
{
    Q_OBJECT
public:
    QPlaceManagerEngineTest(const QMap<QString, QVariant> &parameters)
        : QPlaceManagerEngine(parameters)
    {
    }

    QPlaceDetailsReply *getPlaceDetails(const QString &placeId)
    {
        DetailsReply *reply = new DetailsReply(this);

        if (placeId.isEmpty() || !m_places.contains(placeId)) {
            reply->setError(QPlaceReply::PlaceDoesNotExistError, tr("Place does not exist"));
            QMetaObject::invokeMethod(reply, "emitError", Qt::QueuedConnection);
        } else {
            reply->setResult(m_places.value(placeId));
        }

        QMetaObject::invokeMethod(reply, "emitFinished", Qt::QueuedConnection);

        return reply;
    }

    QPlaceContentReply *getContent(const QPlace &place, const QPlaceContentRequest &query)
    {
        Q_UNUSED(place)
        Q_UNUSED(query)

        return 0;
    }

    QPlaceReply *postRating(const QString &placeId, qreal value)
    {
        Q_UNUSED(placeId)
        Q_UNUSED(value)

        return 0;
    }

    QPlaceSearchReply *searchForPlaces(const QPlaceSearchRequest &query)
    {
        Q_UNUSED(query)

        return 0;
    }

    QPlaceSearchReply *recommendations(const QPlace &place, const QPlaceSearchRequest &query)
    {
        Q_UNUSED(place)
        Q_UNUSED(query)

        return 0;
    }

    QPlaceTextPredictionReply *textPredictions(const QPlaceSearchRequest &query)
    {
        Q_UNUSED(query)

        return 0;
    }

    QPlaceIdReply *savePlace(const QPlace &place)
    {
        IdReply *reply = new IdReply(QPlaceIdReply::SavePlace, this);

        if (!place.placeId().isEmpty() && !m_places.contains(place.placeId())) {
            reply->setError(QPlaceReply::PlaceDoesNotExistError, tr("Place does not exist"));
            QMetaObject::invokeMethod(reply, "emitError", Qt::QueuedConnection);
        } else if (!place.placeId().isEmpty()) {
            m_places.insert(place.placeId(), place);
            reply->setId(place.placeId());
        } else {
            QPlace p = place;
            p.setPlaceId(QUuid::createUuid().toString());
            m_places.insert(p.placeId(), p);

            reply->setId(p.placeId());
        }

        QMetaObject::invokeMethod(reply, "emitFinished", Qt::QueuedConnection);

        return reply;
    }

    QPlaceIdReply *removePlace(const QPlace &place)
    {
        IdReply *reply = new IdReply(QPlaceIdReply::RemovePlace, this);
        reply->setId(place.placeId());

        if (!m_places.contains(place.placeId())) {
            reply->setError(QPlaceReply::PlaceDoesNotExistError, tr("Place does not exist"));
            QMetaObject::invokeMethod(reply, "emitError", Qt::QueuedConnection);
        } else {
            m_places.remove(place.placeId());
        }

        QMetaObject::invokeMethod(reply, "emitFinished", Qt::QueuedConnection);

        return reply;
    }

    QPlaceIdReply *saveCategory(const QPlaceCategory &category, const QString &parentId)
    {
        Q_UNUSED(category);
        Q_UNUSED(parentId);

        return 0;
    }

    QPlaceIdReply *removeCategory(const QString &categoryId)
    {
        Q_UNUSED(categoryId);

        return 0;
    }

    QPlaceReply *initializeCategories()
    {
        return 0;
    }

    QString parentCategoryId(const QString &categoryId) const
    {
        Q_UNUSED(categoryId)
        return QString();
    }

    virtual QStringList childrenCategoryIds(const QString &categoryId) const
    {
        Q_UNUSED(categoryId)
        return QStringList();
    }

    virtual QPlaceCategory category(const QString &categoryId) const {
        Q_UNUSED(categoryId)
        return QPlaceCategory();
    }

    QList<QPlaceCategory> childCategories(const QString &parentId) const {
        Q_UNUSED(parentId);
        return QList<QPlaceCategory>();
    }

    QLocale locale() const
    {
        return m_locale;
    }

    void setLocale(const QLocale &locale)
    {
        m_locale = locale;
    }

private:
    QLocale m_locale;
    QMap<QString, QPlace> m_places;
};

#endif
