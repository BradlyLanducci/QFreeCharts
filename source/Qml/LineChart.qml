import QtQuick 2.15
import QFreeCharts.Series 1.0
import QtQuick.Layouts

Item {
    id: root

    property var series: []

    property color backgroundColor: "cyan"

    Rectangle {
        id: seriesContainer

        anchors.fill: parent

        anchors.bottomMargin: Math.min(root.width / 10, root.height / 10)
        anchors.leftMargin: anchors.bottomMargin

        color: root.backgroundColor
    }

    function seriesContains(series) {
        for (var i = 0; i < root.series.length; i++) {
            if (series === root.series[i]) {
                return true
            }

            return false
        }
    }

    onChildrenChanged: {
        for (var i = 0; i < children.length; i++) {
            if (children[i] instanceof Series) {

                if (!seriesContains(children[i])) {

                    children[i].anchors.fill = seriesContainer

                    root.series.push(children[i])
                }
            }
        }
    }

    Timer {
        repeat: true
        running: true
        interval: 250

        onTriggered: {
            for (var i = 0; i < root.series.length; i++) {
                var series = root.series[i]

                var points = []
                for (var j = 0; j < 50; j++) {
                    points.push(Math.random())
                }

                if (series !== null) {
                    series.replaceSeries(points)
                }
            }
        }
    }
}
