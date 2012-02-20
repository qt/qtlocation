/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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
****************************************************************************/

#ifndef UNSUPPORTEDREPLIES_P_H
#define UNSUPPORTEDREPLIES_P_H

#include "qplacedetailsreply.h"
#include "qplacecontentreply.h"
#include "qplacesearchreply.h"
#include "qplacesearchsuggestionreply.h"
#include "qplaceidreply.h"

#include "qplacematchreply.h"
#include "qplacemanagerengine.h"

class QPlaceDetailsReplyUnsupported : public QPlaceDetailsReply
{
    Q_OBJECT

public:
    QPlaceDetailsReplyUnsupported(QPlaceManagerEngine *parent)
    :   QPlaceDetailsReply(parent)
    {
        setError(QPlaceReply::UnsupportedError,
                 QStringLiteral("Getting place details is not supported."));
        setFinished(this);
        QMetaObject::invokeMethod(this, "error", Qt::QueuedConnection,
                                  Q_ARG(QPlaceReply::Error, error()),
                                  Q_ARG(QString, errorString()));
        QMetaObject::invokeMethod(parent, "error", Qt::QueuedConnection,
                                  Q_ARG(QPlaceReply *, this),
                                  Q_ARG(QPlaceReply::Error, error()),
                                  Q_ARG(QString, errorString()));
        QMetaObject::invokeMethod(this, "finished", Qt::QueuedConnection);
        QMetaObject::invokeMethod(parent, "finished", Qt::QueuedConnection,
                                  Q_ARG(QPlaceReply *, this));
    }
};

class QPlaceContentReplyUnsupported : public QPlaceContentReply
{
    Q_OBJECT

public:
    QPlaceContentReplyUnsupported(QPlaceManagerEngine *parent)
    :   QPlaceContentReply(parent)
    {
        setError(QPlaceReply::UnsupportedError,
                 QStringLiteral("Place content is not supported."));
        setFinished(this);
        QMetaObject::invokeMethod(this, "error", Qt::QueuedConnection,
                                  Q_ARG(QPlaceReply::Error, error()),
                                  Q_ARG(QString, errorString()));
        QMetaObject::invokeMethod(parent, "error", Qt::QueuedConnection,
                                  Q_ARG(QPlaceReply *, this),
                                  Q_ARG(QPlaceReply::Error, error()),
                                  Q_ARG(QString, errorString()));
        QMetaObject::invokeMethod(this, "finished", Qt::QueuedConnection);
        QMetaObject::invokeMethod(parent, "finished", Qt::QueuedConnection,
                                  Q_ARG(QPlaceReply *, this));
    }
};

class QPlaceSearchReplyUnsupported : public QPlaceSearchReply
{
    Q_OBJECT

public:
    QPlaceSearchReplyUnsupported(const QString &message, QPlaceManagerEngine *parent)
    :   QPlaceSearchReply(parent)
    {
        setError(QPlaceReply::UnsupportedError, message);
        setFinished(this);
        QMetaObject::invokeMethod(this, "error", Qt::QueuedConnection,
                                  Q_ARG(QPlaceReply::Error, error()),
                                  Q_ARG(QString, errorString()));
        QMetaObject::invokeMethod(parent, "error", Qt::QueuedConnection,
                                  Q_ARG(QPlaceReply *, this),
                                  Q_ARG(QPlaceReply::Error, error()),
                                  Q_ARG(QString, errorString()));
        QMetaObject::invokeMethod(this, "finished", Qt::QueuedConnection);
        QMetaObject::invokeMethod(parent, "finished", Qt::QueuedConnection,
                                  Q_ARG(QPlaceReply *, this));
    }
};

