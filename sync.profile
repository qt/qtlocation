%modules = ( # path to module name map
    "QtLocation" => "$basedir/src/location",
);
%moduleheaders = ( # restrict the module headers to those found in relative path
);
%classnames = (
    "qtlocationversion.h" => "QtLocationVersion",
);
%mastercontent = (
    "gui" => "#include <QtGui/QtGui>\n",
    "network" => "#include <QtNetwork/QtNetwork>\n",
    "declarative" => "#include <QtDeclarative/qdeclarative.h>\n",
);
%modulepris = (
    "QtLocation" => "$basedir/modules/qt_location.pri",
);
# Module dependencies.
# Every module that is required to build this module should have one entry.
# Each of the module version specifiers can take one of the following values:
#   - A specific Git revision.
#   - any git symbolic ref resolvable from the module's repository (e.g. "refs/heads/master" to track master branch)
#
%dependencies = (
    "qtbase" => "refs/heads/master",
    "qtscript" => "refs/heads/master",
    "qtsvg" => "refs/heads/master",
    "qtxmlpatterns" => "refs/heads/master",
    "qtdeclarative" => "refs/heads/master",
);
