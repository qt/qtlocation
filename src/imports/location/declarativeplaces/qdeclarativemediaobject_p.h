#ifndef QDECLARATIVEMEDIAOBJECT_P_H
#define QDECLARATIVEMEDIAOBJECT_P_H

#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtDeclarative/QDeclarativeListProperty>

#include <qplacemediaobject.h>
#include "qdeclarativesupplier_p.h"

QT_BEGIN_NAMESPACE

class QDeclarativeMediaObject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged);
    Q_PROPERTY(QUrl thumbnailUrl READ thumbnailUrl WRITE setThumbnailUrl NOTIFY thumbnailUrlChanged);
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged);
    Q_PROPERTY(QDeclarativeSupplier* supplier READ supplier WRITE setSupplier NOTIFY supplierChanged);
    Q_PROPERTY(QString mimeType READ mimeType WRITE setMimeType NOTIFY mimeTypeChanged);
    Q_PROPERTY(QString metaInfo READ metaInfo WRITE setMetaInfo NOTIFY metaInfoChanged);

public:
    explicit QDeclarativeMediaObject(QObject* parent = 0);
    explicit QDeclarativeMediaObject(const QPlaceMediaObject &src, QObject* parent = 0);
    ~QDeclarativeMediaObject();

    QPlaceMediaObject mediaObject();
    void setMediaObject(const QPlaceMediaObject &src);

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
    QPlaceMediaObject m_src;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeMediaObject));

#endif // QDECLARATIVEMEDIAOBJECT_P_H
