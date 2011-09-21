/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEPLACE_P_H
#define QDECLARATIVEPLACE_P_H

#include <QObject>
#include <QDeclarativeListProperty>
#include <qplace.h>
#include "qdeclarativegeolocation_p.h"
#include "qdeclarativecategory_p.h"
#include "qdeclarativesupplier_p.h"
#include "qdeclarativerating_p.h"
#include "qdeclarativereviewmodel_p.h"
#include "qdeclarativeplaceimagemodel_p.h"
#include "qdeclarativeplaceeditorialmodel.h"
#include <QDeclarativePropertyMap>

QT_BEGIN_NAMESPACE

class QPlaceReply;

class QPlaceManager;

class QDeclarativePlace : public QObject, public QDeclarativeParserStatus
{
    Q_OBJECT

    Q_ENUMS(Status)

    Q_PROPERTY(QDeclarativeGeoServiceProvider *plugin READ plugin WRITE setPlugin NOTIFY pluginChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeCategory> categories READ categories NOTIFY categoriesChanged)
    Q_PROPERTY(QDeclarativeGeoLocation* location READ location WRITE setLocation NOTIFY locationChanged);
    Q_PROPERTY(QDeclarativeRating* rating READ rating WRITE setRating NOTIFY ratingChanged);
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeSupplier> suppliers READ suppliers NOTIFY suppliersChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged);
    Q_PROPERTY(QString placeId READ placeId WRITE setPlaceId NOTIFY placeIdChanged);

    Q_PROPERTY(QDeclarativeReviewModel *reviewModel READ reviewModel NOTIFY reviewModelChanged)
    Q_PROPERTY(QDeclarativePlaceImageModel *imageModel READ imageModel NOTIFY imageModelChanged)
    Q_PROPERTY(QDeclarativePlaceEditorialModel *editorialModel READ editorialModel NOTIFY editorialModelChanged)

    Q_PROPERTY(QDeclarativePropertyMap *extendedAttributes READ extendedAttributes WRITE setExtendedAttributes NOTIFY extendedAttributesChanged);
    Q_PROPERTY(bool detailsFetched READ detailsFetched WRITE setDetailsFetched NOTIFY detailsFetchedChanged);
    Q_PROPERTY(Status status READ status NOTIFY statusChanged);

    Q_INTERFACES(QDeclarativeParserStatus)

    Q_PROPERTY(QString primaryPhone READ primaryPhone WRITE setPrimaryPhone NOTIFY primaryPhoneChanged);
    Q_PROPERTY(QString primaryFax READ primaryFax WRITE setPrimaryFax NOTIFY primaryFaxChanged);
    Q_PROPERTY(QString primaryEmail READ primaryEmail WRITE setPrimaryEmail NOTIFY primaryEmailChanged);
    Q_PROPERTY(QUrl primaryUrl READ primaryUrl WRITE setPrimaryUrl NOTIFY primaryUrlChanged);

public:
    explicit QDeclarativePlace(QObject* parent = 0);
    explicit QDeclarativePlace(const QPlace &src, QObject* parent = 0);
    ~QDeclarativePlace();

    enum Status {Ready, Saving, Fetching, Removing, Error};

    //From QDeclarativeParserStatus
    virtual void classBegin() {}
    virtual void componentComplete();

    void setPlugin(QDeclarativeGeoServiceProvider *plugin);
    QDeclarativeGeoServiceProvider* plugin() const;

    QDeclarativeReviewModel *reviewModel();
    QDeclarativePlaceImageModel *imageModel();
    QDeclarativePlaceEditorialModel *editorialModel();

    QPlace place();
    void setPlace(const QPlace &src);

    QDeclarativeListProperty<QDeclarativeCategory> categories();
    static void category_append(QDeclarativeListProperty<QDeclarativeCategory> *prop,
                                  QDeclarativeCategory* value);
    static int category_count(QDeclarativeListProperty<QDeclarativeCategory> *prop);
    static QDeclarativeCategory* category_at(QDeclarativeListProperty<QDeclarativeCategory> *prop, int index);
    static void category_clear(QDeclarativeListProperty<QDeclarativeCategory> *prop);

    QDeclarativeGeoLocation *location();
    void setLocation(QDeclarativeGeoLocation *location);
    QDeclarativeRating *rating();
    void setRating(QDeclarativeRating *rating);
    QDeclarativeListProperty<QDeclarativeSupplier> suppliers();
    static void suppliers_append(QDeclarativeListProperty<QDeclarativeSupplier> *prop,
                                  QDeclarativeSupplier* value);
    static int suppliers_count(QDeclarativeListProperty<QDeclarativeSupplier> *prop);
    static QDeclarativeSupplier* suppliers_at(QDeclarativeListProperty<QDeclarativeSupplier> *prop, int index);
    static void suppliers_clear(QDeclarativeListProperty<QDeclarativeSupplier> *prop);
    QString name() const;
    void setName(const QString &name);
    QString placeId() const;
    void setPlaceId(const QString &placeId);
    bool detailsFetched() const;
    void setDetailsFetched(bool fetched);

    Status status() const;
    void setStatus(Status status);

    Q_INVOKABLE void getDetails();
    Q_INVOKABLE void ratePlace(qreal rating);
    Q_INVOKABLE void savePlace();
    Q_INVOKABLE void removePlace();
    Q_INVOKABLE QString errorString() const;

    QString primaryPhone() const;
    void setPrimaryPhone(const QString &phone);

    QString primaryFax() const;
    void setPrimaryFax(const QString &fax);

    QString primaryEmail() const;
    void setPrimaryEmail(const QString &email);

    QUrl primaryUrl() const;
    void setPrimaryUrl(const QUrl &url);

    QDeclarativePropertyMap *extendedAttributes() const;
    void setExtendedAttributes(QDeclarativePropertyMap *attrib);

signals:
    void pluginChanged();
    void categoriesChanged();
    void locationChanged();
    void ratingChanged();
    void suppliersChanged();
    void nameChanged();
    void placeIdChanged();
    void businessInformationChanged();
    void detailsFetchedChanged();
    void fetchingDetailsChanged();
    void reviewModelChanged();
    void imageModelChanged();
    void editorialModelChanged();

    void primaryPhoneChanged();
    void primaryFaxChanged();
    void primaryEmailChanged();
    void primaryUrlChanged();

    void extendedAttributesChanged();
    void statusChanged();

private slots:
    void finished();

private:
    void synchronizeCategories();
    void synchronizeSuppliers();
    void synchronizeExtendedAttributes();

private:
    QPlaceManager *manager();

    QList<QDeclarativeCategory*> m_categories;
    QDeclarativeGeoLocation *m_location;
    QDeclarativeRating *m_rating;
    QList<QDeclarativeSupplier*> m_suppliers;
    QDeclarativeReviewModel *m_reviewModel;
    QDeclarativePlaceImageModel *m_imageModel;
    QDeclarativePlaceEditorialModel *m_editorialModel;
    QDeclarativePropertyMap *m_extendedAttributes;

    QPlace m_src;

    QPlaceReply *m_reply;

    QDeclarativeGeoServiceProvider *m_plugin;
    bool m_complete;

    Status m_status;
    QString m_errorString;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativePlace));

#endif // QDECLARATIVEPLACE_P_H
