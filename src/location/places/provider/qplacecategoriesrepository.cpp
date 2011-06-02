#include "qplacecategoriesrepository.h"
#include "qplacejsoncategoriesparser.h"

using namespace QT_PLACES_NAMESPACE;

static const char *supportedCategories = "{\"categories\":{\"category\":[{\"categorySystemName\":\"find-places\",\"displayName\":\"Accommodation\",\"language\":\"en\",\"name\":\"accommodation\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Administrative Areas & Buildings\",\"language\":\"en\",\"name\":\"administrative-areas-buildings\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Business & Services\",\"language\":\"en\",\"name\":\"business-services\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Eat & Drink\",\"language\":\"en\",\"name\":\"eat-drink\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Facilities\",\"language\":\"en\",\"name\":\"facilities\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Going Out\",\"language\":\"en\",\"name\":\"going-out\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Leisure & Outdoor\",\"language\":\"en\",\"name\":\"leisure-outdoor\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Natural & Geographical\",\"language\":\"en\",\"name\":\"natural-geographical\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Shopping\",\"language\":\"en\",\"name\":\"shopping\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Sights & Museums\",\"language\":\"en\",\"name\":\"sights-museums\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Public Toilet \\/ Rest Area\",\"language\":\"en\",\"name\":\"toilet-rest-area\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Transport\",\"language\":\"en\",\"name\":\"transport\"}],\"group\":[{\"category\":[{\"categorySystemName\":\"find-places\",\"displayName\":\"Administrative Region\",\"language\":\"en\",\"name\":\"administrative-region\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Building\",\"language\":\"en\",\"name\":\"building\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"City, Town or Village\",\"language\":\"en\",\"name\":\"city-town-village\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Outdoor Area \\/ Complex\",\"language\":\"en\",\"name\":\"outdoor-area-complex\"}],\"groupingCategory\":{\"displayName\":\"Administrative Areas & Buildings\",\"language\":\"en\",\"name\":\"administrative-areas-buildings\"}},{\"category\":[{\"categorySystemName\":\"find-places\",\"displayName\":\"Airport\",\"language\":\"en\",\"name\":\"airport\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Ferry Terminal\",\"language\":\"en\",\"name\":\"ferry-terminal\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Public Transport\",\"language\":\"en\",\"name\":\"public-transport\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Train Station\",\"language\":\"en\",\"name\":\"railway-station\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Taxi Stand\",\"language\":\"en\",\"name\":\"taxi-stand\"}],\"groupingCategory\":{\"displayName\":\"Transport\",\"language\":\"en\",\"name\":\"transport\"}},{\"category\":[{\"categorySystemName\":\"find-places\",\"displayName\":\"Amusement or Holiday Park\",\"language\":\"en\",\"name\":\"amusement-holiday-park\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Recreation\",\"language\":\"en\",\"name\":\"recreation\"}],\"groupingCategory\":{\"displayName\":\"Leisure & Outdoor\",\"language\":\"en\",\"name\":\"leisure-outdoor\"}},{\"category\":[{\"categorySystemName\":\"find-places\",\"displayName\":\"ATM \\/ Bank Exchange\",\"language\":\"en\",\"name\":\"atm-bank-exchange\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Business \\/ Industry\",\"language\":\"en\",\"name\":\"business-industry\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Car Dealer \\/ Repair\",\"language\":\"en\",\"name\":\"car-dealer-repair\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Car Rental\",\"language\":\"en\",\"name\":\"car-rental\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Communication \\/ Media\",\"language\":\"en\",\"name\":\"communication-media\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Petrol Station\",\"language\":\"en\",\"name\":\"petrol-station\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Police \\/ Emergency\",\"language\":\"en\",\"name\":\"police-emergency\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Post Office\",\"language\":\"en\",\"name\":\"post-office\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Service\",\"language\":\"en\",\"name\":\"service\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Tourist Information\",\"language\":\"en\",\"name\":\"tourist-information\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Travel Agency\",\"language\":\"en\",\"name\":\"travel-agency\"}],\"groupingCategory\":{\"displayName\":\"Business & Services\",\"language\":\"en\",\"name\":\"business-services\"}},{\"category\":[{\"categorySystemName\":\"find-places\",\"displayName\":\"Bar \\/ Pub\",\"language\":\"en\",\"name\":\"bar-pub\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Coffee \\/ Tea\",\"language\":\"en\",\"name\":\"coffee-tea\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Restaurant\",\"language\":\"en\",\"name\":\"restaurant\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Snacks \\/ Fast food\",\"language\":\"en\",\"name\":\"snacks-fast-food\"}],\"groupingCategory\":{\"displayName\":\"Eat & Drink\",\"language\":\"en\",\"name\":\"eat-drink\"}},{\"category\":[{\"categorySystemName\":\"find-places\",\"displayName\":\"Body of Water\",\"language\":\"en\",\"name\":\"body-of-water\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Forest, Heath or Other Vegetation\",\"language\":\"en\",\"name\":\"forest-heath-vegetation\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Mountain or Hill\",\"language\":\"en\",\"name\":\"mountain-hill\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Undersea Feature\",\"language\":\"en\",\"name\":\"undersea-feature\"}],\"groupingCategory\":{\"displayName\":\"Natural & Geographical\",\"language\":\"en\",\"name\":\"natural-geographical\"}},{\"category\":[{\"categorySystemName\":\"find-places\",\"displayName\":\"Bookshop\",\"language\":\"en\",\"name\":\"bookshop\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Clothing & Accessories\",\"language\":\"en\",\"name\":\"clothing-accessories-shop\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Department Store\",\"language\":\"en\",\"name\":\"department-store\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Electronics\",\"language\":\"en\",\"name\":\"electronics-shop\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Food & Drink\",\"language\":\"en\",\"name\":\"food-drink\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Hardware, House & Garden\",\"language\":\"en\",\"name\":\"hardware-house-garden-shop\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Kiosk \\/ 24\\/7 \\/ Convenience Store\",\"language\":\"en\",\"name\":\"kiosk-convenience-store\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Mall\",\"language\":\"en\",\"name\":\"mall\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Pharmacy\",\"language\":\"en\",\"name\":\"pharmacy\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Sport & Outdoor\",\"language\":\"en\",\"name\":\"sport-outdoor-shop\"}],\"groupingCategory\":{\"displayName\":\"Shopping\",\"language\":\"en\",\"name\":\"shopping\"}},{\"category\":[{\"categorySystemName\":\"find-places\",\"displayName\":\"Camping\",\"language\":\"en\",\"name\":\"camping\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Hostel\",\"language\":\"en\",\"name\":\"hostel\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Hotel\",\"language\":\"en\",\"name\":\"hotel\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Motel\",\"language\":\"en\",\"name\":\"motel\"}],\"groupingCategory\":{\"displayName\":\"Accommodation\",\"language\":\"en\",\"name\":\"accommodation\"}},{\"category\":[{\"categorySystemName\":\"find-places\",\"displayName\":\"Casino\",\"language\":\"en\",\"name\":\"casino\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Movie Theater\",\"language\":\"en\",\"name\":\"cinema\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Dance or Night Club\",\"language\":\"en\",\"name\":\"dance-night-club\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Theater, Music & Culture\",\"language\":\"en\",\"name\":\"theatre-music-culture\"}],\"groupingCategory\":{\"displayName\":\"Going Out\",\"language\":\"en\",\"name\":\"going-out\"}},{\"category\":[{\"categorySystemName\":\"find-places\",\"displayName\":\"Education Facility\",\"language\":\"en\",\"name\":\"education-facility\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Fair & Convention Facility\",\"language\":\"en\",\"name\":\"fair-convention-facility\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Government or Community Facility\",\"language\":\"en\",\"name\":\"government-community-facility\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Hospital or Health Care Facility\",\"language\":\"en\",\"name\":\"hospital-health-care-facility\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Library\",\"language\":\"en\",\"name\":\"library\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Parking Facility\",\"language\":\"en\",\"name\":\"parking-facility\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Sports Facility \\/ Venue\",\"language\":\"en\",\"name\":\"sports-facility-venue\"}],\"groupingCategory\":{\"displayName\":\"Facilities\",\"language\":\"en\",\"name\":\"facilities\"}},{\"category\":[{\"categorySystemName\":\"find-places\",\"displayName\":\"Landmark \\/ Attraction\",\"language\":\"en\",\"name\":\"landmark-attraction\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Museum\",\"language\":\"en\",\"name\":\"museum\"},{\"categorySystemName\":\"find-places\",\"displayName\":\"Religious Place\",\"language\":\"en\",\"name\":\"religious-place\"}],\"groupingCategory\":{\"displayName\":\"Sights & Museums\",\"language\":\"en\",\"name\":\"sights-museums\"}}]}}";

