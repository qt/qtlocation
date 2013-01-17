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
#ifndef IDREPLY_H
#define IDREPLY_H

#include <QtCore/QObject>
#include <QtCore/QJsonArray>
#include <QtJsonDb/QJsonDbRequest>
#include <QtLocation/QPlaceIdReply>

#include "icon.h"
#include "macro.h"
#include "qplacemanagerengine_jsondb.h"
#include "iconhandler.h"

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

    QPlaceManagerEngineJsonDb *engine() { return m_engine; }

protected:
    JsonDb *db() { return m_engine->db(); }

private:
    QPlaceManagerEngineJsonDb *m_engine;
    bool m_isUpdate;
    QString m_parentCategoryId;
};

class SavePlaceReply : public IdReply
{
    Q_OBJECT

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

private slots:
    void checkIfExistsFinished();
    void getCategoriesForPlaceFinished();
    void processIconsFinished(const QJsonObject &thumbnailsJson);
    void savingFinished();

    void requestError(QtJsonDb::QJsonDbRequest::ErrorCode dbCode, const QString &dbErrorString);

private:
    QPlace m_place;
    QJsonObject m_placeJson;
    QStringList m_specifiedDestinations;
    IconHandler *m_iconHandler;
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
    void checkIfExistsFinished();
    void removeFinished();
    void requestError(QtJsonDb::QJsonDbRequest::ErrorCode dbCode, const QString &dbErrorString);
};

class SaveCategoryReply : public IdReply
{
    Q_OBJECT

public:
    SaveCategoryReply(QPlaceManagerEngineJsonDb *engine);
    virtual ~SaveCategoryReply();
    void setCategory(const QPlaceCategory &category);
    void setParentId(const QString &parentId);
    void start();

private slots:
    void checkParentExistsFinished();
    void getCurrentCategoryFinished();
    void processIconsFinished(const QJsonObject &thumbnailsJson);
    void savingFinished();

    void requestError(QtJsonDb::QJsonDbRequest::ErrorCode dbCode, const QString &dbErrorString);

private:
    QJsonObject prepareCategoryJson();
    void processIcons();

    QPlaceCategory m_category;
    QString m_parentId;
    QJsonArray m_newAncestors;
    QJsonArray m_oldAncestors;
    QJsonObject m_categoryJson;
    QList<QJsonObject> m_descendantsJson;
    IconHandler *m_iconHandler;
};

class RemoveCategoryReply : public IdReply
{
    Q_OBJECT

public:
    RemoveCategoryReply(QPlaceManagerEngineJsonDb *engine);
    virtual ~RemoveCategoryReply();
    void setId(const QString &categoryId);
    void start();

private slots:
    void getCategoryFinished();
    void getCategoriesToBeRemovedFinished();
    void removeFinished();
    void requestError(QtJsonDb::QJsonDbRequest::ErrorCode dbCode, const QString &dbErrorString);
};

QT_END_NAMESPACE

#endif
