#include <esterv/utils/qr_image_decoder.hpp>
#include <QGuiApplication>
#include <QImage>
#include <QQuickImageProvider>
#include <memory>
#include <mutex>
#include <qcamera.h>
#include <qlist.h>
#include <qlogging.h>
#include <qmediadevices.h>
#include <qtconfigmacros.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include<QString>
#include<QObject>

#ifdef USE_EMSCRIPTEN

#include <emscripten.h>
#include <emscripten/bind.h>

EMSCRIPTEN_BINDINGS(qrdecoder) {
  emscripten::class_<Esterv::Utils::QrDec::QRImageDecoder>("QRImageDecoder")
      .function("reload", &Esterv::Utils::QrDec::QRImageDecoder::reload,
                emscripten::allow_raw_pointers())
      .class_function("instance",
                      &Esterv::Utils::QrDec::QRImageDecoder::instance,
                      emscripten::allow_raw_pointers());
}
// clang-format off
EM_JS(void, js_start, (), {
    if ('mediaDevices' in navigator && 'getUserMedia' in navigator.mediaDevices) {
        stream = navigator.mediaDevices.getUserMedia({video : {facingMode : 'environment'}, audio : false}).then((stream) => {
                      const settings = stream.getVideoTracks()[0].getSettings();
                      const width = settings.width;
                      const height = settings.height;

                      if (document.querySelector("#qrvideo") === null) {
                        let elemDiv = document.createElement('div');
                        elemDiv.style.cssText = 'display:none; position:absolute;width:100%;height:100%;';
                        elemDiv.innerHTML += '<video controls autoplay id="qrvideo" width="' +
                            width + 'px" height="' + height +
                            'px"></video><canvas id="qrcanvas" width="' +
                            width + 'px" height="' + height +
                            'px" ></canvas></div>';
                        document.body.appendChild(elemDiv);
                      }
                      const video = document.querySelector("#qrvideo");
                      video.srcObject = stream;
                      window.localStream = stream;

                      let canvas = document.querySelector("#qrcanvas");
                      let ctx = canvas.getContext("2d", { willReadFrequently: true });
                      const processFrame = function() {
                        ctx.drawImage(video, 0, 0, canvas.width, canvas.height);
                        const imageData = ctx.getImageData(0, 0, canvas.width, canvas.height);
                        const sourceBuffer = imageData.data;
                        const buffer = _malloc(sourceBuffer.byteLength);
                        HEAPU8.set(sourceBuffer, buffer);
                        Module.QRImageDecoder.instance().reload(buffer, video.width, video.height);
                        _free(buffer);
                        if (window.localStream.active) {
                          requestAnimationFrame(processFrame);
                        } else {
                          ctx.clearRect(0, 0, canvas.width, canvas.height);
                        }
                      };
                      processFrame();
                    })
            .catch(alert);
    }
});

