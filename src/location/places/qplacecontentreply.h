/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QPLACECONTENTREPLY_H
#define QPLACECONTENTREPLY_H

#include <QtLocation/QPlaceReply>
#include <QtLocation/QPlaceContent>

QT_BEGIN_NAMESPACE

class QPlaceContentRequest;
class QPlaceContentReplyPrivate;
class Q_LOCATION_EXPORT QPlaceContentReply : public QPlaceReply
{
    Q_OBJECT

public:
    explicit QPlaceContentReply(QObject *parent = nullptr);
    virtual ~QPlaceContentReply();

    QPlaceReply::Type type() const override;

    QPlaceContent::Collection content() const;

    int totalCount() const;

    QPlaceContentRequest request() const;

    QPlaceContentRequest previousPageRequest() const;
    QPlaceContentRequest nextPageRequest() const;

protected:
    void setContent(const QPlaceContent::Collection &content);
    void setTotalCount(int total);
    void setRequest(const QPlaceContentRequest &request);
    void setPreviousPageRequest(const QPlaceContentRequest &previous);
    void setNextPageRequest(const QPlaceContentRequest &next);

private:
    Q_DISABLE_COPY(QPlaceContentReply)
    Q_DECLARE_PRIVATE(QPlaceContentReply)
};

QT_END_NAMESPACE

#endif
