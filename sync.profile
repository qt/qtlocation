%modules = ( # path to module name map
    "QtLocation" => "$basedir/src/location",
);
%moduleheaders = ( # restrict the module headers to those found in relative path
);
# Module dependencies.
# Every module that is required to build this module should have one entry.
# Each of the module version specifiers can take one of the following values:
#   - A specific Git revision.
#   - any git symbolic ref resolvable from the module's repository (e.g. "refs/heads/master" to track master branch)
#
%dependencies = (
    "qtbase" => "refs/heads/stable",
    "qtsvg" => "refs/heads/stable",
    "qtxmlpatterns" => "refs/heads/stable",
    "qtdeclarative" => "refs/heads/stable",
    "qtjsbackend" => "refs/heads/stable",
    "qt3d" => "refs/heads/master",
    "qtmultimedia" => "refs/heads/stable"
);
