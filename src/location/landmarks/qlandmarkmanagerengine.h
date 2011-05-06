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

#ifndef Q_LANDMARKMANAGERENGINE_H
#define Q_LANDMARKMANAGERENGINE_H
#include <QObject>
#include <QMap>
#include "qmobilityglobal.h"

#include "qlandmarkmanager.h"
#include "qlandmarkabstractrequest.h"


class QIODevice;

QTM_BEGIN_NAMESPACE
class QLandmarkCategory;
class QLandmarkCategoryId;
class QLandmark;
class QLandmarkId;
class QLandmarkSortOrder;
class QLandmarkIdFetchRequest;
class QLandmarkFetchRequest;
class QLandmarkFetchByIdRequest;
class QLandmarkRemoveRequest;
class QLandmarkSaveRequest;
class QLandmarkCategoryIdFetchRequest;
class QLandmarkCategoryFetchRequest;
class QLandmarkCategoryFetchByIdRequest;
class QLandmarkCategoryRemoveRequest;
class QLandmarkCategorySaveRequest;
class QLandmarkImportRequest;
class QLandmarkExportRequest;

class QLandmarkFilter;
class QLandmarkNameFilter;
class QLandmarkProximityFilter;
class QLandmarkCategoryFilter;
class QLandmarkBoxFilter;
class QLandmarkUnionFilter;
class QLandmarkIntersectionFilter;
class QLandmarkAttributeFilter;

class QLandmarkNameSort;
class QLandmarkDistanceSort;

class Q_LOCATION_EXPORT QLandmarkManagerEngine : public QObject
{
    Q_OBJECT
public:
    QLandmarkManagerEngine();
    virtual ~QLandmarkManagerEngine();

    /* URI reporting */
    virtual QString managerName() const = 0;                       // e.g. "com.nokia.qt.landmarks.engines.s60"
    virtual QMap<QString, QString> managerParameters() const = 0;  // e.g. "filename=private.db"
    virtual int managerVersion() const = 0;

    QString managerUri() const;

    /* Filtering */
    virtual QList<QLandmarkId> landmarkIds(const QLandmarkFilter &filter,  int limit, int offset, const QList<QLandmarkSortOrder>& sortOrders,
                                           QLandmarkManager::Error *error, QString *errorString) const = 0;
    virtual QList<QLandmarkCategoryId> categoryIds(int limit, int offset, const QLandmarkNameSort &nameSort, QLandmarkManager::Error *error,
            QString *errorString) const = 0;

    /* Retrieval */
    virtual QLandmark landmark(const QLandmarkId &landmarkId, QLandmarkManager::Error *error,
                               QString *errorString) const =0;
    virtual QList<QLandmark> landmarks(const QList<QLandmarkId> &landmarkIds,QMap<int, QLandmarkManager::Error> *errorMap,
                                    QLandmarkManager::Error *error, QString *errorString) const = 0;
    virtual QList<QLandmark> landmarks(const QLandmarkFilter &filter, int limit, int offset, const QList<QLandmarkSortOrder>& sortOrders,
                                       QLandmarkManager::Error *error, QString *errorString) const = 0;
    virtual QLandmarkCategory category(const QLandmarkCategoryId &landmarkCategoryId, QLandmarkManager::Error *error,
                                       QString *errorString) const = 0;
    virtual QList<QLandmarkCategory> categories(const QList<QLandmarkCategoryId> &landmarkCategoryIds, QMap<int, QLandmarkManager::Error> *errorMap,
                                                 QLandmarkManager::Error *error, QString *errorString) const = 0;
    virtual QList<QLandmarkCategory> categories(int limit, int offset, const QLandmarkNameSort &nameSort, QLandmarkManager::Error *error,
                                                QString *errorString) const = 0;

    /*saving and removing*/
    virtual bool saveLandmark(QLandmark* landmark, QLandmarkManager::Error *error, QString *errorString) = 0;
    virtual bool saveLandmarks(QList<QLandmark> * landmark, QMap<int, QLandmarkManager::Error> *errorMap,
                               QLandmarkManager::Error *error, QString *errorString) = 0;
    virtual bool removeLandmark(const QLandmarkId &landmarkId, QLandmarkManager::Error *error, QString *errorString) = 0;
    virtual bool removeLandmarks(const QList<QLandmarkId> &landmarkId, QMap<int, QLandmarkManager::Error> *errorMap, QLandmarkManager::Error *error,
                                 QString *errorString) = 0;

    virtual bool saveCategory(QLandmarkCategory* category, QLandmarkManager::Error *error, QString *errorString) = 0;
    virtual bool removeCategory(const QLandmarkCategoryId &categoryId, QLandmarkManager::Error *error, QString *errorString) = 0;

    virtual bool importLandmarks(QIODevice *device, const QString& format, QLandmarkManager::TransferOption option, const QLandmarkCategoryId& id,
                                QLandmarkManager::Error *error, QString *errorString) = 0;
    virtual bool exportLandmarks(QIODevice *device, const QString& format, const QList<QLandmarkId> &landmarkIds, QLandmarkManager::TransferOption option,
                                 QLandmarkManager::Error *error, QString *errorString) const = 0;
    virtual QStringList supportedFormats(QLandmarkManager::TransferOperation operation, QLandmarkManager::Error *error, QString *errorString) const;

    virtual QLandmarkManager::SupportLevel filterSupportLevel(const QLandmarkFilter &filter, QLandmarkManager::Error *error, QString *errorString) const = 0;
    virtual QLandmarkManager::SupportLevel sortOrderSupportLevel(const QLandmarkSortOrder &sortOrder, QLandmarkManager::Error *error, QString *errorString) const = 0;
    virtual bool isFeatureSupported(QLandmarkManager::ManagerFeature feature, QLandmarkManager::Error *error, QString *errorString) const = 0;

