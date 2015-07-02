/***************************************************************************
 *   This file is part of awesome-widgets                                  *
 *                                                                         *
 *   awesome-widgets is free software: you can redistribute it and/or      *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   awesome-widgets is distributed in the hope that it will be useful,    *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with awesome-widgets. If not, see http://www.gnu.org/licenses/  *
 ***************************************************************************/

import QtQuick 2.4
import QtQuick.Controls 1.3 as QtControls
import QtQuick.Layouts 1.1
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents

import org.kde.plasma.desktoppanel 1.0
import "."


Item {
    id: main

    property bool debug: DPAdds.isDebugEnabled()
    property variant tooltipSettings: {
        "tooltipColor": plasmoid.configuration.tooltipColor,
        "tooltipType": plasmoid.configuration.tooltipType,
        "tooltipWidth": plasmoid.configuration.tooltipWidth
    }

    signal needUpdate
    signal needTooltipUpdate
    signal sizeUpdate


    // init
    Plasmoid.preferredRepresentation: Plasmoid.fullRepresentation
    Plasmoid.compactRepresentation: Plasmoid.fullRepresentation

    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

    Plasmoid.icon: "utilities-system-monitor"
    Plasmoid.backgroundHints: plasmoid.configuration.background ? "DefaultBackground" : "NoBackground"


    // ui
    GridLayout {
        anchors.fill: parent
        columns: plasmoid.configuration.verticalLayout ? 1 : DPAdds.numberOfDesktops()
        rows: plasmoid.configuration.verticalLayout ? DPAdds.numberOfDesktops() : 1

        Repeater {
            id: repeater
            Layout.columnSpan: 0
            Layout.rowSpan: 0
            model: DPAdds.numberOfDesktops()

            Text {
                id: text
                height: contentHeight
                width: contentWidth
                textFormat: Text.RichText
                wrapMode: Text.NoWrap

                verticalAlignment: Text.AlignVCenter

                text: DPAdds.parsePattern(plasmoid.configuration.text, index + 1)
                property alias tooltip: tooltip

                MouseArea {
                    hoverEnabled: true
                    anchors.fill: parent
                    onClicked: DPAdds.setCurrentDesktop(index + 1);
                    onEntered: needTooltipUpdate()
                }

                PlasmaCore.ToolTipArea {
                    height: tooltip.height
                    width: tooltip.width
                    mainItem: Text {
                        id: tooltip
                        height: contentHeight
                        width: contentWidth
                        textFormat: Text.RichText
                    }
                }
            }
        }
    }

    Timer {
        id: timer
        interval: 1000
        onTriggered: needUpdate()
    }

    onNeedUpdate: {
        if (debug) console.log("[main::onNeedUpdate]")

        for (var i=0; i<repeater.count; i++) {
            if (!repeater.itemAt(i)) {
                if (debug) console.log("[main::onNeedUpdate] : Nothing to do here " + i)
                timer.start()
                return
            }
            repeater.itemAt(i).text = DPAdds.parsePattern(plasmoid.configuration.text, i + 1)
            if (DPAdds.currentDesktop() == i + 1) {
                repeater.itemAt(i).color = plasmoid.configuration.currentFontColor
                repeater.itemAt(i).font.family = plasmoid.configuration.currentFontFamily
                repeater.itemAt(i).font.italic = plasmoid.configuration.currentFontStyle == "italic" ? true : false
                repeater.itemAt(i).font.pointSize = plasmoid.configuration.currentFontSize
                repeater.itemAt(i).font.weight = general.fontWeight[plasmoid.configuration.currentFontWeight]
            } else {
                repeater.itemAt(i).color = plasmoid.configuration.fontColor
                repeater.itemAt(i).font.family = plasmoid.configuration.fontFamily
                repeater.itemAt(i).font.italic = plasmoid.configuration.fontStyle == "italic" ? true : false
                repeater.itemAt(i).font.pointSize = plasmoid.configuration.fontSize
                repeater.itemAt(i).font.weight = general.fontWeight[plasmoid.configuration.fontWeight]
            }
            repeater.itemAt(i).update()
        }

        sizeUpdate()
        needTooltipUpdate()
    }

    onNeedTooltipUpdate: {
        if (debug) console.log("[main::onNeedTooltipUpdate]")

        for (var i=0; i<repeater.count; i++) {
            repeater.itemAt(i).tooltip.text = DPAdds.toolTipImage(i + 1)
        }
    }

    onSizeUpdate: {
        if (debug) console.log("[main::onSizeUpdate]")

        if (plasmoid.configuration.height == 0) {
            var newHeight = 0
            for (var i=0; i<repeater.count; i++)
                newHeight += repeater.itemAt(i).contentHeight
            Layout.minimumHeight = newHeight
            Layout.maximumHeight = -1
        } else {
            Layout.minimumHeight = plasmoid.configuration.height
            Layout.maximumHeight = plasmoid.configuration.height
        }
        if (plasmoid.configuration.width == 0) {
            var newWidth = 0
            for (var i=0; i<repeater.count; i++)
                newWidth += repeater.itemAt(i).contentWidth
            Layout.minimumWidth = newWidth
            Layout.maximumWidth = -1
        } else {
            Layout.minimumWidth = plasmoid.configuration.width
            Layout.maximumWidth = plasmoid.configuration.width
        }
    }

    Plasmoid.onActivated: {
        if (debug) console.log("[main::onActivated]")

//         DPAdds.changePanelsState()
    }

    Plasmoid.onUserConfiguringChanged: {
        if (plasmoid.userConfiguring) return
        if (debug) console.log("[main::onUserConfiguringChanged]")

        DPAdds.setMark(plasmoid.configuration.mark)
        DPAdds.setPanelsToControl(plasmoid.configuration.panels)
        DPAdds.setToolTipData(tooltipSettings)

        needUpdate()
    }

    Component.onCompleted: {
        if (debug) console.log("[main::onCompleted]")

        // init submodule
        Plasmoid.userConfiguringChanged(false)
        DPAdds.desktopChanged.connect(needUpdate)
        DPAdds.windowListChanged.connect(needTooltipUpdate)
    }
}