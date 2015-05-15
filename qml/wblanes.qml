// qml/wblanes.qml – Program UI
//
// Copyright © 2015 Émilien Tlapale
// Licensed under the Simplified BSD License.

import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1

ApplicationWindow {
    visible: true

    FileDialog {
        id: fileDialog
        title: "Select an image to load"
        onAccepted: {
            image.source = fileUrl
            selection.visible = false
            plot.source = ""
        }
    }

    toolBar: ToolBar {
        RowLayout {
            ToolButton {
                text: "Load"
                onClicked: fileDialog.open()
            }
            ToolButton {
                text: "Quit"
                onClicked: Qt.quit()
            }
        }
    }

    statusBar: StatusBar {
        RowLayout {
            Label {
                id: posLabel
            }
            Label {
                id: selSizeLabel
                visible: false
                text: selection.width + "×" + selection.height
            }
            Label {
                objectName: "ratioLabel"
            }
        }
    }

    SplitView {
        orientation: Qt.Vertical
        anchors.fill: parent
        
        ScrollView {
            //anchors.fill: parent
            Layout.fillWidth: true
            
            Image {
                id: image
                objectName: "image"
                Layout.fillWidth: true

                signal areaSelected(real x, real y, real width, real height)

                property real x0
                property real y0

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onPressed: {
                        selection.visible = true
                        image.x0 = selection.x = mouse.x
                        image.y0 = selection.y = mouse.y
                        selection.width = selection.height = 0
                        selSizeLabel.visible = true
                    }
                    onPositionChanged: {
                        if (pressed) {
                            selection.x = Math.min(image.x0, mouse.x)
                            selection.y = Math.min(image.y0, mouse.y)
                            selection.width = Math.abs(mouse.x - image.x0)
                            selection.height = Math.abs(mouse.y - image.y0)
                        }
                        posLabel.text = "("+mouse.x+","+mouse.y+")"
                    }
                    onReleased: {
                        var w = Math.abs(image.x0 - mouse.x);
                        var h = Math.abs(image.y0 - mouse.y);
                        if (w * h > 100 && h > 40)
                        image.areaSelected(Math.min(image.x0, mouse.x),
                        Math.min(image.y0, mouse.y),
                        w, h)
                    }
                }

                Rectangle {
                    id: selection
                    visible: false
                    opacity: 0.35
                    color: "yellow"
                    width: 100
                    height: 100
                }
            }
        }
        ScrollView {
            Layout.fillWidth: true
            Image {
                id: plot
                objectName: "plot"
                Layout.fillWidth: true
            }
        }
    }
}