    virtual bool isReadOnly(QLandmarkManager::Error *error, QString *errorString) const = 0;
    virtual bool isReadOnly(const QLandmarkId &landmarkId, QLandmarkManager::Error *error, QString *errorString) const = 0;
    virtual bool isReadOnly(const QLandmarkCategoryId &categoryId, QLandmarkManager::Error *error, QString *errorString) const = 0;

    virtual QStringList searchableLandmarkAttributeKeys(QLandmarkManager::Error *error, QString *errorString) const = 0;

    /* Asynchronous Request Support */
    virtual void requestDestroyed(QLandmarkAbstractRequest* request) = 0;
    virtual bool startRequest(QLandmarkAbstractRequest* request) = 0;
    virtual bool cancelRequest(QLandmarkAbstractRequest* request) = 0;
    virtual bool waitForRequestFinished(QLandmarkAbstractRequest* request, int msecs) = 0;

Q_SIGNALS:
    void dataChanged();
    void landmarksAdded(const QList<QLandmarkId> &landmarkIds);
    void landmarksChanged(const QList<QLandmarkId> &landmarkIds);
    void landmarksRemoved(const QList<QLandmarkId> &landmarkIds);

    void categoriesAdded(const QList<QLandmarkCategoryId> &categoryIds);
    void categoriesChanged(const QList<QLandmarkCategoryId> &categoryIds);
    void categoriesRemoved(const QList<QLandmarkCategoryId> &categoryIds);

public:
    static void updateRequestState(QLandmarkAbstractRequest *req, QLandmarkAbstractRequest::State state);
    static void updateLandmarkIdFetchRequest(QLandmarkIdFetchRequest* req, const QList<QLandmarkId>& result,
            QLandmarkManager::Error error, const QString &errorString, QLandmarkAbstractRequest::State newState);
    static void updateLandmarkFetchRequest(QLandmarkFetchRequest* req, const QList<QLandmark>& result,
            QLandmarkManager::Error error, const QString &errorString, QLandmarkAbstractRequest::State newState);
    static void updateLandmarkFetchByIdRequest(QLandmarkFetchByIdRequest* req, const QList<QLandmark>& result, QLandmarkManager::Error error,
                                          const QString &errorString, const QMap<int, QLandmarkManager::Error>& errorMap, QLandmarkAbstractRequest::State newState);
    static void updateLandmarkRemoveRequest(QLandmarkRemoveRequest* req, QLandmarkManager::Error error, const QString &errorString,
                                            const QMap<int, QLandmarkManager::Error>& errorMap, QLandmarkAbstractRequest::State newState);
    static void updateLandmarkSaveRequest(QLandmarkSaveRequest* req, const QList<QLandmark>& result, QLandmarkManager::Error error,
                                          const QString &errorString, const QMap<int, QLandmarkManager::Error>& errorMap, QLandmarkAbstractRequest::State newState);
    static void updateLandmarkCategoryIdFetchRequest(QLandmarkCategoryIdFetchRequest *req, const QList<QLandmarkCategoryId>& result,
            QLandmarkManager::Error error, const QString &errorString, QLandmarkAbstractRequest::State newState);
    static void updateLandmarkCategoryFetchRequest(QLandmarkCategoryFetchRequest *req, const QList<QLandmarkCategory>& result,
            QLandmarkManager::Error error, const QString &errorString, QLandmarkAbstractRequest::State newState);
    static void updateLandmarkCategoryFetchByIdRequest(QLandmarkCategoryFetchByIdRequest *req, const QList<QLandmarkCategory>& result, QLandmarkManager::Error error,
            const QString &errorString, const QMap<int, QLandmarkManager::Error>& errorMap, QLandmarkAbstractRequest::State newState);
    static void updateLandmarkCategoryRemoveRequest(QLandmarkCategoryRemoveRequest *req, QLandmarkManager::Error error, const QString &errorString,
            const QMap<int, QLandmarkManager::Error>& errorMap, QLandmarkAbstractRequest::State newState);
    static void updateLandmarkCategorySaveRequest(QLandmarkCategorySaveRequest *req, const QList<QLandmarkCategory>& result, QLandmarkManager::Error error,
            const QString &errorString, const QMap<int, QLandmarkManager::Error>& errorMap, QLandmarkAbstractRequest::State newState);
    static void updateLandmarkImportRequest(QLandmarkImportRequest *req, const QList<QLandmarkId> &ids, QLandmarkManager::Error error, const QString &errorString,
                                            QLandmarkAbstractRequest::State newState);
    static void updateLandmarkExportRequest(QLandmarkExportRequest *req, QLandmarkManager::Error error, const QString &errorString,
                                            QLandmarkAbstractRequest::State newState);

    static int compareLandmark(const QLandmark& a, const QLandmark& b, const QList<QLandmarkSortOrder>& sortOrders);
    static int compareName(const QLandmark &a, const QLandmark &b, const QLandmarkNameSort &nameSort);

    static void addSorted(QList<QLandmark>* sorted, const QLandmark& landmark, const QList<QLandmarkSortOrder>& sortOrders);
    static bool testFilter(const QLandmarkFilter& filter, const QLandmark& landmark);
    static QList<QLandmarkId> sortLandmarks(const QList<QLandmark>& landmarks, const QList<QLandmarkSortOrder>& sortOrders);
};

QTM_END_NAMESPACE

#endif
