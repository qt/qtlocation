/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
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

#ifndef QDECLARATIVECATEGORY_P_H
#define QDECLARATIVECATEGORY_P_H

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

#include <QtLocation/private/qlocationglobal_p.h>
#include <QtQml/qqml.h>
#include <QtQml/QQmlParserStatus>
#include <QObject>

#include <QtLocation/qplacecategory.h>
#include <QtLocation/qplaceicon.h>

#include <QtLocation/private/qdeclarativegeoserviceprovider_p.h>

QT_BEGIN_NAMESPACE

class QPlaceReply;
class QPlaceManager;

class Q_LOCATION_PRIVATE_EXPORT QDeclarativeCategory : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Category)
    QML_ADDED_IN_VERSION(5, 0)

    Q_ENUMS(Status Visibility)


    Q_PROPERTY(QPlaceCategory category READ category WRITE setCategory)
    Q_PROPERTY(QDeclarativeGeoServiceProvider *plugin READ plugin WRITE setPlugin NOTIFY pluginChanged)
    Q_PROPERTY(QString categoryId READ categoryId WRITE setCategoryId NOTIFY categoryIdChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(Visibility visibility READ visibility WRITE setVisibility NOTIFY visibilityChanged)
    Q_PROPERTY(QPlaceIcon icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)

    Q_INTERFACES(QQmlParserStatus)

public:
    explicit QDeclarativeCategory(QObject *parent = nullptr);
    QDeclarativeCategory(const QPlaceCategory &category, QDeclarativeGeoServiceProvider *plugin, QObject *parent = nullptr);
    ~QDeclarativeCategory();

    enum Visibility {
        UnspecifiedVisibility = QLocation::UnspecifiedVisibility,
        DeviceVisibility = QLocation::DeviceVisibility,
        PrivateVisibility = QLocation::PrivateVisibility,
        PublicVisibility = QLocation::PublicVisibility
    };
    enum Status {Ready, Saving, Removing, Error};

    //From QQmlParserStatus
    void classBegin() override {}
    void componentComplete() override;

    void setPlugin(QDeclarativeGeoServiceProvider *plugin);
    QDeclarativeGeoServiceProvider *plugin() const;

    QPlaceCategory category();
    void setCategory(const QPlaceCategory &category);

    QString categoryId() const;
    void setCategoryId(const QString &catID);
    QString name() const;
    void setName(const QString &name);

    Visibility visibility() const;
    void setVisibility(Visibility visibility);

    QPlaceIcon icon() const;
    void setIcon(const QPlaceIcon &icon);

    Q_INVOKABLE QString errorString() const;

    Status status() const;
    void setStatus(Status status, const QString &errorString = QString());

    Q_INVOKABLE void save(const QString &parentId = QString());
    Q_INVOKABLE void remove();

Q_SIGNALS:
    void pluginChanged();
    void categoryIdChanged();
    void nameChanged();
    void visibilityChanged();
    void iconChanged();
    void statusChanged();

private Q_SLOTS:
    void replyFinished();
    void pluginReady();

private:
    QPlaceManager *manager();

    QPlaceCategory m_category;
    QDeclarativeGeoServiceProvider *m_plugin = nullptr;
    QPlaceReply *m_reply = nullptr;
    bool m_complete = false;
    Status m_status = Ready;
    QString m_errorString;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeCategory)

#endif // QDECLARATIVECATEGORY_P_H