static QMap<QString, QString> categoriesMap;
static QPlaceCategory *secondSearchCenter = NULL;

QPlaceCategoriesRepository *QPlaceCategoriesRepository::repositoryInstance = NULL;

QPlaceCategoriesRepository *QPlaceCategoriesRepository::instance()
{
    if (!repositoryInstance) {
        repositoryInstance = new QPlaceCategoriesRepository();
    }
    return repositoryInstance;
}

QPlaceCategoriesRepository::QPlaceCategoriesRepository(QObject *parent)
    : QObject(parent),
      categoriesParser(NULL)
{
    setupCategoriesMapper();

    if (!categoriesParser) {
        categoriesParser = new QPlaceJSonCategoriesParser(this);
    }
    connect(categoriesParser, SIGNAL(finished(const QPlaceJSonCategoriesParser::Error &error, const QString &errorMessage)),
            this, SLOT(categoriesReady(const QPlaceJSonCategoriesParser::Error &error, const QString &errorMessage)));
    categoriesParser->processData(supportedCategories);
}

QPlaceCategoriesRepository::~QPlaceCategoriesRepository()
{
}

QPlaceReply *QPlaceCategoriesRepository::initializeCategories(const QString &categorySystemId)
{
    // to be implemented
    return NULL;
}

QList<QPlaceCategory> QPlaceCategoriesRepository::categories() const
{
    return allCategories.values();
}

