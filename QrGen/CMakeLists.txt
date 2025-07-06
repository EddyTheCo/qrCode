add_library(QrGen src/qrcode_gen.cpp src/utils.cpp include/esterv/utils/qrcode_gen.hpp)
add_library(Esterv::QrGen ALIAS QrGen)

set_target_properties(QrGen PROPERTIES POSITION_INDEPENDENT_CODE ON)

set_target_properties(QrGen PROPERTIES VERSION ${VERSION} SOVERSION
                                                          ${VERSION_MAJOR})

target_include_directories(
  QrGen PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
               "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

install(
  TARGETS QrGen
  EXPORT ${PROJECT_NAME}-config
  COMPONENT EstervQrGen
  ARCHIVE COMPONENT ${PROJECT_NAME}-dev)
install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  COMPONENT ${PROJECT_NAME}-dev)
