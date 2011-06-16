#ifndef QPLACECATEGORIESREPOSITORY_H
#define QPLACECATEGORIESREPOSITORY_H

#include <QObject>
#include <QList>

#include <qmobilityglobal.h>
#include <qplacecategory.h>
#include <qplacereply.h>
#include "qplacejsoncategoriesparser.h"

QTM_BEGIN_NAMESPACE

class QPlaceCategoriesRepository : public QObject
{
    Q_OBJECT
public:
    static QPlaceCategoriesRepository *instance();

    ~QPlaceCategoriesRepository();

    QPlaceReply *initializeCategories();
    QList<QPlaceCategory> categories() const;

    QPlaceCategory mapCategory(const QString &number);
    QString getCategoryTagId(const QPlaceCategory &category);
    QPlaceCategory findCategoryById(const QString &id);

public slots:
    void categoriesReady(const QPlaceJSonCategoriesParser::Error &error, const QString &errorMessage);

private:
    void setupCategoriesMapper();

private:
    QPlaceCategoriesRepository(QObject *parent = 0);

    QHash<QString, QPlaceCategory> allCategories;
    static QPlaceCategoriesRepository *repositoryInstance;
    QPlaceJSonCategoriesParser *categoriesParser;
};

QTM_END_NAMESPACE

#endif // QPLACECATEGORIESREPOSITORY_H
