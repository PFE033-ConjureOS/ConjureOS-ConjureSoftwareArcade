import QtQuick 2.0

FocusScope {

    property int currentCollectionIndex: 0
    property var currentCollection: api.collections.get(currentCollectionIndex)
    property var currentGame: currentCollection.games.get(gameView.currentIndex)

    Keys.onLeftPressed: {
        if (currentCollectionIndex <= 0)
            currentCollectionIndex = api.collections.count - 1;
        else
            currentCollectionIndex--;
    }

    Keys.onRightPressed: {
        if (currentCollectionIndex >= api.collections.count - 1)
            currentCollectionIndex = 0;
        else
            currentCollectionIndex++;
    }

    Rectangle {
        id: menu

        property real contentWidth: width - vpx(100)

        color: "#00008b"

        width: parent.width * 0.3
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        Text {
            id: collectionDisplay

            color: "orange"
            width: parent.contentWidth
            text: currentCollection.shortName
            fontSizeMode: Text.HorizontalFit;
            minimumPixelSize: 10;
            font.pixelSize: 72
            font.capitalization: Font.Capitalize

            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: vpx(50)
        }

        ListView {
            id: gameView

            model: currentCollection.games
            delegate: gameViewDelegate

            width: parent.contentWidth
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: collectionDisplay.bottom
            anchors.bottom: parent.bottom
            anchors.margins: vpx(50)

            highlightRangeMode: ListView.ApplyRange
            highlightMoveDuration: 0
            preferredHighlightBegin: height * 0.5 - vpx(18)
            preferredHighlightEnd: height * 0.5 + vpx(18)

            focus: true
            clip: true
        }

        Component {
            id: gameViewDelegate

            Text {
                text: modelData.title
                color: ListView.isCurrentItem ? "orange" : "white"
                font.family: globalFonts.condensed
                font.pixelSize: ListView.isCurrentItem ? vpx(24) : vpx(20)
                font.bold: ListView.isCurrentItem

                width: ListView.view.width
                height: vpx(36)
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight

                Keys.onReturnPressed: modelData.launch()
            }
        }
    }

    Rectangle {
        id: content

        color: "blue"

        anchors.left: menu.right
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom



        Image {
            id: banner

            anchors.top: parent.top
            anchors.right: parent.right
            anchors.margins: vpx(10)
            anchors.bottom: parent.verticalCenter
            anchors.bottomMargin: 0

            fillMode: Image.PreserveAspectFit
            horizontalAlignment: Image.AlignRight

            source: currentGame.assets.banner
            sourceSize {
                width: 1024; height: 1024
            }
            asynchronous: true
        }

        Image {
            id: cartridge

            fillMode: Image.PreserveAspectFit
            horizontalAlignment: Image.AlignLeft

            source: currentGame.assets.cartridge
            sourceSize {
                width: 1024; height: 576
            }
            anchors.margins: vpx(50)
            anchors.top: parent.top
            anchors.right: banner.left
            anchors.left: parent.left
            anchors.bottom: banner.bottom

            asynchronous: true
        }

        Text {
            id: title

            text: currentGame.title
            color: "white"
            font.family: globalFonts.sans
            font.pixelSize: vpx(42)
            font.bold: true

            horizontalAlignment: Text.AlignRight
            elide: Text.ElideRight

            anchors.top: banner.bottom
            anchors.topMargin: vpx(40)
            anchors.left: banner.left
            anchors.right: banner.right
        }

        Text {
            id: developerText
            text: currentGame.developer
            color: "white"
            font.pixelSize: vpx(18)
            font.family: globalFonts.sans

            anchors.top: title.bottom
            anchors.right: title.right
            anchors.left: title.left
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignRight
        }

        Text {
            id: gameRelease
            text: currentGame.release
            color: "white"
            font.pixelSize: vpx(18)
            font.family: globalFonts.sans
            visible: currentGame.release > 0

            anchors.topMargin: vpx(30)
            anchors.top: developerText.bottom
            anchors.right: title.right
            anchors.left: title.left
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignRight
        }
        Text {
            id: genre

            text: currentGame.genre
            color: "white"
            font.pixelSize: vpx(15)
            font.family: globalFonts.sans

            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignRight
            elide: Text.ElideRight

            anchors.top: gameRelease.bottom
            anchors.topMargin: vpx(10)
            anchors.left: banner.left
            anchors.right: banner.right
        }

        Text {
            id: description

            text: currentGame.summary || currentGame.description
            color: "white"
            font.pixelSize: vpx(18)
            font.family: globalFonts.sans

            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignRight


            anchors.top: genre.bottom
            anchors.topMargin: vpx(30)
            anchors.bottom: parent.bottom
            anchors.bottomMargin: vpx(50)
            anchors.left: banner.left
            anchors.right: banner.right
        }


        Image {
            id: conjureName

            width: parent.contentWidth
            height: vpx(100)

            fillMode: Image.PreserveAspectFit
            source: "assets/logos/noir_sur_blanc.png"

            anchors.right: parent.right
            anchors.bottom: parent.bottom
        }
    }
}

