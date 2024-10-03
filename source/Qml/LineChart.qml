import QtQuick 2.15
import QFreeCharts.Series 1.0
import QtQuick.Layouts

Item {
    id: root

    property var series: []

    property color backgroundColor: "cyan"

    Rectangle {
        id: seriesContainer

        anchors.centerIn: parent

        width: parent.width * 0.8
        height: parent.height * 0.8

        color: root.backgroundColor
    }

    Repeater {
        id: xTicks

        model: 10

        property real tickSpacing: (seriesContainer.height - 1) / (xTicks.count - 1)

        delegate: Rectangle {
            color: "black"

            width: 5
            height: 1

            x: seriesContainer.x - width
            y: seriesContainer.y + seriesContainer.height - height - (xTicks.tickSpacing * index)
        }
    }

    Repeater {
        id: yTicks

        model: 10

        property real tickSpacing: (seriesContainer.width - 1) / (yTicks.count - 1)

        delegate: Rectangle {
            color: "black"

            width: 1
            height: 5

            x: seriesContainer.x + seriesContainer.width - width - (yTicks.tickSpacing * index)
            y: seriesContainer.y + seriesContainer.height
        }
    }

    function containsSeries(series) {
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
                if (!containsSeries(children[i])) {
                    console.log(i, children[i])
                    children[i].anchors.fill = seriesContainer

                    root.series.push(children[i])
                }
            }
        }
    }

    Timer {
        repeat: true
        running: true
        interval: 10

        property var points: []

        property int t: 0
        property real phase: 0
        property real randomFrequency: 0.5
        property int randomInterval: 500

        onTriggered: {

            for (var i = 0; i < 2; i++) {
                var series = root.series[i]

                // Generate a random sine wave frequency between 100 Hz and 1100 Hz
                if (t % randomInterval === 0) {
                    randomFrequency = Math.random() * 0.02
                    randomInterval = Math.random() * 1000
                }

                var randomSineWave = 0.25 * (Math.sin(
                                                 2 * Math.PI * randomFrequency * t + phase) + 1.0)

                // Generate constant sine wave
                var constantSineWave = 0.25 * (Math.sin(
                                                   2 * Math.PI * 1.0 * t + phase) + 1.0)

                // Combine sine waves
                while (points.length < 1000) {
                    var outputSample = randomSineWave + constantSineWave
                    points.push(outputSample)
                    t++
                }

                // Shift points if necessary to keep the length manageable
                while (points.length >= 1000) {
                    points.shift()
                }

                // Update phase for the next iteration
                phase += 0.01

                // Update the series if it's not null
                if (series !== null) {
                    series.replaceSeries(
                                points) // Use replace method to update the series
                }
            }
        }
    }
}
