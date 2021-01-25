/****************************************************************************
**
** Copyright (C) 2014 Aaron McCarthy <mccarthy.aaron@gmail.com>
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
****************************************************************************/

#ifndef QDECLARATIVEGEOMAPCOPYRIGHTSNOTICE_H
#define QDECLARATIVEGEOMAPCOPYRIGHTSNOTICE_H

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

#include <QtGui/QImage>
#include <QPointer>
#include <QtQuick/QQuickPaintedItem>

QT_BEGIN_NAMESPACE

class QTextDocument;
class QDeclarativeGeoMap;
class QDeclarativeGeoMapCopyrightNoticePrivate;
class Q_LOCATION_PRIVATE_EXPORT QDeclarativeGeoMapCopyrightNotice : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeGeoMap *mapSource READ mapSource WRITE setMapSource NOTIFY mapSourceChanged)
    Q_PROPERTY(QString styleSheet READ styleSheet WRITE setStyleSheet NOTIFY styleSheetChanged)

public:
    QDeclarativeGeoMapCopyrightNotice(QQuickItem *parent = nullptr);
    ~QDeclarativeGeoMapCopyrightNotice();

    void setCopyrightsZ(qreal copyrightsZ);

    void setCopyrightsVisible(bool visible);
    bool copyrightsVisible() const;
    void anchorToBottomLeft();

    void setMapSource(QDeclarativeGeoMap *mapSource);
    QDeclarativeGeoMap *mapSource();

    QString styleSheet() const;
    void setStyleSheet(const QString &styleSheet);

public Q_SLOTS:
    void copyrightsChanged(const QImage &copyrightsImage);
    void copyrightsChanged(const QString &copyrightsHtml);
    void onCopyrightsStyleSheetChanged(const QString &styleSheet);

signals:
    void linkActivated(const QString &link);
    void mapSourceChanged();
    void backgroundColorChanged(const QColor &color);
    void styleSheetChanged(const QString &styleSheet);
    void copyrightsVisibleChanged();

protected:
    void paint(QPainter *painter) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void rasterizeHtmlAndUpdate();
    void connectMap();

private:
    void createCopyright();

    QTextDocument *m_copyrightsHtml;
    QString m_html;
    QImage m_copyrightsImage;
    QString m_activeAnchor;
    bool m_copyrightsVisible;
    QPointer<QDeclarativeGeoMap> m_mapSource;
    QColor m_backgroundColor;
    QString m_styleSheet;
    bool m_userDefinedStyleSheet;

    Q_DISABLE_COPY(QDeclarativeGeoMapCopyrightNotice)
    Q_DECLARE_PRIVATE(QDeclarativeGeoMapCopyrightNotice)
};

QT_END_NAMESPACE

#endif
