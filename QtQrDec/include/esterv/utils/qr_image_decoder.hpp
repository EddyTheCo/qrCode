#include <QBuffer>
#include <QImage>
#include <QObject>  
#include <QString>


#include <cstdint>
#include <memory>
#include <qjsengine.h>
#include <qqmlengine.h>
#include <qqmlintegration.h>
#include <qquickimageprovider.h>
#include <qtdeprecationdefinitions.h>
#include <qtmetamacros.h>

#ifndef USE_EMSCRIPTEN
#include <QCamera>
#include <QCameraDevice>
#include <QMediaCaptureSession>
#include <QMediaDevices>
#include <QVideoSink>
#include <condition_variable>
#include <mutex>
#endif

#include <esterv/utils/qrcode_dec.hpp>

#if defined(QTQRDEC_SHARED)
#include <QtCore/QtGlobal>
#ifdef WINDOWS_EXPORT
#define DEC_EXPORT Q_DECL_EXPORT
#else
#define DEC_EXPORT Q_DECL_IMPORT
#endif
#else
#define DEC_EXPORT
#endif

namespace Esterv::Utils::QrDec {
class DEC_EXPORT QRImageDecoder : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString source READ get_source NOTIFY sourceChanged)
  Q_PROPERTY(bool useTorch MEMBER m_useTorch NOTIFY useTorchChanged)
  Q_PROPERTY(bool hasTorch MEMBER m_hasTorch NOTIFY hasTorchChanged)
  QML_ELEMENT
  QML_SINGLETON

  explicit QRImageDecoder(QObject *parent = nullptr);

public:
  ~QRImageDecoder() override {
    {
      const std::lock_guard k_lock(decoding_mutex_);
      decode_running_ = false;
    }
    decoding_variable_.notify_one();
  }
  static auto instance() -> QRImageDecoder *;
  static auto create(QQmlEngine * /*qmlEngine*/, QJSEngine * /*jsEngine*/)-> QRImageDecoder * {
    return instance();
  } 
  enum class State : std::uint8_t { Decoding = 0, Ready };
  Q_INVOKABLE void start();
  Q_INVOKABLE void stop();
  Q_INVOKABLE void clear();
  [[nodiscard]] auto get_source() const { return source_; }

  void reload(int offset, int width, int height);
Q_SIGNALS:
  void decodedQR(QString);
  void sourceChanged();
  void hasTorchChanged();
  void useTorchChanged();

private:
  State state_{State::Ready};
  std::mutex decoding_mutex_;
  std::condition_variable decoding_variable_;
  bool decode_running_{true};
#ifndef USE_EMSCRIPTEN
  std::unique_ptr<QCamera> camera_;
  std::unique_ptr<QMediaCaptureSession> capture_session_;
  std::unique_ptr<QVideoSink> video_sink_;
  void getCamera();
#endif
  void setid();
  void decodePicture();
  QString source_;
  QRDecoder detector_;
  bool use_torch_{false}, has_torch_{false};
};

class DEC_EXPORT WasmImageProvider : public QQuickImageProvider {
public:
  WasmImageProvider() : QQuickImageProvider(QQuickImageProvider::Image) {
    restart();
  }
  auto requestImage(const QString & /*id*/, QSize * /*size*/, const QSize & /*requestedSize*/)->QImage override;
  static void restart();
  static QImage img;
};
} // namespace Esterv::Utils::QrDec
