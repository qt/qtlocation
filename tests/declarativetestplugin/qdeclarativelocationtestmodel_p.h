#ifndef QDECLARATIVELOCATIONTESTMODEL_H
#define QDECLARATIVELOCATIONTESTMODEL_H

#include <QtDeclarative/QSGItem>
#include <QAbstractListModel>
#include <QTimer>
#include <QDebug>
#include <QList>
#include <qdeclarativecoordinate_p.h>

class DataObject: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeCoordinate* coordinate READ coordinate CONSTANT)

public:
    DataObject() : coordinate_(0) {}
    ~DataObject() {delete coordinate_;}

    QDeclarativeCoordinate* coordinate_;
    QDeclarativeCoordinate* coordinate() const {return coordinate_;}
};

class QDeclarativeLocationTestModel : public QAbstractListModel, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_PROPERTY(int datacount READ datacount WRITE setDatacount NOTIFY datacountChanged)
    Q_PROPERTY(int delay READ delay WRITE setDelay NOTIFY delayChanged)
    Q_PROPERTY(QString datatype READ datatype WRITE setDatatype NOTIFY datatypeChanged)
    Q_INTERFACES(QDeclarativeParserStatus)

public:
    QDeclarativeLocationTestModel(QObject* parent = 0);
    ~QDeclarativeLocationTestModel();

    enum Roles {
        TestDataRole = Qt::UserRole + 500
    };

    // from QDeclarativeParserStatus
    virtual void componentComplete();
    virtual void classBegin() {}

    // From QAbstractListModel
    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    int datacount() const;
    void setDatacount(int datacount);

    int delay() const;
    void setDelay(int delay);

    QString datatype() const;
    void setDatatype(QString datatype);

    //Q_INVOKABLE void clear();
    Q_INVOKABLE void reset();
    //Q_INVOKABLE void reset();

signals:
    void countChanged();
    void datacountChanged();
    void datatypeChanged();
    void delayChanged();
    void modelChanged();

private slots:
    void repopulate();

private:
    void scheduleRepopulation();

private:
    int delay_;
    int datacount_;
    bool componentCompleted_;
    QString datatype_;
    QTimer timer_;
    QList<DataObject*> dataobjects_;
};

#endif
