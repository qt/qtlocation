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
** This file is part of the Nokia services plugin for the Maps and
** Navigation API.  The use of these services, whether by use of the
** plugin or by other means, is governed by the terms and conditions
** described by the file NOKIA_TERMS_AND_CONDITIONS.txt in
** this package, located in the directory containing the Nokia services
** plugin source code.
**
****************************************************************************/

#ifndef QPLACECATEGORIESREPOSITORY_H
#define QPLACECATEGORIESREPOSITORY_H

#include <QObject>
#include <QWeakPointer>
#include <QList>

#include <qplacecategory.h>
#include <qplacereply.h>
#include "qplacejsoncategoriesparser.h"
#include "qplacecategoriesreplyimplv1.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE



class QPlaceCategoriesRepository : public QObject
{
    Q_OBJECT
public:
    static QPlaceCategoriesRepository *instance();

    ~QPlaceCategoriesRepository();

    QPlaceReply *initializeCategories();
    QPlaceCategoryTree categories() const;

    QPlaceCategory mapCategory(const QString &number);
    QString getCategoryTagId(const QPlaceCategory &category);
    QPlaceCategory findCategoryById(const QString &id);

public slots:
    void replyFinished();

private:
    void setupCategoriesMapper();

private:
    QPlaceCategoriesRepository(QObject *parent = 0);

    QPlaceCategoryTree m_categoryTree;
    static QPlaceCategoriesRepository *repositoryInstance;
    QWeakPointer<QPlaceCategoriesReplyImplV1> m_categoriesReply;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QPLACECATEGORIESREPOSITORY_H
