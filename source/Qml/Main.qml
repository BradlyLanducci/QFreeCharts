import QtQuick
import QFreeCharts.Series 1.0

Window {
    id: root

    width: 640
    height: 480
    visible: true
    title: qsTr("Free Chart")

    LineChart {
        id: lineChart

        anchors.fill: parent
        anchors.margins: 5

        backgroundColor: "#564946"

        Series {
            color: "#558564"
            lineWidth: 4
        }

        Series {
            color: "#95f9e3"
            lineWidth: 1
        }
    }
}
