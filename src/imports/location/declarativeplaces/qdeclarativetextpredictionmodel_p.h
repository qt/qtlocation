#ifndef QDECLARATIVETEXTPREDICTIONMODEL_P_H
#define QDECLARATIVETEXTPREDICTIONMODEL_P_H

#include <QObject>
#include <QAbstractListModel>

#include <qplacesearchrequest.h>
#include <qplacemanager.h>
#include <qplacetextpredictionreply.h>

#include "qdeclarativecoordinate_p.h"
#include "qdeclarativegeoboundingbox_p.h"

QT_BEGIN_NAMESPACE

class QDeclarativeGeoServiceProvider;
class QGeoServiceProvider;

class QDeclarativeTextPredictionModel : public QAbstractListModel, public QDeclarativeParserStatus
{
    Q_OBJECT

    Q_INTERFACES(QDeclarativeParserStatus)

    Q_PROPERTY(QDeclarativeGeoServiceProvider *plugin READ plugin WRITE setPlugin NOTIFY pluginChanged)
    Q_PROPERTY(QString searchTerm READ searchTerm WRITE setSearchTerm NOTIFY searchTermChanged);
    Q_PROPERTY(QDeclarativeGeoBoundingArea* searchArea READ searchArea WRITE setSearchArea NOTIFY searchAreaChanged);
    Q_PROPERTY(int offset READ offset WRITE setOffset NOTIFY offsetChanged);
    Q_PROPERTY(int limit READ limit WRITE setLimit NOTIFY limitChanged);
    Q_PROPERTY(QStringList predictions READ predictions NOTIFY predictionsChanged)

public:
    explicit QDeclarativeTextPredictionModel(QObject *parent = 0);
    ~QDeclarativeTextPredictionModel();

    void setPlugin(QDeclarativeGeoServiceProvider *plugin);
    QDeclarativeGeoServiceProvider* plugin() const;

    QStringList getSuggestions() const;

    // From QAbstractListModel
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    enum Roles {
        TextPredictionRole = Qt::UserRole + 500
    };

    QString searchTerm() const;
    void setSearchTerm(const QString &searchTerm);
    QDeclarativeGeoBoundingArea *searchArea() const;
    void setSearchArea(QDeclarativeGeoBoundingArea *area);
    int offset() const;
    void setOffset(const int &offset);
    int limit() const;
    void setLimit(const int &limit);

    QStringList predictions() const;

    Q_INVOKABLE void executeQuery();
    Q_INVOKABLE void cancelRequest();

    void classBegin();
    void componentComplete();

signals:
    void pluginChanged();

    void queryFinished(const int &error);

    void searchTermChanged();
    void searchAreaChanged();
    void offsetChanged();
    void limitChanged();
    void predictionsChanged();

private slots:
    void replyFinished();
    void replyError(QPlaceReply::Error error, const QString &errorString);

private:
    void resetModel();
    void cancelPreviousRequest();
    void connectNewResponse(QPlaceTextPredictionReply *newResponse);

private:
    QDeclarativeGeoBoundingArea *m_searchArea;
    QPlaceSearchRequest m_queryParameters;

    QPlaceTextPredictionReply *m_response;

    QDeclarativeGeoServiceProvider *m_plugin;
    bool m_complete;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeTextPredictionModel));

#endif // QDECLARATIVETEXTPREDICTIONMODEL_P_H
