/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#include "qiopipe_p.h"
#include <QtCore/qmetaobject.h>
#include <QDebug>

QT_BEGIN_NAMESPACE

/*
    proxying means do *not* emit readyRead, and instead pump data
    into child pipes directly in a zero-copy fashion.
*/
QIOPipePrivate::QIOPipePrivate(QIODevice *iodevice, bool proxying)
    :  m_proxying(proxying), source(iodevice)
{
}

QIOPipePrivate::~QIOPipePrivate()
{
}

void QIOPipePrivate::initialize()
{
    const QIOPipe *parentPipe = qobject_cast<QIOPipe *>(source);
    if (parentPipe && parentPipe->d_func()->m_proxying) // with proxying parent,
        return;                                                     // don't do anything

    // read available data, does not emit.
    readAvailableData();
    // connect readyRead to onReadyRead
    QObjectPrivate::connect(source, &QIODevice::readyRead, this, &QIOPipePrivate::_q_onReadyRead);
}

bool QIOPipePrivate::readAvailableData() {
    if (!source)
        return false;
    QByteArray ba = source->readAll();
    if (!ba.size())
        return false;

    pumpData(ba);
    return true;
}

void QIOPipePrivate::pumpData(const QByteArray &ba)
{
    if (m_proxying) {
        QVector<int> toRemove;
        for (int i = 0; i < childPipes.size(); ++i) {
            const QPointer<QIOPipe> &cp = childPipes.at(i);
            if (!cp) {
                toRemove.append(i);
                continue;
            }
            QIOPipePrivate *cpp = cp->d_func();
            cpp->pushData(ba);
        }
        for (int i = toRemove.size() - 1; i >= 0; --i) {
            childPipes.remove(i);
        }
    } else {
        for (int i = 0; i < readBuffers.size(); i++)
            readBuffers[i].append(ba);
    }
}

void QIOPipePrivate::pushData(const QByteArray &ba)
{
    Q_Q(QIOPipe);
    if (!ba.size())
        return;

    pumpData(ba);
    if (!m_proxying)
        emit q->readyRead();
}

void QIOPipePrivate::_q_onReadyRead()
{
    Q_Q(QIOPipe);
    if (readAvailableData() && !m_proxying)
        emit q->readyRead();
}

void QIOPipePrivate::addChildPipe(QIOPipe *childPipe)
{
    if (childPipes.contains(childPipe))
        return;
    childPipes.append(childPipe);
}

void QIOPipePrivate::removeChildPipe(QIOPipe *childPipe)
{
    childPipes.removeOne(childPipe);
}

QIOPipe::QIOPipe(QIODevice *parent, Mode mode)
    : QIODevice(*new QIOPipePrivate(parent, mode == ProxyPipe), parent)
{
    this->d_func()->initialize();
    if (!parent->isOpen() && !parent->open(QIODevice::ReadOnly)) {
        qWarning() << "QIOPipe: Failed to open " << parent;
        return;
    }
    open(ReadOnly);
}

QIOPipe::~QIOPipe()
{

}

bool QIOPipe::open(QIODevice::OpenMode mode)
{
    if (isOpen())
        return true;

    static const OpenMode supportedOpenMode = ReadOnly; // Currently limit it to read only
    if (!(mode & supportedOpenMode)) {
        qFatal("Unsupported open mode");
        return false;
    }

    return QIODevice::open(mode);
}

bool QIOPipe::isSequential() const
{
    return true;
}

void QIOPipe::setReadChannelCount(int count)
{
    Q_D(QIOPipe);
    d->setReadChannelCount(qMax(count, 1));
}

void QIOPipe::addChildPipe(QIOPipe *childPipe)
{
    Q_D(QIOPipe);
    d->addChildPipe(childPipe);
}

/*!
    \reimp

    \omit
    This function does not really read anything, as we use QIODevicePrivate's
    buffer. The buffer will be read inside of QIODevice before this
    method will be called.
    See QIODevicePrivate::read, buffer.read(data, maxSize).
    \endomit
*/
qint64 QIOPipe::readData(char *data, qint64 maxlen)
{
    Q_UNUSED(data);
    Q_UNUSED(maxlen);

    // return 0 indicating there may be more data in the future
    // Returning -1 means no more data in the future (end of stream).
    return qint64(0);
}

qint64 QIOPipe::writeData(const char * /*data*/, qint64 /*len*/)
{
    qFatal("QIOPipe is a read-only device");
    return qint64(0);
}

QT_END_NAMESPACE
