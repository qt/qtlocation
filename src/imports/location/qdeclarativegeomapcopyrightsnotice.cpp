/****************************************************************************
**
** Copyright (C) 2014 Aaron McCarthy <mccarthy.aaron@gmail.com>
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia. For licensing terms and
** conditions see http://qt.digia.com/licensing. For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights. These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativegeomapcopyrightsnotice_p.h"

#include <QtGui/QTextDocument>
#include <QtGui/QAbstractTextDocumentLayout>
#include <QtGui/QPainter>
#include <QtQuick/private/qquickanchors_p.h>
#include <QtQuick/private/qquickanchors_p_p.h>

QT_BEGIN_NAMESPACE

QDeclarativeGeoMapCopyrightNotice::QDeclarativeGeoMapCopyrightNotice(QQuickItem *parent)
:   QQuickPaintedItem(parent), m_copyrightsHtml(0)
{
    QQuickAnchors *anchors = property("anchors").value<QQuickAnchors *>();
    if (anchors) {
        anchors->setLeft(QQuickAnchorLine(parent, QQuickAnchorLine::Left));
        anchors->setBottom(QQuickAnchorLine(parent, QQuickAnchorLine::Bottom));
    }
}

QDeclarativeGeoMapCopyrightNotice::~QDeclarativeGeoMapCopyrightNotice()
{
}

/*!
    \internal
*/
void QDeclarativeGeoMapCopyrightNotice::paint(QPainter *painter)
{
    painter->drawImage(0, 0, m_copyrightsImage);
}

void QDeclarativeGeoMapCopyrightNotice::mousePressEvent(QMouseEvent *event)
{
    if (m_copyrightsHtml) {
        m_activeAnchor = m_copyrightsHtml->documentLayout()->anchorAt(event->pos());
        if (!m_activeAnchor.isEmpty())
            return;
    }

    QQuickPaintedItem::mousePressEvent(event);
}

void QDeclarativeGeoMapCopyrightNotice::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_copyrightsHtml) {
        QString anchor = m_copyrightsHtml->documentLayout()->anchorAt(event->pos());
        if (anchor == m_activeAnchor && !anchor.isEmpty()) {
            emit linkActivated(anchor);
            m_activeAnchor.clear();
        }
    }
}

/*!
    \internal
*/
void QDeclarativeGeoMapCopyrightNotice::setCopyrightsZ(int copyrightsZ)
{
    setZ(copyrightsZ);
    update();
}

/*!
    \internal
*/
void QDeclarativeGeoMapCopyrightNotice::copyrightsChanged(const QImage &copyrightsImage)
{
    delete m_copyrightsHtml;
    m_copyrightsHtml = 0;

    m_copyrightsImage = copyrightsImage;

    setWidth(m_copyrightsImage.width());
    setHeight(m_copyrightsImage.height());

    setKeepMouseGrab(false);
    setAcceptedMouseButtons(Qt::NoButton);

    update();
}

void QDeclarativeGeoMapCopyrightNotice::copyrightsChanged(const QString &copyrightsHtml)
{
    if (!m_copyrightsHtml)
        m_copyrightsHtml = new QTextDocument(this);

    m_copyrightsHtml->setHtml(copyrightsHtml);

    m_copyrightsImage = QImage(m_copyrightsHtml->size().toSize(),
                               QImage::Format_ARGB32_Premultiplied);
    m_copyrightsImage.fill(qPremultiply(qRgba(255, 255, 255, 128)));

    QPainter painter(&m_copyrightsImage);
    m_copyrightsHtml->drawContents(&painter);

    setWidth(m_copyrightsImage.width());
    setHeight(m_copyrightsImage.height());

    setContentsSize(m_copyrightsImage.size());

    setKeepMouseGrab(true);
    setAcceptedMouseButtons(Qt::LeftButton);

    update();
}

QT_END_NAMESPACE
