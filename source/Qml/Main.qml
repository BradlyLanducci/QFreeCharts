import QtQuick
import Series

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Free Chart")

    Series {
        id: freeChart

        anchors.fill: parent

        antialiasing: true
    }
}
