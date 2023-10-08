import QtQuick 2.0
import QtQuick.Controls 1.4

FocusScope {
    id: root

    property alias title: titleText.text
    property alias message: messageText.text

    property int textSize: vpx(18)
    property int titleTextSize: vpx(20)

    property string temppassword: ""

    signal accept()
    signal cancel()

    anchors.fill: parent
    visible: shade.opacity > 0

    focus: true
    onActiveFocusChanged: state = activeFocus ? "open" : ""
    Component.onCompleted: passwordField.focus = true

    function validation(){
        if(passwordField.text === "HardcodedPassword"){
            accept()
        }
        return
    }

    Keys.onPressed: {
        if (api.keys.isCancel(event) && !event.isAutoRepeat) {
            event.accepted = true;
            root.cancel();
        }
    }

    // actual dialog
    MouseArea {
        anchors.centerIn: parent
        width: dialogBox.width
        height: dialogBox.height
    }

    Shade {
        id: shade
        onCancel: root.cancel()
    }

    title: qsTr("Attention") + api.tr
    message: qsTr("Zone Interdite") + api.tr

    Column {
        id: dialogBox

        width: parent.height * 0.8
        anchors.centerIn: parent
        scale: 0.5

        Behavior on scale {
            NumberAnimation {
                duration: 125
            }
        }

        // title bar
        Rectangle {
            id: titleBar
            width: parent.width
            height: root.titleTextSize * 2.25
            color: "#333"

            Text {
                id: titleText

                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    leftMargin: root.titleTextSize * 0.75
                }

                color: "#eee"
                font {
                    bold: true
                    pixelSize: root.titleTextSize
                    family: globalFonts.sans
                }
            }
        }


        // text area
        Rectangle {
            width: parent.width
            height: messageText.height + 3 * root.textSize
            color: "#555"

            Text {
                id: messageText

                anchors.centerIn: parent
                width: parent.width - 2 * root.textSize

                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter

                color: "#eee"
                font {
                    pixelSize: root.textSize
                    family: globalFonts.sans
                }
            }
        }



        //password field
        TextField {
            id: passwordField
            width: parent.width

            placeholderText: qsTr("Enter password...")

            echoMode: TextInput.Password

            KeyNavigation.down: okButton

            Keys.onPressed: {
                if (api.keys.isAccept(event) && !event.isAutoRepeat) {
                    event.accepted = true;
                    validation()
                }
            }


        }

        // button row
        Row {
            width: parent.width
            height: root.textSize * 2

            Rectangle {
                id: okButton

                width: parent.width * 0.5
                height: root.textSize * 2.25
                color: (focus || okMouseArea.containsMouse) ? "#3c4" : "#666"

                Keys.onPressed: {
                    if (api.keys.isAccept(event) && !event.isAutoRepeat) {
                        event.accepted = true;
                        validation()
                    }
                }

                Text {
                    anchors.centerIn: parent

                    text: qsTr("Ok") + api.tr
                    color: "#eee"
                    font {
                        pixelSize: root.textSize
                        family: globalFonts.sans
                    }
                }

                MouseArea {
                    id: okMouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: validation()
                }
            }

            Rectangle {
                id: cancelButton

                focus: true

                width: parent.width * 0.5
                height: root.textSize * 2.25
                color: (focus || cancelMouseArea.containsMouse) ? "#f43" : "#666"

                KeyNavigation.left: okButton
                Keys.onPressed: {
                    if (api.keys.isAccept(event) && !event.isAutoRepeat) {
                        event.accepted = true;
                        root.cancel();
                    }
                }

                Text {
                    anchors.centerIn: parent

                    text: qsTr("Cancel") + api.tr
                    color: "#eee"
                    font {
                        pixelSize: root.textSize
                        family: globalFonts.sans
                    }
                }

                MouseArea {
                    id: cancelMouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: root.cancel()
                }
            }
        }
    }

    states: [
        State {
            name: "open"
            PropertyChanges {
                target: shade; opacity: 0.8
            }
            PropertyChanges {
                target: dialogBox; scale: 1
            }
        }
    ]

}
