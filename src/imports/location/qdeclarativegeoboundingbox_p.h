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
***************************************************************************/

#ifndef QDECLARATIVEGEOBOUNDINGBOX_P_H
#define QDECLARATIVEGEOBOUNDINGBOX_P_H

#include "qdeclarativecoordinate_p.h"
#include <qgeoboundingbox.h>

#include <QtCore>
#include <QtDeclarative/qdeclarative.h>

QTM_BEGIN_NAMESPACE

class QDeclarativeGeoBoundingBox : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeCoordinate* bottomLeft READ bottomLeft WRITE setBottomLeft NOTIFY bottomLeftChanged)
    Q_PROPERTY(QDeclarativeCoordinate* bottomRight READ bottomRight WRITE setBottomRight NOTIFY bottomRightChanged)
    Q_PROPERTY(QDeclarativeCoordinate* topLeft READ topLeft WRITE setTopLeft NOTIFY topLeftChanged)
    Q_PROPERTY(QDeclarativeCoordinate* topRight READ topRight WRITE setTopRight NOTIFY topRightChanged)
    Q_PROPERTY(QDeclarativeCoordinate* center READ center WRITE setCenter NOTIFY centerChanged)
    Q_PROPERTY(double height READ height WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(double width READ width WRITE setWidth NOTIFY widthChanged)

public:
    explicit QDeclarativeGeoBoundingBox(QObject* parent = 0);
    QDeclarativeGeoBoundingBox(const QGeoBoundingBox& box, QObject* parent = 0);
    void setBox(const QGeoBoundingBox& box);
    QGeoBoundingBox box();

    QDeclarativeCoordinate* bottomLeft();
    void setBottomLeft(QDeclarativeCoordinate* coordinate);
    QDeclarativeCoordinate* bottomRight();
    void setBottomRight(QDeclarativeCoordinate* coordinate);
    QDeclarativeCoordinate* topLeft();
    void setTopLeft(QDeclarativeCoordinate* coordinate);
    QDeclarativeCoordinate* topRight();
    void setTopRight(QDeclarativeCoordinate* coordinate);
    QDeclarativeCoordinate* center();
    void setCenter(QDeclarativeCoordinate* coordinate);
    double height();
    void setHeight(const double height);
    double width();
    void setWidth(const double width);

signals:
    void bottomLeftChanged();
    void bottomRightChanged();
    void topLeftChanged();
    void topRightChanged();
    void centerChanged();
    void heightChanged();
    void widthChanged();

private:
    void synchronizeDeclarative();

private:
    QDeclarativeCoordinate m_declarativeBottomLeft;
    QDeclarativeCoordinate m_declarativeBottomRight;
    QDeclarativeCoordinate m_declarativeTopLeft;
    QDeclarativeCoordinate m_declarativeTopRight;
    QDeclarativeCoordinate m_declarativeCenter;
    QGeoBoundingBox m_box;
    double m_height;
    double m_width;
};

QTM_END_NAMESPACE
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGeoBoundingBox));

#endif // QDECLARATIVEGEOBOUNDINGBOX_P_H
