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
#ifndef IDREPLY_H
#define IDREPLY_H

#include <QtCore/QObject>
#include <QtLocation/QPlaceIdReply>

#include "icon.h"
#include "macro.h"
#include "qplacemanagerengine_jsondb.h"

QT_BEGIN_NAMESPACE

class IdReply : public QPlaceIdReply
{
    Q_OBJECT
public:
    IdReply(QPlaceIdReply::OperationType operationType, QPlaceManagerEngineJsonDb *engine);
    virtual ~IdReply();
    void setId(const QString &id);

    DECLARE_TRIGGER_DONE_FN

    void setIsUpdate();
    bool isUpdate() const;

    QString parentCategoryId() const;
    void setParentCategoryId(const QString &parentId);

    virtual void start();

protected:
    QPlaceManagerEngineJsonDb *m_engine;
    JsonDbClient *db() { return m_engine->db();}

private:
    bool m_isUpdate;
    QString m_parentCategoryId;
};

class SavePlaceReply : public IdReply
{
    Q_OBJECT
    enum State {
        Initial,
        CheckIfExists,
        GetIcons,
        GetCategories,
        Saving
    };

    enum IconDestination {
        Small,
        Medium,
        Large,
        FullScreen
    };

public:
    SavePlaceReply(QPlaceManagerEngineJsonDb *engine);
    virtual ~SavePlaceReply();

    void setPlace(const QPlace &place);
    void start();
    void enterGetCategoriesState();
    void enterGetIconsState();

private slots:
    void processIcons();

    void processResponse(int id, const QVariant &data);
    void processError(int id, int code, const QString &errorString);

private:
    void trySetDestination(const QString &destination);
    QUrl convertToUrl(const QVariant &var, bool *ok);

private:
    QPlace m_place;
    int m_reqId;
    State m_state;
    QVariantMap m_placeMap;
    QList<Icon *> m_icons;
    int currIconIndex;
    QStringList m_specifiedDestinations;
};

class RemovePlaceReply : public IdReply
{
    Q_OBJECT
public:
    RemovePlaceReply(QPlaceManagerEngineJsonDb *engine);
    virtual ~RemovePlaceReply();
    void setId(const QString &placeId);
    void start();

private slots:
    void processResponse(int id, const QVariant &data);
    void processError(int id, int code, const QString &errorString);

private:
    int m_reqId;
};

class SaveCategoryReply : public IdReply
{
    Q_OBJECT
    enum State {
        Initial,
        CheckParentExists,
        GetCurrentCategory,
        GetParentDescendants,
        Saving
    };

public:
    SaveCategoryReply(QPlaceManagerEngineJsonDb *engine);
    virtual ~SaveCategoryReply();
    void setCategory(const QPlaceCategory &category);
    void setParentId(const QString &parentId);
    void start();

    void doSave();

private slots:
    void processResponse(int id, const QVariant &data);
    void processError(int id, int code, const QString &errorString);

private:
    QPlaceCategory m_category;
    QString m_parentId;
    int m_reqId;
    State m_state;
    QStringList m_newAncestors;
    QStringList m_oldAncestors;
};

class RemoveCategoryReply : public IdReply
{
    Q_OBJECT
    enum State {
        Initial,
        GetCategory,
        GetCategoriesToBeRemoved,
        RemoveCategories
    };

public:
    RemoveCategoryReply(QPlaceManagerEngineJsonDb *engine);
    virtual ~RemoveCategoryReply();
    void setId(const QString &categoryId);
    void start();

private slots:
    void processResponse(int id, const QVariant &data);
    void processError(int id, int code, const QString &errorString);

private:
    int m_reqId;
    State m_state;
};

QT_END_NAMESPACE

#endif
