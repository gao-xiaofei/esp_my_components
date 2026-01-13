# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

set(COMPONENTS "main"
    "nvs_flash"
    "unity"
    "doso_driver_gpio"
    "doso_driver_uart"
    )

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp_my_components)
