// Pegasus Frontend
// Copyright (C) 2017-2018  Mátyás Mustoha
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.


import "common"
import "qrc:/qmlutils" as PegasusUtils
import QtQuick 2.12
import QtQuick.Window 2.12

FocusScope {
    id: root

    signal close
//    signal openKeySettings
    signal openGamepadSettings
    signal openGameDirSettings
    signal openAdvancedControllersConfiguration

    width: parent.width
    height: parent.height
    visible: 0 < (x + width) && x < Window.window.width

    enabled: focus

    Keys.onPressed: {
        if (api.keys.isCancel(event) && !event.isAutoRepeat) {
            event.accepted = true;
            root.close();
            api.internal.recalbox.saveParameters();
        }
    }
    PegasusUtils.HorizontalSwipeArea {
        anchors.fill: parent
        onSwipeRight: root.close()
    }
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        onClicked: root.close()
    }
    ScreenHeader {
        id: header
        text: qsTr("Controllers") + api.tr
        z: 2
    }
    Flickable {
        id: container

        width: content.width
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        contentWidth: content.width
        contentHeight: content.height

        Behavior on contentY { PropertyAnimation { duration: 100 } }

        readonly property int yBreakpoint: height * 0.5
        readonly property int maxContentY: contentHeight - height

        function onFocus(item) {
            if (item.focus)
                contentY = Math.min(Math.max(0, item.y - yBreakpoint), maxContentY);
        }
        FocusScope {
            id: content

            focus: true
            enabled: focus

            width: contentColumn.width
            height: contentColumn.height

            Column {
                id: contentColumn
                spacing: vpx(5)

                width: root.width * 0.7
                height: implicitHeight
                Item {
                    width: parent.width
                    height: header.height + vpx(25)
                }
                SectionTitle {
                    text: qsTr("Controllers") + api.tr
                    first: true
                }
                SimpleButton {
                    id: optPairControllers

                    // set focus only on first item
                    focus: true

                    label: qsTr("Pair Bluetooth Controllers") + api.tr
                    note: qsTr("") + api.tr
                    onActivate: {
                        focus = true;
                        root.openKeySettings();
                    }
                    onFocusChanged: container.onFocus(this)
                    KeyNavigation.down: optGamepadConfig
                }
                SimpleButton {
                    id: optGamepadConfig

                    label: qsTr("Gamepad layout") + api.tr
                    note: qsTr("Show game layout configuration controller") + api.tr
                    onActivate: {
                        focus = true;
                        root.openGamepadSettings();
                    }
                    onFocusChanged: container.onFocus(this)
                    KeyNavigation.down: optAdvancedControllers
                }
                SimpleButton {
                    id: optAdvancedControllers

                    label: qsTr("Advanced Controllers Configuration") + api.tr
                    note: qsTr("Choose your drivers or Special Controllers") + api.tr
                    onActivate: {
                        focus = true;
                        root.openAdvancedControllersConfiguration();
                    }
                    onFocusChanged: container.onFocus(this)
                    KeyNavigation.down: optInputP1
                }
                SectionTitle {
                    text: qsTr("Controllers Input") + api.tr
                    first: true
                }
                SimpleButton {
                    id: optInputP1

                    label: qsTr("Input players 1") + api.tr
                    onActivate: {
                        focus = true;
                        root.openInputP1();
                    }
                    onFocusChanged: container.onFocus(this)
                    KeyNavigation.down: optInputP2
                }
                SimpleButton {
                    id: optInputP2

                    label: qsTr("Input players 2") + api.tr
                    onActivate: {
                        focus = true;
                        root.openInputP2();
                    }
                    onFocusChanged: container.onFocus(this)
                    KeyNavigation.down: optInputP3
                }
                SimpleButton {
                    id: optInputP3

                    label: qsTr("Input players 3") + api.tr
                    onActivate: {
                        focus = true;
                        root.openInputP3();
                    }
                    onFocusChanged: container.onFocus(this)
                    KeyNavigation.down: optInputP4
                }
                SimpleButton {
                    id: optInputP4

                    label: qsTr("Input players 4") + api.tr
                    onActivate: {
                        focus = true;
                        root.openInputP4();
                    }
                    onFocusChanged: container.onFocus(this)
                    KeyNavigation.down: optPairControllers
                }
                Item {
                    width: parent.width
                    height: implicitHeight + vpx(30)
                }
            }
        }
    }
    MultivalueBox {
        id: localeBox
        z: 3

        model: api.internal.settings.locales
        index: api.internal.settings.locales.currentIndex

        onClose: content.focus = true
        onSelect: api.internal.settings.locales.currentIndex = index
    }
    MultivalueBox {
        id: themeBox
        z: 3

        model: api.internal.settings.themes
        index: api.internal.settings.themes.currentIndex

        onClose: content.focus = true
        onSelect: api.internal.settings.themes.currentIndex = index
    }
}
