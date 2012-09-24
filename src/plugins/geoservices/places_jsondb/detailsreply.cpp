/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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

#include "detailsreply.h"
#include "qplacemanagerengine_jsondb.h"

DetailsReply::DetailsReply(QPlaceManagerEngineJsonDb *engine)
    : QPlaceDetailsReply(engine), m_engine(engine)
{
}

DetailsReply::~DetailsReply()
{
}

void DetailsReply::setPlaceId(const QString &placeId)
{
    m_placeId = placeId;
}

void DetailsReply::start()
{
    db()->getPlace(m_placeId, this, SLOT(getPlaceFinished()));
}

void DetailsReply::getPlaceFinished()
{
    QJsonDbRequest *request = qobject_cast<QJsonDbRequest *>(sender());
    Q_ASSERT(request);
    QList<QJsonObject> results = request->takeResults();

    if (results.isEmpty()) {
        triggerDone(QPlaceReply::PlaceDoesNotExistError,
                                  QStringLiteral("Specified place does not exist"));
        return;

    } else {
        QJsonObject placeJson = results.takeFirst();
        setPlace(JsonDb::convertJsonObjectToPlace(placeJson, m_engine));

        QStringList categoryUuids = placeJson.value(JsonDb::CategoryUuids).toVariant().toStringList();
        if (!categoryUuids.isEmpty())
            db()->getCategories(categoryUuids, this, SLOT(getCategoriesForPlaceFinished()));
        else //don't need to retrieve categores so return the details
            triggerDone();
    }
}

void DetailsReply::getCategoriesForPlaceFinished()
{
    QJsonDbRequest *request = qobject_cast<QJsonDbRequest *>(sender());
    Q_ASSERT(request);
    QList<QJsonObject> results = request->takeResults();

    QPlace p = place();
    p.setCategories(JsonDb::convertJsonObjectsToCategories(results, m_engine));
    setPlace(p);
    triggerDone();
}

void DetailsReply::requestError(QtJsonDb::QJsonDbRequest::ErrorCode dbCode, const QString &dbErrorString)
{
    QString errorString = QString::fromLatin1("Unknown error during details fetch operation: jsondb error code =%1, erroString=%2").
                  arg(dbCode).arg(dbErrorString);
    triggerDone(QPlaceReply::UnknownError, errorString);
}


