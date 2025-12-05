# vcpkg Setup Instructions
  * Install vcpkg.
  * Open QtCreator.
  * Navigate to "Help" => "About Plugins..."
  * Under "Utilities", enable vcpkg.
    * If vcpkg is not present, update to a newer version of QtCreator.
  * Navigate to "Edit" => "Preferences..." => "CMake" => "Vcpkg"
  * Ensure the path to vcpkg is set correctly

## Set the Triplet for Dynamic Linking

## Linux
Under the kit configuration, set the value to the appropriate triplet for dynamic linking, e.g. "VCPKG_DEFAULT_TRIPLET=x64-linux-dynamic" for an x64 system.

## Windows
QtCreator 18.0.0 persistently overwrites the kit configuration value of "VCPKG_DEFAULT_TRIPLET" using the environment variable. I have not found a solution other than changing this variable's value to "x64-mingw-dynamic" whenver you want to build.