/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QLANDMARKREQUESTS_P_H
#define QLANDMARKREQUESTS_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qlandmarkabstractrequest_p.h"
#include "qlandmark.h"
#include "qlandmarkcategory.h"
#include "qlandmarkcategoryfetchrequest.h"
#include "qlandmarknamesort.h"

QTM_BEGIN_NAMESPACE

class QLandmarkIdFetchRequestPrivate : public QLandmarkAbstractRequestPrivate
{
public:
    QLandmarkIdFetchRequestPrivate(QLandmarkManager *mgr)
        : QLandmarkAbstractRequestPrivate(mgr),
          limit(-1), offset(0)
    {
        type = QLandmarkAbstractRequest::LandmarkIdFetchRequest;
    }

    QLandmarkFilter filter;
    QList<QLandmarkSortOrder>sorting;
    int limit;
    int offset;
    QList<QLandmarkId> landmarkIds;
};

class QLandmarkFetchRequestPrivate : public QLandmarkAbstractRequestPrivate
{
public:
    QLandmarkFetchRequestPrivate(QLandmarkManager *mgr)
        : QLandmarkAbstractRequestPrivate(mgr),
        limit(-1), offset(0)
    {
        type = QLandmarkAbstractRequest::LandmarkFetchRequest;
    }
    QLandmarkFilter filter;
    QList<QLandmarkSortOrder> sorting;
    int limit;
    int offset;
    QList<QLandmark> landmarks;
};

class QLandmarkFetchByIdRequestPrivate : public QLandmarkAbstractRequestPrivate
{
public:
    QLandmarkFetchByIdRequestPrivate(QLandmarkManager *mgr)
        : QLandmarkAbstractRequestPrivate(mgr)
    {
        type = QLandmarkAbstractRequest::LandmarkFetchByIdRequest;
    }

    QList<QLandmarkId> landmarkIds;
    QList<QLandmark> landmarks;
    QMap<int, QLandmarkManager::Error> errorMap;
};

class QLandmarkRemoveRequestPrivate : public QLandmarkAbstractRequestPrivate
{
public:
    QLandmarkRemoveRequestPrivate(QLandmarkManager *manager)
        : QLandmarkAbstractRequestPrivate(manager)
    {
        type = QLandmarkAbstractRequest::LandmarkRemoveRequest;
    }

    QList<QLandmarkId> landmarkIds;
    QMap<int, QLandmarkManager::Error> errorMap;
};


class QLandmarkSaveRequestPrivate : public QLandmarkAbstractRequestPrivate
{
public:
    QLandmarkSaveRequestPrivate(QLandmarkManager *mgr)
        : QLandmarkAbstractRequestPrivate(mgr)
    {
        type = QLandmarkAbstractRequest::LandmarkSaveRequest;
    }

    QList<QLandmark> landmarks;
    QMap<int, QLandmarkManager::Error> errorMap;
};

class QLandmarkCategoryIdFetchRequestPrivate: public QLandmarkAbstractRequestPrivate
{
public:
    QLandmarkCategoryIdFetchRequestPrivate(QLandmarkManager *mgr)
        : QLandmarkAbstractRequestPrivate(mgr),
          limit(-1), offset(0)
    {
        type = QLandmarkAbstractRequest::CategoryIdFetchRequest;
    }

    QList<QLandmarkCategoryId> categoryIds;
    QLandmarkNameSort sorting;
    int limit;
    int offset;
};

class QLandmarkCategoryFetchRequestPrivate : public QLandmarkAbstractRequestPrivate
{
public:
    QLandmarkCategoryFetchRequestPrivate(QLandmarkManager *mgr)
        :QLandmarkAbstractRequestPrivate(mgr),
         limit(-1), offset(0)
    {
        type = QLandmarkAbstractRequest::CategoryFetchRequest;
    }

    QList<QLandmarkCategory> categories;
    QLandmarkNameSort sorting;
    int limit;
    int offset;
};

class QLandmarkCategoryFetchByIdRequestPrivate : public QLandmarkAbstractRequestPrivate
{
public:
    QLandmarkCategoryFetchByIdRequestPrivate(QLandmarkManager *mgr)
        :QLandmarkAbstractRequestPrivate(mgr)
    {
        type = QLandmarkAbstractRequest::CategoryFetchByIdRequest;
    }

    QList<QLandmarkCategoryId> categoryIds;
    QList<QLandmarkCategory> categories;
    QMap<int, QLandmarkManager::Error> errorMap;
};

class QLandmarkCategoryRemoveRequestPrivate : public QLandmarkAbstractRequestPrivate
{
public:
    QLandmarkCategoryRemoveRequestPrivate(QLandmarkManager *mgr)
        : QLandmarkAbstractRequestPrivate(mgr)
    {
        type = QLandmarkAbstractRequest::CategoryRemoveRequest;
    }


    QList<QLandmarkCategoryId> categoryIds;
    QMap<int, QLandmarkManager::Error> errorMap;
};

class QLandmarkCategorySaveRequestPrivate : public QLandmarkAbstractRequestPrivate
{
public:
    QLandmarkCategorySaveRequestPrivate(QLandmarkManager *mgr)
        : QLandmarkAbstractRequestPrivate(mgr)
    {
        type = QLandmarkAbstractRequest::CategorySaveRequest;
    }

    QList<QLandmarkCategory> categories;
    QMap<int, QLandmarkManager::Error> errorMap;
};

class QLandmarkImportRequestPrivate : public QLandmarkAbstractRequestPrivate
{
public:
    QLandmarkImportRequestPrivate(QLandmarkManager *mgr)
        : QLandmarkAbstractRequestPrivate(mgr),
        device(0),
        fileName(QString()),
        format(QString()),
        option(QLandmarkManager::IncludeCategoryData),
        categoryId(QLandmarkCategoryId()),
        landmarkIds(QList<QLandmarkId>())
    {
        type = QLandmarkAbstractRequest::ImportRequest;
    }

    QIODevice *device;
    QString fileName;
    QString format;
    QLandmarkManager::TransferOption option;
    QLandmarkCategoryId categoryId;
    QList<QLandmarkId> landmarkIds;
};

class QLandmarkExportRequestPrivate : public QLandmarkAbstractRequestPrivate
{
public:
    QLandmarkExportRequestPrivate(QLandmarkManager *mgr)
        : QLandmarkAbstractRequestPrivate(mgr),
          device(0),
          fileName(QString()),
          format(QString()),
          option(QLandmarkManager::IncludeCategoryData),
          landmarkIds(QList<QLandmarkId>())
    {
        type = QLandmarkAbstractRequest::ExportRequest;
    }

    QIODevice *device;
    QString fileName;
    QString format;
    QLandmarkManager::TransferOption option;
    QList<QLandmarkId> landmarkIds;
};

QTM_END_NAMESPACE

#endif
