if(EMSCRIPTEN)
  find_package(Qt6 COMPONENTS Core Gui Qml Quick ShaderTools)
else()
  find_package(Qt6 COMPONENTS Core Gui Qml Quick ShaderTools Multimedia)
endif(EMSCRIPTEN)

if(Qt6_FOUND AND TARGET QrDec)
  qt_policy(SET QTP0004 NEW)

  FetchContent_Declare(
    EstervDesigns
    GIT_REPOSITORY https://github.com/EddyTheCo/Esterv.Designs.git
    GIT_TAG improvements 
    FIND_PACKAGE_ARGS 2 COMPONENTS SimpleStyle CustomControls CONFIG)
  FetchContent_MakeAvailable(EstervDesigns)
  qt_standard_project_setup()
  qt6_add_qml_module(
    QtQrDec
    URI
    Esterv.CustomControls.QrDec
    VERSION
    1.0
    SOURCES
    src/qr_image_decoder.cpp
    include/esterv/utils/qr_image_decoder.hpp
    QML_FILES
    "qml/QrCam.qml"
    "qml/QrDecPop.qml"
    "qml/QrTextField.qml"
    RESOURCE_PREFIX
    "/esterVtech.com/imports"
    OUTPUT_TARGETS
    out_targets_var
    OUTPUT_DIRECTORY
    ${CMAKE_BINARY_DIR}/Esterv/CustomControls/QrDec
    IMPORT_PATH
    ${CMAKE_BINARY_DIR})

  add_library(Esterv::QtQrDec ALIAS QtQrDec)
  add_library(Esterv::QtQrDecplugin ALIAS QtQrDecplugin)
  target_compile_definitions(
    QtQrDec
    PUBLIC
      $<IF:$<AND:$<BOOL:WIN32>,$<STREQUAL:$<TARGET_PROPERTY:QtQrDec,TYPE>,SHARED_LIBRARY>>,QTQRDEC_SHARED,>
  )
  target_compile_definitions(QtQrDec PRIVATE WINDOWS_EXPORT)
  set_target_properties(QtQrDec PROPERTIES VERSION ${VERSION} SOVERSION
                                                              ${VERSION_MAJOR})

  qt6_add_shaders(
    QtQrDec
    "esterVtech.com.imports.QtQrDec.shaders"
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
    QtQrDec PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                   "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
  target_include_directories(
    QtQrDec PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/esterv/utils)
  if(EMSCRIPTEN)
    target_compile_definitions(QtQrDec PUBLIC USE_EMSCRIPTEN)
  else()
    target_link_libraries(QtQrDec PUBLIC Qt6::Multimedia)
  endif(EMSCRIPTEN)
  target_link_libraries(
    QtQrDec
    PUBLIC
      Qt6::Gui
      Qt6::Quick
      QrDec
      Esterv::SimpleStyle
      Esterv::CustomControls
      $<$<STREQUAL:$<TARGET_PROPERTY:Esterv::SimpleStyle,TYPE>,STATIC_LIBRARY>:Esterv::SimpleStyleplugin>
      $<$<STREQUAL:$<TARGET_PROPERTY:Esterv::CustomControls,TYPE>,STATIC_LIBRARY>:Esterv::CustomControlsplugin>
  )

  install(
    TARGETS QtQrDec ${out_targets_var} ${out_targets_var2}
    EXPORT ${PROJECT_NAME}-config
    COMPONENT EstervQtQrDec
    ARCHIVE COMPONENT ${PROJECT_NAME}-dev)
  install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT ${PROJECT_NAME}-dev)
  install(
    DIRECTORY ${CMAKE_BINARY_DIR}/Esterv/CustomControls/QrDec
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/Esterv/CustomControls
    COMPONENT EstervQtQrDec-qml)

  install(
    TARGETS QtQrDecplugin
    EXPORT ${PROJECT_NAME}-config
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/Esterv/CustomControls/QrDec
    COMPONENT EstervQtQrDec-qml
    ARCHIVE COMPONENT ${PROJECT_NAME}-dev)

  if(EMSCRIPTEN)
    target_compile_definitions(QtQrDec PRIVATE USE_EMSCRIPTEN)
  endif(EMSCRIPTEN)
  if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    add_subdirectory(examples)
  endif(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
else(Qt6_FOUND AND TARGET QrDec)
  message(
    STATUS "The QML Module/Qt methods for QRCODE decoding will not be built")
endif(Qt6_FOUND AND TARGET QrDec)
