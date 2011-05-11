/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QDECLARATIVEREVERSEGEOCODEMODEL_H
#define QDECLARATIVEREVERSEGEOCODEMODEL_H

#include "qdeclarativegeosearchmodel_p.h"

#include "qdeclarativecoordinate_p.h"

QTM_BEGIN_NAMESPACE

class QDeclarativeReverseGeocodeModel : public QDeclarativeGeoSearchModel
{

    Q_OBJECT

    Q_PROPERTY(QDeclarativeCoordinate* coordinate READ coordinate WRITE setCoordinate NOTIFY coordinateChanged)

public:
    explicit QDeclarativeReverseGeocodeModel(QObject *parent = 0);
    ~QDeclarativeReverseGeocodeModel();

    void setCoordinate(QDeclarativeCoordinate *coordinate);
    QDeclarativeCoordinate* coordinate();

    // From QDeclarativeParserStatus
    virtual void componentComplete();

    // From QAbstractListModel
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

Q_SIGNALS:
    void coordinateChanged(QDeclarativeCoordinate *coordinate);

private:
    void update();

    bool complete_;
    QDeclarativeCoordinate coordinate_;
};

QTM_END_NAMESPACE

#endif
