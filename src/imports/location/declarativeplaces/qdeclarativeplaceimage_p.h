#ifndef QDECLARATIVEPLACEIMAGE_P_H
#define QDECLARATIVEPLACEIMAGE_P_H

#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtDeclarative/QDeclarativeListProperty>

#include "qdeclarativesupplier_p.h"
#include <qplaceimage.h>

QT_BEGIN_NAMESPACE

class QDeclarativePlaceImage : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged);
    Q_PROPERTY(QUrl thumbnailUrl READ thumbnailUrl WRITE setThumbnailUrl NOTIFY thumbnailUrlChanged);
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged);
    Q_PROPERTY(QDeclarativeSupplier* supplier READ supplier WRITE setSupplier NOTIFY supplierChanged);
    Q_PROPERTY(QString mimeType READ mimeType WRITE setMimeType NOTIFY mimeTypeChanged);
    Q_PROPERTY(QString metaInfo READ metaInfo WRITE setMetaInfo NOTIFY metaInfoChanged);

public:
    explicit QDeclarativePlaceImage(QObject* parent = 0);
    explicit QDeclarativePlaceImage(const QPlaceImage &src, QObject* parent = 0);
    ~QDeclarativePlaceImage();

    QPlaceImage image();
    void setImage(const QPlaceImage &src);

    QUrl url() const;
    void setUrl(const QUrl &url);
    QUrl thumbnailUrl() const;
    void setThumbnailUrl(const QUrl &url);
    QString id() const;
    void setId(const QString &data);
    QString metaInfo() const;
    void setMetaInfo(const QString &data);
    QString mimeType() const;
    void setMimeType(const QString &data);
    QDeclarativeSupplier *supplier();
    void setSupplier(QDeclarativeSupplier *data);

signals:
    void urlChanged();
    void thumbnailUrlChanged();
    void idChanged();
    void supplierChanged();
    void mimeTypeChanged();
    void metaInfoChanged();

private:
    QDeclarativeSupplier m_declarativeSupplier;
    QPlaceImage m_src;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativePlaceImage));

#endif
