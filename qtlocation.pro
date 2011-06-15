TEMPLATE = subdirs

module_qtlocation_src.subdir = src
module_qtlocation_src.target = module-qtlocation-src

module_qtlocation_demos.subdir = demos
module_qtlocation_demos.target = module-qtlocation-demos
module_qtlocation_demos.depends = module_qtlocation_src
!contains(QT_BUILD_PARTS,demos) {
    module_qtlocation_demos.CONFIG += no_default_install no_default_target
}

module_qtlocation_examples.subdir = examples/declarative
module_qtlocation_examples.target = module-qtlocation-examples
module_qtlocation_examples.depends = module_qtlocation_src
!contains(QT_BUILD_PARTS,examples) {
    module_qtlocation_examples.CONFIG += no_default_install no_default_target
}

module_qtlocation_tests.subdir = tests
module_qtlocation_tests.target = module-qtlocation-tests
module_qtlocation_tests.depends = module_qtlocation_src
!contains(QT_BUILD_PARTS,tests) {
    module_qtlocation_tests.CONFIG = no_default_target no_default_install
}


SUBDIRS += module_qtlocation_src \
           module_qtlocation_demos \
           module_qtlocation_examples \
           module_qtlocation_tests \
