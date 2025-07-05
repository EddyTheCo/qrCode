#include <esterv/utils/qrcode_gen.hpp>
#include <sstream>
#include <string>
namespace Esterv::Utils::QrGen {

auto toSvgString(const QrCode &qr, std::string fill) -> std::string {

  std::ostringstream stream_buffer;
  stream_buffer << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  stream_buffer << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" "
        "\"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n";
  stream_buffer << R"(<svg xmlns="http://www.w3.org/2000/svg" version="1.1" viewBox="0 0 )";
  stream_buffer << (qr.getSize()) << " " << (qr.getSize()) << "\" stroke=\"none\">\n";
  stream_buffer << "\t<rect width=\"100%\" height=\"100%\" fill=\"none\"/>\n";
  stream_buffer << "\t<path d=\"";
  for (int y = 0; y < qr.getSize(); y++) {
    for (int x = 0; x < qr.getSize(); x++) {
      if (qr.getModule(x, y)) {
        if (x != 0 || y != 0) {
          stream_buffer << " ";
        }
        stream_buffer << "M" << (x) << "," << (y) << "h1v1h-1z";
      }
    }
  }
  stream_buffer << "\" fill=\"" << fill << "\"/>\n";
  stream_buffer << "</svg>\n";
  return stream_buffer.str();
}
} // namespace Esterv::Utils::QrGen
