import QtQuick 2.0

FocusScope {


    property int currentCollectionIndex: 0
    property var currentCollection: api.collections.get(currentCollectionIndex)
    property var currentGame: currentCollection.games.get(gameView.currentIndex)

    Keys.onLeftPressed: {
        if (currentCollectionIndex <= 0)
        {
            currentCollectionIndex = api.collections.count - 1;
        }
        else {
            currentCollectionIndex--;
        }
    }

    Keys.onRightPressed: {
        if (currentCollectionIndex >= api.collections.count - 1)
        {
            currentCollectionIndex = 0;
        }
        else {
            currentCollectionIndex++;
        }
    }

    Rectangle {
        id: topbar

        color: "#0B2670"
        height: parent.height * 0.08

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        Text {
            id: conjureOsDisplay
            text : "Conjure Arcade"

            font.family:"Roboto"
            font.capitalization: Font.AllUppercase

            color: "white"
            fontSizeMode: Text.Fit

            minimumPointSize: 5
            font.pointSize: 30

            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: vpx(20)

            anchors.left: parent.left
        }

        Image {
            id: conjureName

            fillMode: Image.PreserveAspectFit
            source: "assets/logos/noir_sur_blanc.png"

            anchors.rightMargin: vpx(20)
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.top: parent.top
        }
    }

    Image {
        fillMode: Image.PreserveAspectCrop
        source: "assets/BG.png"
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        z: -3
    }

    Rectangle {
        id: menu

        property real contentWidth: width - vpx(25)

        color: "#9900008b"

        width: parent.width * 0.2
        anchors.top: topbar.bottom
        anchors.bottom: parent.bottom
        z: 2

        Text {
            id: collectionDisplay

            color: "#B8BFFC"
            width: parent.contentWidth
            text: currentCollection.shortName

            minimumPixelSize: 10;
            font.pixelSize: 60


            font.capitalization: Font.Capitalize
            fontSizeMode: Text.HorizontalFit;
            font.family:"Roboto"

            anchors.horizontalCenter: parent.horizontalCenter

            anchors.top: parent.top
            anchors.topMargin: vpx(20)
        }

        ListView {
            id: gameView

            model: currentCollection.games
            delegate: gameViewDelegate

            width: parent.contentWidth
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: collectionDisplay.bottom
            anchors.bottom: parent.bottom
            anchors.margins: vpx(20)

            highlightRangeMode: ListView.StrictlyEnforceRange
            highlightMoveDuration: 0

            preferredHighlightBegin: 10
            preferredHighlightEnd: height

            snapMode: ListView.SnapOneItem

            spacing : vpx(15)

            focus: true
            clip: true
        }

        Component {
            id: gameViewDelegate

            Item {
                id: itemList

                width: ListView.view.width - vpx(50)
                height: cartridgeGame.height
                anchors.horizontalCenter: parent.horizontalCenter

                Rectangle {
                    id: selectionMarker

                    width: cartridgeGame.width + vpx(10)
                    height: cartridgeGame.height + vpx(10)

                    color: itemList.ListView.isCurrentItem ? "#C18800" : "transparent"

                    anchors.centerIn: cartridgeGame
                }

                Image {
                    id: cartridgeGame

                    source: modelData.assets.cartridge

                    sourceSize { width: 512; height: 288 }

                    width: parent.width

                    fillMode :Image.PreserveAspectFit

                    anchors.fill: parent.width

                    Keys.onPressed: {
                        if (api.keys.isAccept(event) && !event.isAutoRepeat)
                        {
                            modelData.launch()
                        }
                    }
                }



            }
        }
    }

    Rectangle {
        id: content

        color: "transparent"

        anchors.left: menu.right
        anchors.right: parent.right
        anchors.top: topbar.bottom
        anchors.bottom: parent.bottom

        z: 1

        Image {
            id: cartridge

            fillMode: Image.PreserveAspectFit
            horizontalAlignment: Image.AlignRight

            source: currentGame.assets.boxFront
                || currentGame.assets.poster
                || currentGame.assets.banner
                || currentGame.assets.tile

            sourceSize {
                width: 1024; height: 576
            }

            anchors.topMargin: vpx(40)
            anchors.rightMargin: vpx(40)
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.left: parent.left

            height: parent.height * 0.5

            asynchronous: true
        }

        Text {
            id: title

            text: currentGame.title
            color: "white"
            font.family:"Roboto"
            font.pixelSize: vpx(50)
            font.bold: true

            horizontalAlignment: Text.AlignRight
            elide: Text.ElideRight

            anchors.topMargin: vpx(10)

            anchors.top: cartridge.bottom
            anchors.right: cartridge.right
        }

        Text {
            id: developerText
            text: currentGame.developer
            color: "#FFC1FD"
            font.pixelSize: vpx(15)
            font.family:"Roboto"

            anchors.top: title.bottom
            anchors.right: cartridge.right
            anchors.left: parent.left

            anchors.topMargin: vpx(10)
            anchors.leftMargin: vpx(250)

            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignRight
        }

        Text {
            id: gameRelease
            text: currentGame.release.toLocaleDateString(Qt.locale(), Locale.ShortFormat)
            color: "white"
            font.pixelSize: vpx(15)
            font.family:"Roboto"
            visible: currentGame.release > 0

            anchors.topMargin: vpx(10)
            anchors.top: developerText.bottom
            anchors.right: cartridge.right
            anchors.left: cartridge.left

            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignRight
        }

        Text {
            id: genre

            text: currentGame.genre
            color: "white"
            font.pixelSize: vpx(15)
            font.family:"Roboto"

            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignRight
            elide: Text.ElideRight

            anchors.top: gameRelease.bottom
            anchors.topMargin: vpx(5)
            anchors.left: cartridge.left
            anchors.right: cartridge.right
        }

        Text {
            id: description

            text: currentGame.summary || currentGame.description
            color: "white"
            font.pixelSize: vpx(18)
            font.family:"Roboto"

            anchors.topMargin: vpx(10)
            anchors.leftMargin: vpx(20)

            anchors.top: genre.bottom

            anchors.bottom: genre.top

            anchors.left: cartridge.left
            anchors.right: cartridge.right

            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignRight
        }

    }

}

