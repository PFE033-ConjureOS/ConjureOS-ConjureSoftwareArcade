// Pegasus Frontend
// Copyright (C) 2017  Mátyás Mustoha
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


import QtQuick 2.12

Rectangle {
    property alias text: label.text
    property bool pressed: false
    property bool recording: false

    width: vpx(140)
    height: label.font.pixelSize * 1.5
    color: {
        if (recording) return "#c33";
        if (pressed) return "#353";
        if (activeFocus) return "#3aa"
        return themeColor.secondary;
    }

    anchors {
        left: parent.alignment === Text.AlignLeft ? parent.left : undefined
        right: parent.alignment === Text.AlignRight ? parent.right : undefined
        margins: (Positioner.index - 1) * vpx(3)
    }

    Text {
        id: label
        color: themeColor.textLabel
        font {
            family: globalFonts.sans
            pixelSize: vpx(18)
        }
        horizontalAlignment: parent.parent.alignment
        anchors {
            left: parent.left
            right: parent.right
            verticalCenter: parent.verticalCenter
            margins: vpx(5)
        }
    }
}
