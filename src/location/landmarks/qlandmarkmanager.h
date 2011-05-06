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

#ifndef QLANDMARKMANAGER_H
#define QLANDMARKMANAGER_H

#include "qmobilityglobal.h"
#include "qlandmarkid.h"
#include "qlandmarkfilter.h"
#include "qlandmarksortorder.h"
#include "qlandmarkcategoryid.h"
#include "qlandmarknamesort.h"
#include "qlandmarkfilter.h"
#include "qlandmarkcategory.h"
#include "qlandmark.h"

#include <qlatin1constant.h>
#include <QObject>
#include <QMap>
class QIODevice;
class QStringList;

QTM_BEGIN_NAMESPACE
class QLandmarkManagerPrivate;
class QLandmarkManagerEngine;

class Q_LOCATION_EXPORT QLandmarkManager: public QObject
{
    Q_OBJECT
public:
    enum Error {
        NoError = 0,
        DoesNotExistError,
        LandmarkDoesNotExistError,
        CategoryDoesNotExistError,
        AlreadyExistsError,
        LockedError,
        PermissionsError,
        OutOfMemoryError,
        VersionMismatchError,
        NotSupportedError,
        BadArgumentError,
        InvalidManagerError,
        ParsingError,
        CancelError,
        UnknownError,
    };

    enum SupportLevel {
        NativeSupport,
        EmulatedSupport,
        NoSupport
    };

    enum ManagerFeature {
        ImportExportFeature,
        NotificationsFeature
    };


    enum TransferOption {
        IncludeCategoryData,
        ExcludeCategoryData,
        AttachSingleCategory
    };

    enum TransferOperation {
        ImportOperation,
        ExportOperation
    };

#ifdef Q_QDOC
    static const QLatin1Constant Gpx;
    static const QLatin1Constant Lmx;
    static const QLatin1Constant Kml;
    static const QLatin1Constant Kmz;
#else
    Q_DECLARE_LATIN1_CONSTANT(Gpx, "Gpx");
    Q_DECLARE_LATIN1_CONSTANT(Lmx, "Lmx");
    Q_DECLARE_LATIN1_CONSTANT(Kml, "Kml");
    Q_DECLARE_LATIN1_CONSTANT(Kmz, "Kmz");
#endif

#ifdef Q_QDOC
    QLandmarkManager(QObject *parent = 0);
    QLandmarkManager(const QString &managerName, const QMap<QString, QString> &parameters = 0, QObject *parent = 0);
#else
    QLandmarkManager(QObject *parent = 0);
    QLandmarkManager(const QString &managerName, const QMap<QString, QString>& parameters = (QMap<QString, QString>()), QObject *parent = 0);
#endif
    virtual ~QLandmarkManager();

    bool saveLandmark(QLandmark *landmark);
    bool saveLandmarks(QList<QLandmark> *landmarks);

    bool removeLandmark(const QLandmarkId &landmarkId);
    bool removeLandmark(const QLandmark &landmark);

    bool removeLandmarks(const QList<QLandmarkId> &landmarksIds);
    bool removeLandmarks(const QList<QLandmark> &landmarks);

    bool saveCategory(QLandmarkCategory *category);
    bool removeCategory(const QLandmarkCategoryId &categoryId);
    bool removeCategory(const QLandmarkCategory &category);

    QLandmarkCategory category(const QLandmarkCategoryId &categoryId) const;
    QList<QLandmarkCategory> categories(const QList<QLandmarkCategoryId> &categoryIds) const;

    QList<QLandmarkCategory> categories( int limit=-1, int offset=0, const QLandmarkNameSort &nameSort = QLandmarkNameSort()) const;
    QList<QLandmarkCategoryId> categoryIds(int limit =-1, int offset=0, const QLandmarkNameSort &nameSort = QLandmarkNameSort()) const;

    QLandmark landmark(const QLandmarkId &landmarkId) const;
    QList<QLandmark> landmarks(const QLandmarkFilter &filter,
                                int limit, int offset,
                                const QList<QLandmarkSortOrder>& sortOrders) const;
    QList<QLandmark> landmarks(const QLandmarkFilter &filter = QLandmarkFilter(),
                            int limit=-1, int offset=0,
                            const QLandmarkSortOrder &sortOrder = QLandmarkSortOrder()) const;

    QList<QLandmark> landmarks(const QList<QLandmarkId> &landmarkIds) const;
    QList<QLandmarkId> landmarkIds(const QLandmarkFilter &filter,
                                    int limit, int offset,
                                   const QList<QLandmarkSortOrder> &sortOrders)const;
    QList<QLandmarkId> landmarkIds(const QLandmarkFilter &filter = QLandmarkFilter(),
                                    int limit=-1, int offset =0,
                                   const QLandmarkSortOrder &sortOrder = QLandmarkSortOrder()) const;

    bool importLandmarks(QIODevice *device, const QString &format= QString() ,QLandmarkManager::TransferOption option = IncludeCategoryData, const QLandmarkCategoryId& = QLandmarkCategoryId());
    bool importLandmarks(const QString &fileName, const QString &format = QString(),QLandmarkManager::TransferOption option = IncludeCategoryData, const QLandmarkCategoryId& = QLandmarkCategoryId());
    bool exportLandmarks(QIODevice *device, const QString &format, const QList<QLandmarkId> &landmarkIds = QList<QLandmarkId>(), QLandmarkManager::TransferOption option = IncludeCategoryData) const;
    bool exportLandmarks(const QString &, const QString &format, const QList<QLandmarkId> &landmarkIds = QList<QLandmarkId>(), QLandmarkManager::TransferOption option = IncludeCategoryData) const;

    QStringList supportedFormats(TransferOperation operation) const;

    Error error() const;
    QString errorString() const;
    QMap<int, QLandmarkManager::Error> errorMap() const;

    bool isFeatureSupported(ManagerFeature feature) const;
    SupportLevel filterSupportLevel(const QLandmarkFilter &filter) const;
    SupportLevel sortOrderSupportLevel(const QLandmarkSortOrder &sortOrder) const;

    bool isReadOnly() const;
    bool isReadOnly(const QLandmarkId &id) const;
    bool isReadOnly(const QLandmarkCategoryId &id) const;

    QStringList searchableLandmarkAttributeKeys() const;

    QString managerName() const;
    QMap<QString, QString> managerParameters() const;
    QString managerUri() const;
    int managerVersion() const;

    static QStringList availableManagers();
    static QString buildUri(const QString& managerName, const QMap<QString, QString>& params,int implementationVersion = -1);
    static QLandmarkManager* fromUri(const QString& uri, QObject* parent = 0);
    static bool parseUri(const QString& uri, QString* managerName, QMap<QString, QString>* params);

Q_SIGNALS:
    void dataChanged();
    void landmarksAdded(const QList<QLandmarkId> &landmarkIds);
    void landmarksChanged(const QList<QLandmarkId> &landmarkIds);
    void landmarksRemoved(const QList<QLandmarkId> &landmarkIds);

    void categoriesAdded(const QList<QLandmarkCategoryId> &categoryIds);
    void categoriesChanged(const QList<QLandmarkCategoryId> &categoryIds);
    void categoriesRemoved(const QList<QLandmarkCategoryId> &categoryIds);

protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);

private:
    QLandmarkManagerPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QLandmarkManager)
    friend class QLandmarkAbstractRequest;
    QLandmarkManagerEngine *engine();
};

QTM_END_NAMESPACE

#endif
