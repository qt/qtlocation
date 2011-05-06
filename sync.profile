%modules = ( # path to module name map
    "QtLocation" => "$basedir/src/location",
);
%moduleheaders = ( # restrict the module headers to those found in relative path
);
%classnames = (
);
%mastercontent = (
    "gui" => "#include <QtGui/QtGui>\n",
    "network" => "#include <QtNetwork/QtNetwork>\n",
    "sql" => "#include <QtSql/QtSql>\n",
);
%modulepris = (
    "QtLocation" => "$basedir/modules/qt_location.pri",
);
# Modules and programs, and their dependencies.
# Each of the module version specifiers can take one of the following values:
#   - A specific Git revision.
#   - "LATEST_REVISION", to always test against the latest revision.
#   - "LATEST_RELEASE", to always test against the latest public release.
#   - "THIS_REPOSITORY", to indicate that the module is in this repository.
%dependencies = (
    "QtLocation" => {
        "QtGui" => "0c637cb07ba3c9b353e7e483a209537485cc4e2a",
        "QtNetwork" => "0c637cb07ba3c9b353e7e483a209537485cc4e2a",
        "QtSql" => "0c637cb07ba3c9b353e7e483a209537485cc4e2a",
        "QtCore" => "0c637cb07ba3c9b353e7e483a209537485cc4e2a",
    },
);
