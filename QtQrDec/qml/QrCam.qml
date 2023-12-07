import QtQuick 2.0
import QtQuick.Controls
import QtQuick.Layouts
import Esterv.Styles.Simple

import Esterv.CustomControls.QrDec
Image
{
    id:control


    cache : false
    source: "image://wasm/"+QRImageDecoder.source

    Switch {
        id:useTorch
        opacity: checked ? 0.75 : 0.25
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        visible:QRImageDecoder.hasTorch
        onCheckedChanged:
        {
            QRImageDecoder.useTorch=useTorch.checked;
        }
        text:qsTr("Torch")
    }

}
