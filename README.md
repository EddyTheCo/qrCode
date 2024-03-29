# qrCode

[TOC]

This repository is intended to provide a library for working with QR codes from c++. The main purpose is to exploit modern CMake and facilitate reuse and develop.

The GUI part will be based on Qt libraries and QML. Examples of this library compiled for Web Assembly can be found on:
- [QtQrGen](https://eddytheco.github.io/qmlonline/?example_url=qt_qr_gen)
- [QtQrDec](https://eddytheco.github.io/qmlonline/?example_url=qt_qr_dec)

## Adding the libraries to your CMake project 

```CMake
include(FetchContent)
FetchContent_Declare(
	qrCode
	GIT_REPOSITORY https://github.com/EddyTheCo/qrCode.git
	GIT_TAG vMAJOR.MINOR.PATCH 
	FIND_PACKAGE_ARGS MAJOR.MINOR CONFIG  
	)
FetchContent_MakeAvailable(qrCode)

target_link_libraries(<target> <PRIVATE|PUBLIC|INTERFACE> qrCode::QrGen qrCode::QtQrGen qrCode::QrDec qrCode::QtQrDec)
```
For more information check

- [QrGen](QrGen/README.md)
- [QrDec](QrDec/README.md)
- [QtQrGen](QtQrGen/README.md)
- [QtQrDec](QtQrDec/README.md)

## API reference

You can read the [API reference](https://eddytheco.github.io/qrCode/) here, or generate it yourself like
```
cmake -DBUILD_DOCS=ON ../
cmake --build . --target doxygen_docs
```

## Contributing

We appreciate any contribution!


You can open an issue or request a feature.
You can open a PR to the `develop` branch and the CI/CD will take care of the rest.
Make sure to acknowledge your work, and ideas when contributing.

