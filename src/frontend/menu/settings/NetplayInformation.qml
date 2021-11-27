// Pegasus Frontend
//
// Created by BozoTheGeek 16/11/2021
//

import "common"
import "../../search"
import "qrc:/qmlutils" as PegasusUtils
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12

FocusScope {
    id: root
	
	property bool isCallDirectly : false
	
    signal close

    width: parent.width
    height: parent.height

    anchors.fill: parent
	
    //enabled: focus
    
    visible: 0 < (x + width) && x < Window.window.width

    //timer to refresh Netplay list
    property var counter: 4
    Timer {
        id: netplayTimer
        interval: 1000 // Run the timer every second
        repeat: true
        running: true
        triggeredOnStart: true
        onTriggered: {

                if ((interval/1000)*counter === 5){ // wait 5 seconds before to refresh
                    //console.log("netplayTimer - before refresh: availableNetplayRooms.selectedButtonIndex", availableNetplayRooms.selectedButtonIndex);
                    api.internal.netplay.rooms.refresh();
                    counter = 0;
				}
                else counter = counter + 1;
        }
    }
    //function to update index where focus should be
    function updateFocusIndex()
    {
        //if existing selected index is visible
        if(availableNetplayRooms.itemAt(availableNetplayRooms.selectedButtonIndex).visible !== true){
            //need to select an other index
            //first after if exist...
            for(var i = availableNetplayRooms.selectedButtonIndex; i < availableNetplayRooms.count; i++)
            {
                if(availableNetplayRooms.itemAt(i).visible === true) {
                    availableNetplayRooms.selectedButtonIndex = i;
                    break;
                }
            }
            //if no visible found after...
            if(availableNetplayRooms.itemAt(availableNetplayRooms.selectedButtonIndex).visible !== true){
                //need check before if exist...
                for(var j = availableNetplayRooms.selectedButtonIndex; j >= 0; j--)
                {
                    if(availableNetplayRooms.itemAt(j).visible === true) {
                        availableNetplayRooms.selectedButtonIndex = j;
                        break;
                    }
                }
            }
        }
        //console.log("availableNetplayRooms.selectedButtonIndex: ", availableNetplayRooms.selectedButtonIndex);
    }

    Keys.onPressed: {
        if (api.keys.isCancel(event) && !event.isAutoRepeat) {
            event.accepted = true;
            //clean rooms model
            api.internal.netplay.rooms.reset();
            root.close();
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
        text: isCallDirectly ? qsTr("Netplay information") + api.tr : qsTr("Accounts > Netplay information") + api.tr
        z: 2
    }
    Rectangle {
        width: parent.width
        color: themeColor.main
        anchors {
            top: header.bottom
            bottom: parent.bottom
        }
    }
    Flickable {
        id: container

        width: content.width
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: header.bottom
        anchors.bottom: parent.bottom

        contentWidth: content.width
        contentHeight: content.height

        Behavior on contentY { PropertyAnimation { duration: 100 } }
        boundsBehavior: Flickable.StopAtBounds
        boundsMovement: Flickable.StopAtBounds

        readonly property int yBreakpoint: height * 0.7
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

                width: root.width * 0.9
				height: implicitHeight

                Item {
					width: parent.width
					height: implicitHeight + vpx(30)
                }


                //for test purpose only
                ListModel {
                    id: myFriends
                    //ListElement { nickname: "Anonymous"; }
                }

                SectionTitle {
                    text: qsTr("My Friend's rooms") + api.tr
                    first: true
                    visible: myFriends.count > 0 ? true : false
                }


                Row{
                    Image {
                        id: logoRetroarch
                        height: vpx(50)
                        source: "../../assets/libretro-retroarch-simple-logo.png"
                        anchors.verticalCenter: retroarch_title.verticalCenter
                        fillMode: Image.PreserveAspectFit
                    }
                    SectionTitle {
                    id: retroarch_title
                    text: "  " + qsTr("Retroarch lobby : ") + (availableNetplayRooms.count - availableNetplayRooms.hidden) + qsTr(" room(s)")  + api.tr
					first: true
					visible: true
                    }
                }
                //for test purpose only
                /*ListModel {
                    id: availableNetplayRoomsModel
                    ListElement { country: "br"; username: "Anonymous";
                                  game_name: "Metal Slug X (U) [SLUS-01212]";
                                  game_crc: "D634567DF";
                                  core_name: "PCSX-ReARMed";
                                  core_version: "r22 36f3ea6";
                                  retroarch_version: "1.8.8";
                                  frontend: "win32 x64";
                                  ip: "192.168.0.1";
                                  port: 8080;
                                  mitm_ip: "";
                                  mitm_port: 0;
                                  host_method : 0;
                                  has_password: false;
                                  has_spectate_password: true;
                                  created: "19 Oct 21 12:05 UTC";
                                  updated: "19 Oct 21 12:10 UTC";
                                }
                }*/
                Repeater {
                    id: availableNetplayRooms
                    model: api.internal.netplay.rooms  // availableNetplayRoomsModel //for test purpose
                    property var selectedButtonIndex : 0
                    property var hidden : 0
                    onItemRemoved:{
                        //RFU
                        //console.log("onItemRemoved: ", index)
                    }
                    onItemAdded:{
                        //RFU
                        //console.log("onItemAdded: ", index)
                    }
                    delegate: DetailedButton {
                        SearchGame {
                            id: searchByCRCorFile;
                            onMaxChanged:{
                                //console.log("onMaxChanged - max :",searchByCRCorFile.max);
                                //console.log("onMaxChanged - game_crc :",game_crc);
                                //console.log("onMaxChanged - game_name :",game_name);
                                //console.log("onMaxChanged - crc :",searchByCRCorFile.crc);
                                //console.log("onMaxChanged - crcToFind :",searchByCRCorFile.crcToFind);
                                //console.log("onMaxChanged - filename :",searchByCRCorFile.filename);
                                //console.log("onMaxChanged - filenameRegEx :",searchByCRCorFile.filenameRegEx);
                                //console.log("onMaxChanged - filenameToFilter :",searchByCRCorFile.filenameToFilter);
                                if((game_crc === "") && (game_name === "")) {
                                    searchByCRCorFile.crc = "";
                                    searchByCRCorFile.filename = "";
                                    picture = "";
                                    icon2 = "";
                                }
                                else if (searchByCRCorFile.max === 1 && searchByCRCorFile.crc !== "") { //CRC search and match

                                    picture = result.games.get(0).assets.screenshot;
                                    icon2 = result.games.get(0).assets.logo;
                                }
                                else if (searchByCRCorFile.max >= 1 && searchByCRCorFile.filename !== "") { //file name match
                                    picture = searchByCRCorFile.result.games.get(0).assets.screenshot;
                                    icon2 = searchByCRCorFile.result.games.get(0).assets.logo;
                                }
                                else if (max !== 1 && crc !== ""){
                                    picture = "";
                                    icon2 = "";
                                    //Search by file name now if CRC not found
                                    //Launch timer for that
                                    searchByCRCorFile.crc = "";
                                    searchByCRCorFile.filename = game_name;
                                }
                                else{
                                    picture = "";
                                    icon2 = "";
                                }
                            }
                        }

                        property var status_icon : "\uf1c0 " // or "\uf1c1"/"?" or "\uf1c2"/"X"
                        property var latency_icon : "\uf1c8 " // or "\uf1c7" or "\uf1c6" or "\uf1c5" or "\uf1c9"/"?"
                        property var private_icon : has_password ? "\uf071 " : ""
                        property var visibility_icon : has_spectate_password ? "\uf070 " : " "
                        width: parent.width - vpx(100)
                        enabled: visible
                        visible :{
                            availableNetplayRooms.hidden = api.internal.netplay.rooms.nbEmptyRooms()
                            if ((game_crc === "") && (game_name === "")) return false;
                            else return true;
                        }
                        //for preview
                        label: {
                            return (status_icon + latency_icon + private_icon + visibility_icon + username + " / " + ((searchByCRCorFile.max === 1 && searchByCRCorFile.crc !== "") ? searchByCRCorFile.result.games.get(0).title : ((searchByCRCorFile.max >= 1 && searchByCRCorFile.filename !== "") ? searchByCRCorFile.result.games.get(0).title : game_name)));
                        }
                        note: {
                            return (" " + qsTr("Creation date") + ": " + created);
                        }
                        //check note only because created date will change only if room is change
                        onNoteChanged:
                        {
                            //game change or add
                            //console.log("At: ",index,"- Add/Change: ",game_name);
                            //check if both are not empty as deleted one
                            if(game_crc !== "" && game_name !== ""){
                                //search by crc first
                                if (game_crc !== "00000000"){
                                    searchByCRCorFile.filename = "";
                                    searchByCRCorFile.crc = game_crc;
                                }
                                else{
                                    searchByCRCorFile.filename = game_name;
                                    searchByCRCorFile.crc = "";
                                }
                            }
                            else
                            {
                                searchByCRCorFile.filename = "";
                                searchByCRCorFile.crc = "";
                            }
                        }
                        //add image of country
                        icon: {
                            return ("https://flagcdn.com/h60/" + country + ".png");
                        }
                        detailed_line2: {
                            return "Retroarch version : ";
                        }
                        detailed_line3: {
                            return "Core: ";
                        }
                        detailed_line4: {
                            return "Core version : ";
                        }
                        detailed_line5: {
                            return "Architecture : ";
                        }
                        detailed_line6: {
                            return "Game CRC : ";
                        }
                        detailed_line7: {
                            return "Game file : ";
                        }
                        detailed_line10: {
                            return "\uf1c0" + " " + retroarch_version;
                        }
                        detailed_line11: {
                            return "\uf1c0" + " " + core_name
                        }
                        detailed_line11_color: {
                            return "green"
                        }
                        detailed_line12: {
                            return "\uf1c0" + " " + core_version
                        }
                        detailed_line12_color: {
                            return "green"
                        }
                        detailed_line13: {
                            return  frontend;
                        }
                        detailed_line14: {
                            return ((searchByCRCorFile === 1 && searchByCRCorFile.crc !== "") ? "\uf1c0" : "\uf1c2" ) + " " + game_crc;
                        }
                        detailed_line14_color: {
                            return ((searchByCRCorFile.max === 1 && searchByCRCorFile.crc !== "") ? "green" : "red" )
                        }
                        detailed_line15: {
                            return ((searchByCRCorFile.max >= 1 && searchByCRCorFile.filename !== "") ? "\uf1c0" : "\uf1c2" ) + " " + game_name;
                        }
                        detailed_line15_color: {
                            return ((searchByCRCorFile.max >= 1 && searchByCRCorFile.filename !== "") ? "green" : "red" )
                        }



                        // set focus only on first item
                        focus:{
                            //console.log("------Begin of Focus-------");
                            //console.log("api.internal.netplay.rooms.count : ", api.internal.netplay.rooms.rowCount() );
                            //console.log("availableNetplayRooms.selectedButtonIndex : ",availableNetplayRooms.selectedButtonIndex)
                            //console.log("Index : ",index)

                            if (index === availableNetplayRooms.selectedButtonIndex){
                                if(visible){
                                    return true;
                                }
                                updateFocusIndex();
                                /*
                                else if((availableNetplayRooms.selectedButtonIndex + 1) < api.internal.netplay.rooms.rowCount()){
                                    availableNetplayRooms.selectedButtonIndex = availableNetplayRooms.selectedButtonIndex + 1;

                                }
                                else availableNetplayRooms.selectedButtonIndex = api.internal.netplay.rooms.rowCount()-1;*/
                            }
                            return false;
                        }
                        onActivate: {
                            ////to force change of focus
                            // confirmDialog.focus = false;
                            // confirmDialog.setSource("../../dialogs/Generic3ChoicesDialog.qml",
                                                    // { "title": myDiscoveredDevicesModel.get(index).vendor + " " + myDiscoveredDevicesModel.get(index).name + " " + myDiscoveredDevicesModel.get(index).service,
                                                      // "message": qsTr("Do you want to pair or ignored this device ?") + api.tr,
                                                      // "symbol": myDiscoveredDevicesModel.get(index).icon,
                                                      // "firstchoice": qsTr("Pair") + api.tr,
                                                      // "secondchoice": qsTr("Ignored") + api.tr,
                                                      // "thirdchoice": qsTr("Cancel") + api.tr});
                            ////Save action states for later
                            // actionState = "Pair";
                            // actionListIndex = index;
                            ////to force change of focus
                            // confirmDialog.focus = true;
                            ////focus = true;

                        }

                        onFocusChanged:{
							container.onFocus(this)                            
						}
                        Keys.onPressed: {
                            //verify if finally other lists are empty or not when we are just before to change list
                            //it's a tip to refresh the KeyNavigations value just before to change from one list to an other
                            if ((event.key === Qt.Key_Up) && !event.isAutoRepeat) {
                                if (index !== 0) {
                                    availableNetplayRooms.selectedButtonIndex = index-1;
									KeyNavigation.up = availableNetplayRooms.itemAt(index-1);
								}
                                else {
									KeyNavigation.up = availableNetplayRooms.itemAt(0);
                                    availableNetplayRooms.selectedButtonIndex = 0;
								}
                            }
                            if ((event.key === Qt.Key_Down) && !event.isAutoRepeat) {
                                if (index < availableNetplayRooms.count-1) {
									KeyNavigation.down = availableNetplayRooms.itemAt(index+1);
                                    availableNetplayRooms.selectedButtonIndex = index+1;
								}
                                else {
									KeyNavigation.down = availableNetplayRooms.itemAt(availableNetplayRooms.count-1);
                                    availableNetplayRooms.selectedButtonIndex = availableNetplayRooms.count-1;
								}
                            }
                        }

                        Button {
                            id: pairButton
                            property int fontSize: vpx(22)
                            height: fontSize * 1.5
                            text: qsTr("Play/View ?") + api.tr
                            visible: parent.focus
                            anchors.left: parent.right
                            anchors.leftMargin: vpx(20)
                            anchors.verticalCenter: parent.verticalCenter
                            
							contentItem: Text {
                                text: pairButton.text
                                font.pixelSize: fontSize
                                font.family: globalFonts.sans
                                opacity: 1.0
                                color: themeColor.textSectionTitle
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                elide: Text.ElideRight
                            }
							
							background: Rectangle {
                                implicitWidth: 100
                                implicitHeight: parent.height
                                opacity: 1.0
                                border.color: themeColor.textSectionTitle
                                color: themeColor.textLabel
                                border.width: 3
                                radius: 25
                            }
                        }
                    }
                }				
				
				SectionTitle {
					text: qsTr("Dolphin") + api.tr
                    first: false
                    visible: false // hide for the moment
				}
			}
		}
	}
}
