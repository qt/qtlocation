#ifndef QPLACECATEGORIESREPLYIMPL_H
#define QPLACECATEGORIESREPLYIMPL_H

#include <QObject>
#include <QList>

#include <qplacereply.h>
#include "qplacerestreply.h"
#include "qplacejsoncategoriesparser.h"

class QPlaceCategoriesReplyImpl : public QPlaceReply
{
    Q_OBJECT
public:
    explicit QPlaceCategoriesReplyImpl(QPlaceRestReply *reply, QObject *parent = 0);
    ~QPlaceCategoriesReplyImpl();

    QPlaceCategoryTree categories() const;
    QList<QPlaceCategory> categoriesFlat() const;

    void abort();

Q_SIGNALS:
    void processingFinished(QPlaceReply *reply);
    void processingError(QPlaceReply *reply, const QPlaceReply::Error &error, const QString &errorMessage);

private slots:
    void restError(QPlaceRestReply::Error error);
    void resultReady(const QPlaceJSonParser::Error &error,
                          const QString &errorMessage);

private:
    QPlaceRestReply *restReply;
    QPlaceJSonCategoriesParser *parser;

    QPlaceCategoryTree m_categoryTree;
};

#endif // QPLACECATEGORIESREPLYIMPL_H