EM_JS(void, js_stop, (), {
    if (window.localStream)
        window.localStream.getVideoTracks()[0].stop();
});
// clang-format on
namespace Esterv::Utils::QrDec {
#else
#include <thread>
#if QT_CONFIG(permissions)
#include <QPermission>
#endif

namespace Esterv::Utils::QrDec {

void QRImageDecoder::getCamera() {
  const QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
  if (!cameras.empty()) {
    QCameraDevice best = cameras.front();
    for (const QCameraDevice &camera_device : cameras) {

      if (camera_device.position() == QCameraDevice::BackFace) {
        best = camera_device;
      }
    }
    camera_ = std::make_unique<QCamera>(best);
  }
}

#endif
auto QRImageDecoder::instance() -> QRImageDecoder * {
  static QRImageDecoder instance; 
    return &instance;
}
QRImageDecoder::QRImageDecoder(QObject *parent)
    : QObject(parent)

#ifndef USE_EMSCRIPTEN
      ,
      capture_session_{std::make_unique<QMediaCaptureSession>()},
      video_sink_{std::make_unique<QVideoSink>()}
#endif
{
#ifndef USE_EMSCRIPTEN
  std::thread decoding_thread([this]() {
    std::unique_lock lock(decoding_mutex_);
    while (decode_running_) {
      decoding_variable_.wait(lock);
      decodePicture();
    }
  });
  decoding_thread.detach();
  capture_session_->setVideoOutput(video_sink_.get());
  QObject::connect(video_sink_.get(), &QVideoSink::videoFrameChanged, this,
                   [this](const QVideoFrame &Vframe) {
                     if (camera_ && camera_->isActive() && Vframe.isValid()) {
                       auto picture = Vframe.toImage();
                       WasmImageProvider::img = picture;
                       setid();
                       if (state_ == State::Ready) {
                         {
                           std::lock_guard const lock(decoding_mutex_);
                           state_ = State::Decoding;
                         }
                         decoding_variable_.notify_one();
                       }
                     }
                   });
  connect(this, &QRImageDecoder::useTorchChanged, this, [this]() {
    if (camera_->isActive() && use_torch_){
      camera_->setTorchMode(QCamera::TorchOn);
    }
    else
        {
        camera_->setTorchMode(QCamera::TorchOff);
    }
  });
#endif
};
void QRImageDecoder::stop() {
#ifdef USE_EMSCRIPTEN
  js_stop();
#else
    if (camera_){
        camera_->stop();
    }
#endif
};
void QRImageDecoder::start() {
#ifdef USE_EMSCRIPTEN
  clear();
  js_start();
#elif QT_CONFIG(permissions)
  QCameraPermission const k_permission;
  switch (qApp->checkPermission(k_permission)) {
  case Qt::PermissionStatus::Undetermined:
    qApp->requestPermission(k_permission, this, &QRImageDecoder::start);
    return;
  case Qt::PermissionStatus::Denied:
    return;
  case Qt::PermissionStatus::Granted:
    if (camera_ == nullptr) {
      getCamera();
      if (camera_ != nullptr) {
        capture_session_->setCamera(camera_.get());
        QObject::connect(camera_.get(), &QCamera::activeChanged, this,[this](bool var) {
          if (var && camera_->isTorchModeSupported(QCamera::TorchOn)) {
            has_torch_ = true;
            Q_EMIT hasTorchChanged();
          }
        });

        QObject::connect(camera_.get(), &QCamera::errorOccurred,
                         [](QCamera::Error  /*error*/, const QString &error_string) {
                           qDebug() << "Camera Error:" << error_string;
                         });
      }
    }
    if (camera_ != nullptr) {
      clear();
      camera_->start();
    }

    return;
  }

#endif
}

void QRImageDecoder::decodePicture() {
  QImage picture = WasmImageProvider::img;
  picture.convertTo(QImage::Format_Grayscale8);
  const auto k_str = detector_.decodeGrey(picture.bits(), picture.height(),
                                        picture.bytesPerLine());
  const auto k_qstr = QString::fromStdString(k_str);
  if (k_qstr != "") {
    emit decodedQR(k_qstr);
  }
  state_ = State::Ready;
}

QImage WasmImageProvider::img = QImage();
auto WasmImageProvider::requestImage(const QString &/*id*/, QSize */*size*/,
                                       const QSize &/*requestedSize*/) -> QImage {
  return img;
}

void QRImageDecoder::clear() {
  WasmImageProvider::restart();
  setid();
}

void WasmImageProvider::restart() {
  WasmImageProvider::img = QImage(QSize(200, 150), QImage::Format_RGBA8888);
  WasmImageProvider::img.fill("black");
}

void QRImageDecoder::reload(int offset, int width, int height) {
  auto *imgarr = reinterpret_cast<uchar *>(offset);
  WasmImageProvider::img =
      QImage(imgarr, width, height, QImage::Format_RGBA8888);
  setid();
  if (state_ == State::Ready) {
    state_ = State::Decoding;
    decodePicture();
  }
}

void QRImageDecoder::setid() {
  static quint8 index = 0;
  source_ = "qrimage" + QString::number(index);
  emit sourceChanged();
  index++;
}

}  // namespace Esterv::Utils::QrDec
