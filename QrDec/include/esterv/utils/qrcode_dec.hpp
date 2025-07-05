#pragma once
#include <opencv2/objdetect.hpp>
#include <string>

namespace Esterv::Utils::QrDec {
class QRDecoder : public cv::QRCodeDetectorAruco {

public:
  QRDecoder()= default;
  auto decodeGrey(unsigned char *img, int rows, int cols) -> std::string;
};
} // namespace Esterv::Utils::QrDec
