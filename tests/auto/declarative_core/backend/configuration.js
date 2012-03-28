/* This file loads a fixed location Javascript file to configure the backend tests.

   The configuration file should expose these variables (optional ones  are in []:

   pluginName : string  # must be set to "nokia"
   appId      : string  # Application Id received during registration process at https://api.forum.nokia.com/ovi-api/ui/registration
   token      : string
   [proxy]    : string  # e.g. http://127.0.0.1:8081
*/

Qt.include("/var/lib/qt/qtlocation_tests_configuration.js")

