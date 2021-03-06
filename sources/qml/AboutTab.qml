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

import QtQuick 2.0
import QtQuick.Controls 1.3 as QtControls
import QtQuick.Layouts 1.0 as QtLayouts


Column {
    anchors.fill: parent

    property var textProvider

    QtControls.TabView {
        height: parent.height
        width: parent.width

        QtControls.Tab {
            anchors.margins: 10.0
            title: i18n("About")

            QtLayouts.ColumnLayout {
                QtControls.Label {
                    QtLayouts.Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    text: textProvider.getAboutText("header")
                }

                QtControls.Label {
                    QtLayouts.Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    text: textProvider.getAboutText("version")
                }

                QtControls.Label {
                    QtLayouts.Layout.fillWidth: true
                    horizontalAlignment: Text.AlignJustify
                    text: textProvider.getAboutText("description")
                }

                QtControls.Label {
                    QtLayouts.Layout.fillWidth: true
                    horizontalAlignment: Text.AlignLeft
                    textFormat: Text.RichText
                    text: textProvider.getAboutText("links")
                    onLinkActivated: Qt.openUrlExternally(link)
                }

                QtControls.Label {
                    QtLayouts.Layout.fillHeight: true
                    QtLayouts.Layout.fillWidth: true
                    font.capitalization: Font.SmallCaps
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignBottom
                    textFormat: Text.RichText
                    text: textProvider.getAboutText("copy")
                }
            }
        }

        QtControls.Tab {
            anchors.margins: 10.0
            title: i18n("Acknowledgment")

            QtLayouts.ColumnLayout {
                QtControls.Label {
                    QtLayouts.Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignJustify
                    text: textProvider.getAboutText("translators")
                }

                QtControls.Label {
                    QtLayouts.Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignJustify
                    textFormat: Text.RichText
                    text: textProvider.getAboutText("3rdparty")
                    onLinkActivated: Qt.openUrlExternally(link)
                }

                QtControls.Label {
                    QtLayouts.Layout.fillHeight: true
                    QtLayouts.Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignJustify
                    verticalAlignment: Text.AlignTop
                    textFormat: Text.RichText
                    text: textProvider.getAboutText("thanks")
                    onLinkActivated: Qt.openUrlExternally(link)
                }
            }
        }
    }
}
