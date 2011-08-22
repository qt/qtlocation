#include <QtCore/QString>
#include <QtTest/QtTest>

#include <qplacecontentrequest.h>

QT_USE_NAMESPACE

class tst_QPlaceContentRequest : public QObject
{
    Q_OBJECT

public:
    tst_QPlaceContentRequest();

private Q_SLOTS:
    void contentTest();
    void clearTest();
};

tst_QPlaceContentRequest::tst_QPlaceContentRequest()
{
}

void tst_QPlaceContentRequest::contentTest()
{
    QPlaceContentRequest req;
    QCOMPARE(req.limit(), -1);
    QCOMPARE(req.offset(), 0);
    QCOMPARE(req.contentType(), QPlaceContent::InvalidType);

    //check that we can set the request fields
    req.setLimit(100);
    req.setOffset(5);
    req.setContentType(QPlaceContent::ImageType);
    QCOMPARE(req.limit(), 100);
    QCOMPARE(req.offset(), 5);
    QCOMPARE(req.contentType(), QPlaceContent::ImageType);

    //check that we assignment works correctly
    QPlaceContentRequest otherReq;
    otherReq.setLimit(10);
    otherReq.setOffset(15);
    otherReq.setContentType(QPlaceContent::ReviewType);
    req = otherReq;
    QCOMPARE(req.limit(), 10);
    QCOMPARE(req.offset(), 15);
    QCOMPARE(req.contentType(), QPlaceContent::ReviewType);

    //check assigningment to a base class instance and comparison
    QPlaceRequest plainReq(otherReq);
    QVERIFY(req == otherReq);

    //check assignment from base class to derived class
    QPlaceContentRequest contentReq = plainReq;
    QVERIFY(contentReq == req);

   //check that comparison will fail if one the fields are different
    contentReq.setLimit(9000);
    QVERIFY(contentReq != req);
}

void tst_QPlaceContentRequest::clearTest()
{
    QPlaceContentRequest req;
    req.setContentType(QPlaceContent::ReviewType);
    req.setLimit(9000);
    req.setOffset(1);
    req.clear();
    QVERIFY(req.contentType() == QPlaceContent::InvalidType);
    QVERIFY(req.limit() == -1);
    QVERIFY(req.offset() == 0);
}

QTEST_APPLESS_MAIN(tst_QPlaceContentRequest);

#include "tst_qplacecontentrequest.moc"
