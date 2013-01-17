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

#ifndef TRAVERSER_H
#define TRAVERSER_H

#include "qplacemanagerengine_jsondb.h"

#include <QtCore/QHash>
#include <QtCore/QJsonObject>
#include <QtCore/QStringList>
#include <QtJsonDb/QJsonDbRequest>

QT_BEGIN_NAMESPACE

class JsonDb;

class CategoryTraverser : public QObject
{
    Q_OBJECT
public:
    CategoryTraverser(JsonDb *db, QObject *parent = 0);
    void start(const QString &rootId = QString());
    QList<QJsonObject> results() { return m_catObjs; }

    QString errorString() { return m_errorString; }

    static CategoryTree convertToTree(const QList<QJsonObject> &catObjects,
                                      const QPlaceManagerEngineJsonDb *engine);

Q_SIGNALS:
    void finished();

private slots:
    void rootCategoryFinished();
    void requestFinished();
    void requestError(QtJsonDb::QJsonDbRequest::ErrorCode dbCode,
                      const QString &dbErrorString);

private:
    void processQueue();
    QList<QJsonObject> m_catObjs;
    JsonDb *m_db;
    QString m_rootId;
    QStringList m_catIdQueue;
    QString m_errorString;
};

QT_END_NAMESPACE

#endif
