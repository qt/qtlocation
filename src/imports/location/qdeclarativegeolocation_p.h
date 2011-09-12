#ifndef QDECLARATIVEGEOLOCATION_P_H
#define QDECLARATIVEGEOLOCATION_P_H

#include <QObject>
#include <QDeclarativeListProperty>
#include <qgeolocation.h>
#include "qdeclarativecoordinate_p.h"
#include "qdeclarativegeoboundingbox_p.h"
#include "qdeclarativegeoaddress_p.h"

QT_BEGIN_NAMESPACE

class QDeclarativeGeoLocation : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeGeoAddress* address READ address WRITE setAddress NOTIFY addressChanged);
    Q_PROPERTY(QDeclarativeCoordinate* coordinate READ coordinate WRITE setCoordinate NOTIFY coordinateChanged);
    Q_PROPERTY(QString locationId READ locationId WRITE setLocationId NOTIFY locationIdChanged);
    Q_PROPERTY(QDeclarativeGeoBoundingBox* viewport READ viewport WRITE setViewport NOTIFY viewportChanged);

public:
    explicit QDeclarativeGeoLocation(QObject* parent = 0);
    explicit QDeclarativeGeoLocation(const QGeoLocation &src, QObject* parent = 0);
    ~QDeclarativeGeoLocation();

    QGeoLocation location() ;
    void setLocation(const QGeoLocation &src);

    QDeclarativeGeoAddress *address();
    void setAddress(QDeclarativeGeoAddress *address);
    QDeclarativeCoordinate *coordinate();
    void setCoordinate(QDeclarativeCoordinate *coordinate);
    QString locationId() const;
    void setLocationId(const QString &locationId);
    QDeclarativeGeoBoundingBox *viewport();
    void setViewport(QDeclarativeGeoBoundingBox *boundingBox);

signals:
    void addressChanged();
    void coordinateChanged();
    void locationIdChanged();
    void viewportChanged();

private:
    QDeclarativeGeoAddress m_address;
    QDeclarativeCoordinate m_coordinate;
    QDeclarativeGeoBoundingBox m_boundingBox;
    QGeoLocation m_src;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeGeoLocation));

#endif // QDECLARATIVELOCATION_P_H