class QPlaceSearchSuggestionReplyUnsupported : public QPlaceSearchSuggestionReply
{
    Q_OBJECT

public:
    QPlaceSearchSuggestionReplyUnsupported(QPlaceManagerEngine *parent)
    :   QPlaceSearchSuggestionReply(parent)
    {
        setError(QPlaceReply::UnsupportedError,
                 QStringLiteral("Place search suggestions are not supported."));
        setFinished(this);
        QMetaObject::invokeMethod(this, "error", Qt::QueuedConnection,
                                  Q_ARG(QPlaceReply::Error, error()),
                                  Q_ARG(QString, errorString()));
        QMetaObject::invokeMethod(parent, "error", Qt::QueuedConnection,
                                  Q_ARG(QPlaceReply *, this),
                                  Q_ARG(QPlaceReply::Error, error()),
                                  Q_ARG(QString, errorString()));
        QMetaObject::invokeMethod(this, "finished", Qt::QueuedConnection);
        QMetaObject::invokeMethod(parent, "finished", Qt::QueuedConnection,
                                  Q_ARG(QPlaceReply *, this));
    }
};

class QPlaceIdReplyUnsupported : public QPlaceIdReply
{
    Q_OBJECT

public:
    QPlaceIdReplyUnsupported(const QString &message, QPlaceIdReply::OperationType type,
                             QPlaceManagerEngine *parent)
    :   QPlaceIdReply(type, parent)
    {
        setError(QPlaceReply::UnsupportedError, message);
        setFinished(this);
        QMetaObject::invokeMethod(this, "error", Qt::QueuedConnection,
                                  Q_ARG(QPlaceReply::Error, error()),
                                  Q_ARG(QString, errorString()));
        QMetaObject::invokeMethod(parent, "error", Qt::QueuedConnection,
                                  Q_ARG(QPlaceReply *, this),
                                  Q_ARG(QPlaceReply::Error, error()),
                                  Q_ARG(QString, errorString()));
        QMetaObject::invokeMethod(this, "finished", Qt::QueuedConnection);
        QMetaObject::invokeMethod(parent, "finished", Qt::QueuedConnection,
                                  Q_ARG(QPlaceReply *, this));
    }
};

class QPlaceReplyUnsupported : public QPlaceReply
{
    Q_OBJECT

public:
    QPlaceReplyUnsupported(const QString &message, QPlaceManagerEngine *parent)
    :   QPlaceReply(parent)
    {
        setError(QPlaceReply::UnsupportedError, message);
        setFinished(this);
        QMetaObject::invokeMethod(this, "error", Qt::QueuedConnection,
                                  Q_ARG(QPlaceReply::Error, error()),
                                  Q_ARG(QString, errorString()));
        QMetaObject::invokeMethod(parent, "error", Qt::QueuedConnection,
                                  Q_ARG(QPlaceReply *, this),
                                  Q_ARG(QPlaceReply::Error, error()),
                                  Q_ARG(QString, errorString()));
        QMetaObject::invokeMethod(this, "finished", Qt::QueuedConnection);
        QMetaObject::invokeMethod(parent, "finished", Qt::QueuedConnection,
                                  Q_ARG(QPlaceReply *, this));
    }
};

class QPlaceMatchReplyUnsupported : public QPlaceMatchReply
{
    Q_OBJECT

public:
    QPlaceMatchReplyUnsupported(QPlaceManagerEngine *parent)
    :   QPlaceMatchReply(parent)
    {
        setError(QPlaceReply::UnsupportedError,
                 QStringLiteral("Place matching is not supported."));
        setFinished(this);
        QMetaObject::invokeMethod(this, "error", Qt::QueuedConnection,
                                  Q_ARG(QPlaceReply::Error, error()),
                                  Q_ARG(QString, errorString()));
        QMetaObject::invokeMethod(parent, "error", Qt::QueuedConnection,
                                  Q_ARG(QPlaceReply *, this),
                                  Q_ARG(QPlaceReply::Error, error()),
                                  Q_ARG(QString, errorString()));
        QMetaObject::invokeMethod(this, "finished", Qt::QueuedConnection);
        QMetaObject::invokeMethod(parent, "finished", Qt::QueuedConnection,
                                  Q_ARG(QPlaceReply *, this));
    }
};

#endif