QPlaceCategory QPlaceCategoriesRepository::mapCategory(const QString &number)
{
    QString categoryId = categoriesMap.value(number, "");
    if (categoryId == "second-search-center") {
        if (!secondSearchCenter) {
            secondSearchCenter = new QPlaceCategory();
            secondSearchCenter->setCategoryId("second-search-center");
        }
        return *secondSearchCenter;
    } else if (!categoryId.isEmpty()) {
        return findCategoryById(categoryId);
    }
    return QPlaceCategory();
}

QString QPlaceCategoriesRepository::getCategoryTagId(const QPlaceCategory &category)
{
    return categoriesMap.key(category.categoryId(), QString());
}

QPlaceCategory QPlaceCategoriesRepository::findCategoryById(const QString &id)
{
    return allCategories.value(id, QPlaceCategory());
}

void QPlaceCategoriesRepository::categoriesReady(const QPlaceJSonCategoriesParser::Error &error, const QString &errorMessage)
{
    Q_UNUSED(errorMessage);

    if (!error) {
        foreach (QPlaceCategory cat, categoriesParser->resultCategories()) {
            allCategories.insert(cat.categoryId(), cat);
        }
    }
    delete categoriesParser;
    categoriesParser = NULL;
}

void QPlaceCategoriesRepository::setupCategoriesMapper()
{
    // setup mapper
    if (!categoriesMap.count()) {
        //BusinessServices
        categoriesMap.insert(QString("9000269"),QString("business-services"));
        categoriesMap.insert(QString("9000005"),QString("business-industry"));
        categoriesMap.insert(QString("9000277"),QString("travel-agency"));
        categoriesMap.insert(QString("9000028"),QString("tourist-information"));
        categoriesMap.insert(QString("9000040"),QString("police-emergency"));
        categoriesMap.insert(QString("9000017"),QString("petrol-station"));
        categoriesMap.insert(QString("9000278"),QString("communication-media"));
        categoriesMap.insert(QString("9000047"),QString("atm-bank-exchange"));
        categoriesMap.insert(QString("9000002"),QString("car-dealer-repair"));
        categoriesMap.insert(QString("9000020"),QString("car-rental"));
        categoriesMap.insert(QString("9000215"),QString("service"));
        categoriesMap.insert(QString("9000019"),QString("post-office"));
        // Transport
        categoriesMap.insert(QString("9000272"),QString("transport"));
        categoriesMap.insert(QString("9000216"),QString("taxi-stand"));
        categoriesMap.insert(QString("9000043"),QString("airport"));
        categoriesMap.insert(QString("9000041"),QString("railway-station"));
        categoriesMap.insert(QString("9000058"),QString("public-transport"));
        categoriesMap.insert(QString("9000035"),QString("ferry-terminal"));
        // Shopping
        categoriesMap.insert(QString("9000270"),QString("shopping"));
        categoriesMap.insert(QString("9000205"),QString("clothing-accessories-shop"));
        categoriesMap.insert(QString("9000276"),QString("bookshop"));
        categoriesMap.insert(QString("9000194"),QString("hardware-house-garden-shop"));
        categoriesMap.insert(QString("9000049"),QString("pharmacy"));
        categoriesMap.insert(QString("9000024"),QString("mall"));
        categoriesMap.insert(QString("9000197"),QString("kiosk-convenience-store"));
        categoriesMap.insert(QString("9000196"),QString("electronics-shop"));
        categoriesMap.insert(QString("9000191"),QString("sport-outdoor-shop"));
        categoriesMap.insert(QString("9000189"),QString("department-store"));
        // Accomodation
        categoriesMap.insert(QString("9000271"),QString("accommodation"));
        categoriesMap.insert(QString("9000032"),QString("camping"));
        categoriesMap.insert(QString("9000174"),QString("motel"));
        categoriesMap.insert(QString("9000038"),QString("hotel"));
        categoriesMap.insert(QString("9000173"),QString("hostel"));
        // GoingOut
        categoriesMap.insert(QString("9000274"),QString("going-out"));
        categoriesMap.insert(QString("9000203"),QString("dance-night-club"));
        categoriesMap.insert(QString("9000003"),QString("casino"));
        categoriesMap.insert(QString("9000004"),QString("cinema"));
        categoriesMap.insert(QString("9000181"),QString("theatre-music-culture"));
        // Facility
        categoriesMap.insert(QString("9000261"),QString("facilities"));
        categoriesMap.insert(QString("9000039"),QString("parking-facility"));
        categoriesMap.insert(QString("9000007"),QString("fair-convention-facility"));
        categoriesMap.insert(QString("9000012"),QString("government-community-facility"));
        categoriesMap.insert(QString("9000200"),QString("hospital-health-care-facility"));
        categoriesMap.insert(QString("9000106"),QString("education-facility"));
        categoriesMap.insert(QString("9000220"),QString("sports-facility-venue"));
        categoriesMap.insert(QString("9000031"),QString("library"));
        // NatureGeography
        categoriesMap.insert(QString("9000265"),QString("natural-geographical"));
        categoriesMap.insert(QString("9000262"),QString("body-of-water"));
        categoriesMap.insert(QString("9000045"),QString("mountain-hill"));
        categoriesMap.insert(QString("9000259"),QString("forest-health-vegetation"));
        categoriesMap.insert(QString("9000260"),QString("undersea-feature"));
        // AreasInfrastructure
        categoriesMap.insert(QString("9000266"),QString("administrative-areas-buildings"));
        categoriesMap.insert(QString("9000279"),QString("administrative-region"));
        categoriesMap.insert(QString("9000263"),QString("outdoor-area-complex"));
        categoriesMap.insert(QString("9000283"),QString("city-town-village"));
        categoriesMap.insert(QString("9000280"),QString("building"));
        // EatDrink
        categoriesMap.insert(QString("9000275"),QString("eat-drink"));
        categoriesMap.insert(QString("9000063"),QString("coffee-tea"));
        categoriesMap.insert(QString("9000022"),QString("restaurant"));
        categoriesMap.insert(QString("9000064"),QString("snacks-fast-food"));
        categoriesMap.insert(QString("9000033"),QString("bar-pub"));
        categoriesMap.insert(QString("9000143"),QString("food-drink"));
        // Leisure
        categoriesMap.insert(QString("9000267"),QString("leisure-outdoor"));
        categoriesMap.insert(QString("9000048"),QString("recreation"));
        categoriesMap.insert(QString("9000001"),QString("amusement-holiday-park"));
        // SightsMuseums
        categoriesMap.insert(QString("9000273"),QString("sights-museums"));
        categoriesMap.insert(QString("9000014"),QString("museum"));
        categoriesMap.insert(QString("9000158"),QString("religious-place"));
        categoriesMap.insert(QString("9000211"),QString("landmark-attraction"));
        // SSC
        categoriesMap.insert(QString("9000282"),QString("second-search-center"));
    }
}
