import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: root
    width: 1400
    height: 900
    visible: true
    title: "SurgiView Image Tracker"
    color: "#11141A"

    property int refreshTick: surgiView.frameRevision

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#1B2430" }
            GradientStop { position: 1.0; color: "#0E1015" }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            TextField {
                id: folderPath
                Layout.fillWidth: true
                placeholderText: "Slice folder path (CT/X-ray images)"
                text: ""
                color: "#E4EAF2"
                placeholderTextColor: "#8493A5"
                background: Rectangle {
                    radius: 8
                    color: "#222A35"
                    border.color: "#415067"
                }
            }

            Button {
                text: "Load Slices"
                onClicked: surgiView.loadSliceFolder(folderPath.text)
            }

            Button {
                text: "Prev"
                onClicked: surgiView.previousSlice()
            }

            Button {
                text: "Next"
                onClicked: surgiView.nextSlice()
            }

            Label {
                text: "Slice: " + (surgiView.currentSliceIndex + 1) + " / " + surgiView.totalSlices
                color: "#C8D5E6"
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Button {
                text: surgiView.trackerRunning ? "Stop Tracking" : "Start Tracking"
                onClicked: surgiView.trackerRunning ? surgiView.stopTracking() : surgiView.startTracking()
            }

            CheckBox {
                id: labMode
                checked: true
                text: "Lab testing mode (simulate C-arm drift)"
                onToggled: surgiView.setLabTestingMode(checked)
            }

            ComboBox {
                id: sourceModeBox
                model: ["simulated", "external"]
                currentIndex: surgiView.trackingSourceMode === "external" ? 1 : 0
                onActivated: surgiView.setTrackingSourceMode(currentText)
            }

            TextField {
                id: telemetryPort
                Layout.preferredWidth: 110
                text: surgiView.telemetryServerPort > 0 ? surgiView.telemetryServerPort.toString() : "45454"
                placeholderText: "tcp port"
            }

            Button {
                text: surgiView.telemetryServerListening ? "Stop TCP" : "Start TCP"
                onClicked: {
                    if (surgiView.telemetryServerListening) {
                        surgiView.stopTelemetryServer()
                    } else {
                        surgiView.startTelemetryServer(parseInt(telemetryPort.text))
                    }
                }
            }

            Button {
                text: surgiView.recording ? "Stop Recording" : "Start Recording"
                onClicked: surgiView.recording ? surgiView.stopRecording() : surgiView.startRecording()
            }

            Button {
                text: "Play"
                onClicked: surgiView.startPlayback()
            }

            Button {
                text: "Pause"
                onClicked: surgiView.pausePlayback()
            }

            TextField {
                id: extX
                Layout.preferredWidth: 90
                text: "220"
                placeholderText: "tool x"
            }

            TextField {
                id: extY
                Layout.preferredWidth: 90
                text: "170"
                placeholderText: "tool y"
            }

            TextField {
                id: extDepth
                Layout.preferredWidth: 90
                text: "10.5"
                placeholderText: "depth"
            }

            Button {
                text: "Inject Telemetry"
                onClicked: {
                    surgiView.ingestExternalTelemetry(parseFloat(extX.text), parseFloat(extY.text), parseFloat(extDepth.text))
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            TextField {
                id: sessionPath
                Layout.fillWidth: true
                text: "session.json"
                placeholderText: "Session JSON path"
            }

            Button {
                text: "Save Session"
                onClicked: surgiView.saveRecording(sessionPath.text)
            }

            Button {
                text: "Load Session"
                onClicked: surgiView.loadRecording(sessionPath.text)
            }

            TextField {
                id: exportFolder
                Layout.preferredWidth: 260
                text: "export_frames"
                placeholderText: "Export folder"
            }

            TextField {
                id: videoPath
                Layout.preferredWidth: 260
                text: "annotated.mp4"
                placeholderText: "Output video path"
            }

            Button {
                text: "Export Annotated"
                onClicked: surgiView.exportAnnotatedFrames(exportFolder.text)
            }

            Button {
                text: "Export Video"
                onClicked: surgiView.exportAnnotatedVideo(exportFolder.text, videoPath.text)
            }
        }

        Rectangle {
            id: viewer
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: 12
            color: "#111319"
            border.color: "#38475D"
            border.width: 1
            clip: true

            Image {
                id: medicalFrame
                anchors.fill: parent
                source: "image://surgiview/current?rev=" + refreshTick
                fillMode: Image.PreserveAspectFit
                cache: false
            }

            Canvas {
                id: overlay
                anchors.fill: parent
                onPaint: {
                    var ctx = getContext("2d")
                    ctx.clearRect(0, 0, width, height)

                    var tx = surgiView.toolTip.x
                    var ty = surgiView.toolTip.y
                    var gx = surgiView.targetPoint.x
                    var gy = surgiView.targetPoint.y

                    ctx.strokeStyle = "#56E094"
                    ctx.fillStyle = "#56E09466"
                    ctx.lineWidth = 2
                    ctx.beginPath()
                    ctx.arc(gx, gy, 9, 0, 2 * Math.PI)
                    ctx.fill()
                    ctx.stroke()

                    ctx.strokeStyle = "#FF5B5B"
                    ctx.fillStyle = "#FF5B5B66"
                    ctx.beginPath()
                    ctx.arc(tx, ty, 9, 0, 2 * Math.PI)
                    ctx.fill()
                    ctx.stroke()

                    ctx.strokeStyle = "#F7C948"
                    ctx.lineWidth = 2
                    ctx.beginPath()
                    ctx.moveTo(tx, ty)
                    ctx.lineTo(gx, gy)
                    ctx.stroke()
                }

                Connections {
                    target: surgiView
                    function onOverlayChanged() {
                        overlay.requestPaint()
                    }
                    function onFrameUpdated() {
                        overlay.requestPaint()
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        surgiView.setTargetPoint(mouse.x, mouse.y)
                    }
                    onPressed: {
                        surgiView.setMeasurementStart(mouse.x, mouse.y)
                    }
                    onPositionChanged: {
                        if (pressed) {
                            surgiView.setMeasurementEnd(mouse.x, mouse.y)
                        }
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 24

            Label {
                text: "Alignment Error: " + surgiView.alignmentErrorPx.toFixed(2) + " px"
                color: "#E3EDF9"
                font.bold: true
            }

            Label {
                text: "Tool Depth: " + surgiView.depthMm.toFixed(2) + " mm"
                color: "#E3EDF9"
                font.bold: true
            }

            Label {
                text: "Measured Distance: " + surgiView.measurementPx.toFixed(2) + " px / " + surgiView.measurementMm.toFixed(2) + " mm"
                color: "#E3EDF9"
            }
        }

        Label {
            Layout.fillWidth: true
            text: surgiView.statusMessage
            color: "#8FD3FF"
            wrapMode: Text.Wrap
        }
    }
}
