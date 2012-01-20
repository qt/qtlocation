/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: http://www.qt-project.org/
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

class PlaceReply : public QPlaceReply
{
    Q_OBJECT

    friend class QPlaceManagerEngineTest;

public:
    PlaceReply(QObject *parent = 0)
    :   QPlaceReply(parent)
    { }

    Q_INVOKABLE void emitFinished()
    {
        emit finished();
    }
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
        m_locales << QLocale();

        if (parameters.value(QLatin1String("initializeCategories"), false).toBool()) {
            QPlaceCategory accommodation;
            accommodation.setName(QLatin1String("Accommodation"));
            accommodation.setCategoryId(QUuid::createUuid().toString());
            m_categories.insert(accommodation.categoryId(), accommodation);
            m_childCategories[QString()].append(accommodation.categoryId());

            QPlaceCategory hotel;
            hotel.setName(QLatin1String("Hotel"));
            hotel.setCategoryId(QUuid::createUuid().toString());
            m_categories.insert(hotel.categoryId(), hotel);
            m_childCategories[accommodation.categoryId()].append(hotel.categoryId());

            QPlaceCategory motel;
            motel.setName(QLatin1String("Motel"));
            motel.setCategoryId(QUuid::createUuid().toString());
            m_categories.insert(motel.categoryId(), motel);
            m_childCategories[accommodation.categoryId()].append(motel.categoryId());

            QPlaceCategory camping;
            camping.setName(QLatin1String("Camping"));
            camping.setCategoryId(QUuid::createUuid().toString());
            m_categories.insert(camping.categoryId(), camping);
            m_childCategories[accommodation.categoryId()].append(camping.categoryId());

            QPlaceCategory park;
            park.setName(QLatin1String("Park"));
            park.setCategoryId(QUuid::createUuid().toString());
            m_categories.insert(park.categoryId(), park);
            m_childCategories[QString()].append(park.categoryId());
        }
    }

    QPlaceDetailsReply *getPlaceDetails(const QString &placeId)
    {
        DetailsReply *reply = new DetailsReply(this);

        if (placeId.isEmpty() || !m_places.contains(placeId)) {
            reply->setError(QPlaceReply::PlaceDoesNotExistError, tr("Place does not exist"));
            QMetaObject::invokeMethod(reply, "emitError", Qt::QueuedConnection);
        } else {
            reply->setPlace(m_places.value(placeId));
        }

        QMetaObject::invokeMethod(reply, "emitFinished", Qt::QueuedConnection);

        return reply;
    }

    QPlaceContentReply *getPlaceContent(const QString &placeId, const QPlaceContentRequest &query)
    {
        Q_UNUSED(placeId)
        Q_UNUSED(query)

        return 0;
    }

    QPlaceSearchReply *search(const QPlaceSearchRequest &query)
    {
        Q_UNUSED(query)

        return 0;
    }

    QPlaceSearchReply *recommendations(const QString &placeId, const QPlaceSearchRequest &query)
    {
        Q_UNUSED(placeId)
        Q_UNUSED(query)

        return 0;
    }

    QPlaceSearchSuggestionReply *searchSuggestions(const QPlaceSearchRequest &query)
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

    QPlaceIdReply *removePlace(const QString &placeId)
    {
        IdReply *reply = new IdReply(QPlaceIdReply::RemovePlace, this);
        reply->setId(placeId);

        if (!m_places.contains(placeId)) {
            reply->setError(QPlaceReply::PlaceDoesNotExistError, tr("Place does not exist"));
            QMetaObject::invokeMethod(reply, "emitError", Qt::QueuedConnection);
        } else {
            m_places.remove(placeId);
        }

        QMetaObject::invokeMethod(reply, "emitFinished", Qt::QueuedConnection);

        return reply;
    }

    QPlaceIdReply *saveCategory(const QPlaceCategory &category, const QString &parentId)
    {
        IdReply *reply = new IdReply(QPlaceIdReply::SaveCategory, this);

        if ((!category.categoryId().isEmpty() && !m_categories.contains(category.categoryId())) ||
            (!parentId.isEmpty() && !m_categories.contains(parentId))) {
            reply->setError(QPlaceReply::CategoryDoesNotExistError, tr("Category does not exist"));
            QMetaObject::invokeMethod(reply, "emitError", Qt::QueuedConnection);
        } else if (!category.categoryId().isEmpty()) {
            m_categories.insert(category.categoryId(), category);
            QStringList children = m_childCategories.value(parentId);

            QMutableHashIterator<QString, QStringList> i(m_childCategories);
            while (i.hasNext()) {
                i.next();
                i.value().removeAll(category.categoryId());
            }

            if (!children.contains(category.categoryId())) {
                children.append(category.categoryId());
                m_childCategories.insert(parentId, children);
            }
            reply->setId(category.categoryId());
        } else {
            QPlaceCategory c = category;
            c.setCategoryId(QUuid::createUuid().toString());
            m_categories.insert(c.categoryId(), c);
            QStringList children = m_childCategories.value(parentId);
            if (!children.contains(c.categoryId())) {
                children.append(c.categoryId());
                m_childCategories.insert(parentId, children);
            }

            reply->setId(c.categoryId());
        }

        QMetaObject::invokeMethod(reply, "emitFinished", Qt::QueuedConnection);

        return reply;
    }

    QPlaceIdReply *removeCategory(const QString &categoryId)
    {
        IdReply *reply = new IdReply(QPlaceIdReply::RemoveCategory, this);
        reply->setId(categoryId);

        if (!m_categories.contains(categoryId)) {
            reply->setError(QPlaceReply::CategoryDoesNotExistError, tr("Category does not exist"));
            QMetaObject::invokeMethod(reply, "emitError", Qt::QueuedConnection);
        } else {
            m_categories.remove(categoryId);

            QMutableHashIterator<QString, QStringList> i(m_childCategories);
            while (i.hasNext()) {
                i.next();
                i.value().removeAll(categoryId);
            }
        }

        QMetaObject::invokeMethod(reply, "emitFinished", Qt::QueuedConnection);

        return reply;
    }

    QPlaceReply *initializeCategories()
    {
        QPlaceReply *reply = new PlaceReply(this);

        QMetaObject::invokeMethod(reply, "emitFinished", Qt::QueuedConnection);

        return reply;
    }

    QString parentCategoryId(const QString &categoryId) const
    {
        QHashIterator<QString, QStringList> i(m_childCategories);
        while (i.hasNext()) {
            i.next();
            if (i.value().contains(categoryId))
                return i.key();
        }

        return QString();
    }

    virtual QStringList childrenCategoryIds(const QString &categoryId) const
    {
        return m_childCategories.value(categoryId);
    }

    virtual QPlaceCategory category(const QString &categoryId) const
    {
        return m_categories.value(categoryId);
    }

    QList<QPlaceCategory> childCategories(const QString &parentId) const
    {
        QList<QPlaceCategory> categories;

        foreach (const QString &id, m_childCategories.value(parentId))
            categories.append(m_categories.value(id));

        return categories;
    }

    QList<QLocale> locales() const
    {
        return m_locales;
    }

    void setLocales(const QList<QLocale> &locales)
    {
        m_locales = locales;
    }

    QUrl constructIconUrl(const QPlaceIcon &icon, const QSize &size, QPlaceIcon::IconFlags flags)
    {
        Q_UNUSED(flags)

        const QString url = icon.baseUrl().toString() + QLatin1String("_%1x%2.png");
        return url.arg(size.width()).arg(size.height());
    }

    QPlaceManager::ManagerFeatures supportedFeatures() const {
        return QPlaceManager::SavePlaceFeature
               | QPlaceManager::SaveCategoryFeature
               | QPlaceManager::SearchSuggestionsFeature;
    }

    QPlace compatiblePlace(const QPlace &original) const {
        QPlace place;
        place.setName(original.name());
        return place;
    }

private:
    QList<QLocale> m_locales;
    QHash<QString, QPlace> m_places;
    QHash<QString, QPlaceCategory> m_categories;
    QHash<QString, QStringList> m_childCategories;
};

#endif
