import QtQuick
import FreeChart

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Free Chart")

    // Timer {
    //     repeat: true
    //     running: true
    //     interval: 250

    //     onTriggered: {
    //         freeChart.data = []

    //         for (var i = 0; i < 100; i++) {
    //             freeChart.data.push(Math.random())
    //         }

    //         freeChart.replaceSeries(freeChart.dataProperty)
    //     }
    // }
    FreeChart {
        id: freeChart

        anchors.fill: parent
    }
}
