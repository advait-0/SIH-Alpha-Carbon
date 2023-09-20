# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/aryan/esp/esp-idf/components/bootloader/subproject"
  "/home/aryan/SIH_ESP_NOW/MAC_ADDRESS/build/bootloader"
  "/home/aryan/SIH_ESP_NOW/MAC_ADDRESS/build/bootloader-prefix"
  "/home/aryan/SIH_ESP_NOW/MAC_ADDRESS/build/bootloader-prefix/tmp"
  "/home/aryan/SIH_ESP_NOW/MAC_ADDRESS/build/bootloader-prefix/src/bootloader-stamp"
  "/home/aryan/SIH_ESP_NOW/MAC_ADDRESS/build/bootloader-prefix/src"
  "/home/aryan/SIH_ESP_NOW/MAC_ADDRESS/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/aryan/SIH_ESP_NOW/MAC_ADDRESS/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/aryan/SIH_ESP_NOW/MAC_ADDRESS/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
