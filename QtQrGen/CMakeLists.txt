find_package(
  Qt6
  COMPONENTS Core Gui Qml Quick
  OPTIONAL_COMPONENTS Svg)

if(Qt6_FOUND AND TARGET QrGen)
  qt_policy(SET QTP0004 NEW)

  FetchContent_Declare(
    EstervDesigns
    GIT_REPOSITORY https://github.com/EddyTheCo/Esterv.Designs.git
    GIT_TAG improvements 
    FIND_PACKAGE_ARGS 2 COMPONENTS CustomControls SimpleStyle CONFIG)
  FetchContent_MakeAvailable(EstervDesigns)
  qt_standard_project_setup()
  qt6_add_qml_module(
    QtQrGen
    URI
    Esterv.CustomControls.QrGen
    VERSION
    ${VERSION}
    SOURCES
    src/qr_image_provider.cpp
    include/esterv/utils/qr_image_provider.hpp
    QML_FILES
    "qml/QrGenImage.qml"
    "qml/QrGenPop.qml"
    "qml/QrText.qml"
    RESOURCE_PREFIX
    "/esterVtech.com/imports"
    OUTPUT_TARGETS
    out_targets_var
    OUTPUT_DIRECTORY
    ${CMAKE_BINARY_DIR}/Esterv/CustomControls/QrGen
    IMPORT_PATH
    ${CMAKE_BINARY_DIR})

  add_library(Esterv::QtQrGen ALIAS QtQrGen)
  add_library(Esterv::QtQrGenplugin ALIAS QtQrGenplugin)
  target_compile_definitions(
    QtQrGen
    PUBLIC
      $<IF:$<AND:$<BOOL:WIN32>,$<STREQUAL:$<TARGET_PROPERTY:QtQrGen,TYPE>,SHARED_LIBRARY>>,QTQRGEN_SHARED,>
  )
  target_compile_definitions(QtQrGen PRIVATE WINDOWS_EXPORT)
  set_target_properties(QtQrGen PROPERTIES VERSION ${VERSION} SOVERSION
                                                              ${VERSION_MAJOR})

  qt6_add_shaders(
    QtQrGen
    "esterVtech.com.imports.QtQrGen.shaders"
    BATCHABLE
    PRECOMPILE
    OPTIMIZED
    OUTPUT_TARGETS
    out_targets_var2
    PREFIX
    "/esterVtech.com/imports/Designs"
    FILES
    "frag/qrscanner.frag")

  target_include_directories(
    QtQrGen PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                   "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/Esterv/>")
  target_include_directories(
    QtQrGen PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/esterv/utils)
  if(NOT TARGET Qt6::Svg)
    FetchContent_Declare(
      qtsvg
      GIT_REPOSITORY https://github.com/qt/qtsvg.git
      GIT_TAG 6.7.0)
    FetchContent_MakeAvailable(qtsvg)
  endif()

  target_link_libraries(QtQrGen PUBLIC Qt6::Quick)
  target_link_libraries(
    QtQrGen
    PRIVATE
      QrGen
      Qt6::Core
      Qt6::Gui
      Qt6::Qml
      Qt6::Svg
      Esterv::SimpleStyle
      Esterv::CustomControls
      $<$<STREQUAL:$<TARGET_PROPERTY:Esterv::SimpleStyle,TYPE>,STATIC_LIBRARY>:Esterv::SimpleStyleplugin>
      $<$<STREQUAL:$<TARGET_PROPERTY:Esterv::CustomControls,TYPE>,STATIC_LIBRARY>:Esterv::CustomControlsplugin>
  )

  install(
    TARGETS QtQrGen ${out_targets_var} ${out_targets_var2}
    EXPORT ${PROJECT_NAME}-config
    COMPONENT EstervQtQrGen
    ARCHIVE COMPONENT ${PROJECT_NAME}-dev)
  install(
    DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT ${PROJECT_NAME}-dev)
  install(
    DIRECTORY ${CMAKE_BINARY_DIR}/Esterv/CustomControls/QrGen
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/Esterv/CustomControls
    COMPONENT EstervQtQrGen-qml)

  install(
    TARGETS QtQrGenplugin
    EXPORT ${PROJECT_NAME}-config
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/Esterv/CustomControls/QrGen
    COMPONENT EstervQtQrGen-qml
    ARCHIVE COMPONENT ${PROJECT_NAME}-dev)
  if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    add_subdirectory(examples)
  endif(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
endif(Qt6_FOUND AND TARGET QrGen)
