#include "qdeclarativelocationtestmodel_p.h"
#include <qgeocoordinate.h>
#include <QtGui/QApplication>

QDeclarativeLocationTestModel::QDeclarativeLocationTestModel(QObject *parent):
     QAbstractListModel(parent),
    delay_(0),
    datacount_(0),
    componentCompleted_(false)
{
    timer_.setSingleShot(true);
    connect(&timer_, SIGNAL(timeout()), this, SLOT(repopulate()));
    // Establish role names so that they can be queried from this model
    QHash<int, QByteArray> roleNames;
    roleNames = QAbstractItemModel::roleNames();
    roleNames.insert(TestDataRole, "modeldata");
    setRoleNames(roleNames);
}

QDeclarativeLocationTestModel::~QDeclarativeLocationTestModel()
{
    if (timer_.isActive())
        timer_.stop();
    if (!dataobjects_.isEmpty()) {
        qDeleteAll(dataobjects_);
        dataobjects_.clear();
    }
}

void QDeclarativeLocationTestModel::componentComplete()
{
    componentCompleted_ = true;
    scheduleRepopulation();
}


int QDeclarativeLocationTestModel::datacount() const
{
    return datacount_;
}

void QDeclarativeLocationTestModel::setDatacount(int datacount)
{
    if (datacount_ == datacount)
        return;
    datacount_ = datacount;
    emit datacountChanged();
    scheduleRepopulation();
}

int QDeclarativeLocationTestModel::delay() const
{
    return delay_;
}

void QDeclarativeLocationTestModel::setDelay(int delay)
{
    if (delay_ == delay)
        return;
    delay_ = delay;
    emit delayChanged();
}

QString QDeclarativeLocationTestModel::datatype() const
{
    return datatype_;
}

void QDeclarativeLocationTestModel::setDatatype(QString datatype)
{
    if (datatype_ == datatype)
        return;
    datatype_ = datatype;
    emit datatypeChanged();
    scheduleRepopulation();
}

// only coordinate datatype for now to get started with,
// refactor if more usecases for the model emerge.
void QDeclarativeLocationTestModel::repopulate()
{
    double latitude = 0.0;
    double longitude = 1.0;
    beginResetModel();
    if (!dataobjects_.isEmpty()) {
        qDeleteAll(dataobjects_);
        dataobjects_.clear();
    }
    for (int i = 0; i < datacount_; ++i) {
        DataObject* dataobject = new DataObject;
        QDeclarativeCoordinate* coordinate = new QDeclarativeCoordinate(QGeoCoordinate(latitude,longitude));
        dataobject->coordinate_ = coordinate;
        dataobjects_.append(dataobject);
        longitude += 1.0;
        latitude += 1.0;
    }
    endResetModel();
}

void QDeclarativeLocationTestModel::reset()
{
    beginResetModel();
    if (!dataobjects_.isEmpty()) {
        qDeleteAll(dataobjects_);
        dataobjects_.clear();
    }
    endResetModel();
}

void QDeclarativeLocationTestModel::scheduleRepopulation()
{
    if (!componentCompleted_)
        return;
    if (timer_.isActive())
        timer_.stop();
    if (delay_ > 0)
        timer_.start(delay_);
    else
        repopulate();
}

int QDeclarativeLocationTestModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return dataobjects_.count();
}

// Returns the stored under the given role for the item referred to by the index.
QVariant QDeclarativeLocationTestModel::data(const QModelIndex& index, int role) const
{
    switch (role) {
    case TestDataRole:
        if (dataobjects_.at(index.row())) {
            return QVariant::fromValue(qobject_cast<QObject*>(dataobjects_.at(index.row())));
        }
        break;
    }
    return QVariant();
}
