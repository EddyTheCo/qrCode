#include <esterv/utils/qrcode_dec.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/mat.hpp>
#include <string>

namespace Esterv::Utils::QrDec {

auto QRDecoder::decodeGrey(unsigned char *img, int rows, int cols) -> std::string {
  cv::Mat greyImg = cv::Mat(rows, cols, CV_8UC1, img);
  auto str = detectAndDecode(greyImg);
  if (!str.empty()) {
    return str;
  }
  cv::bitwise_not(greyImg, greyImg);
  return detectAndDecode(greyImg);
}

} // namespace Esterv::Utils::QrDec
