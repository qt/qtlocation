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

#ifndef QLANDMARK_H
#define QLANDMARK_H

#include "qmobilityglobal.h"

#include <QObject>
#include <QSharedDataPointer>
#include <QVariant>
#include <QStringList>
#include <QUrl>
#include "qgeoplace.h"
#include "qlandmarkid.h"
#include "qlandmarkcategoryid.h"

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QLandmarkPrivate;
class Q_LOCATION_EXPORT QLandmark : public QGeoPlace
{
public:
    QLandmark();
    QLandmark(const QGeoPlace &other);
    QLandmark(const QLandmark &other);
    ~QLandmark();

    QLandmark &operator= (const QLandmark &other);
    bool operator== (const QLandmark &other) const;
    bool operator!= (const QLandmark &other) const {
        return !(*this == other);
    }

    QString name() const;
    void setName(const QString &name);

    QList<QLandmarkCategoryId> categoryIds() const;
    void setCategoryIds(const QList<QLandmarkCategoryId> &categoryIds);
    void addCategoryId(const QLandmarkCategoryId &categoryId);
    void removeCategoryId(const QLandmarkCategoryId &categoryId);

    QString description() const;
    void setDescription(const QString &description);

    QUrl iconUrl() const;
    void setIconUrl(const QUrl &iconUrl);

    qreal radius() const;
    void setRadius(qreal radius);

    QString phoneNumber() const;
    void setPhoneNumber(const QString &phoneNumber);

    QUrl url() const;
    void setUrl(const QUrl &url);

    QLandmarkId landmarkId() const;
    void setLandmarkId(const QLandmarkId &id);

    void clear();
private:
    QLandmarkPrivate* d_func();
    const QLandmarkPrivate* d_func() const;
};

QTM_END_NAMESPACE

Q_DECLARE_METATYPE(QTM_PREPEND_NAMESPACE(QLandmark))

Q_DECLARE_TYPEINFO(QTM_PREPEND_NAMESPACE(QLandmark), Q_MOVABLE_TYPE);

QT_END_HEADER

#endif
