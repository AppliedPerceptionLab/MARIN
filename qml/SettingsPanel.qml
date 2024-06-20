import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.2

Item {
    id: settingsPannelContainer
    //x: set in uimanager.cpp
    //width: set in uimanager.cpp
    //height: set in uimanager.cpp
    opacity: 1


    property bool on: false
    signal triggerAction( int action )

    Rectangle {
        id: rectangle1
        color: "#000000"
        opacity: 0.8
        z: -1
        anchors.fill: parent
    }


    Rectangle {
        id: rectangle
        x: 96
        y: 76
        width: 524
        height: 452
        color: "#363940"
        anchors.verticalCenterOffset: 8
        anchors.horizontalCenterOffset: -81
        z: 2
        border.color: "#272a30"
        border.width: 0
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        property bool isSettingOneActive: false
        property bool isSettingTwoActive: false

        Image {
            id: gear
            x: 173
            y: 25
            anchors.top: parent.top
            width: 45
            height: 45
            opacity: 0.9
            source: "../images/gear_on.png"
            anchors.horizontalCenterOffset: -108
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 27

        }

        Text {
            id: messageText1
            x: 13
            y: 97
            height: 50
            anchors.top: gear.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            text: qsTr("         Settings Panel")
            verticalAlignment: Text.AlignVCenter
            font.bold: true
            fontSizeMode: Text.FixedSize
            horizontalAlignment: Text.AlignHCenter
            anchors.rightMargin: 0
            anchors.leftMargin: 0
            anchors.topMargin: -50
            font.pixelSize: 30
            color: "#f6f3f3"
        }

        Text {
            id: text3
            x: 0
            y: 410
            width: 524
            height: 17
            color: "#f6f3f3"
            text: qsTr("Changes are saved automatically")
            opacity: 0.4
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 14
        }

        ///////////////////////////////////////////////////////////////////
        //Server info:
        ///////////////////////////////////////////////////////////////////
        Text {
            id: serverLabel
            x: 76
            y: 113
            width: 120
            height: 25
            color: "#f6f3f3"
            text: qsTr("Server : ")
            font.family: "Verdana"
            font.underline: false
            font.bold: false
            font.pixelSize: 20
            horizontalAlignment: Text.AlignRight
            font.italic: false
            verticalAlignment: Text.AlignVCenter
        }
        Text {
            id: serverAddressLabel
            x: 225
            y: 113
            width: 120
            height: 25
            color: "#f6f3f3"
            text: qsTr( th.getServerAddress() )
            font.family: "Verdana"
            font.underline: false
            font.bold: false
            font.pixelSize: 20
        }

        ///////////////////////////////////////////////////////////////////
        //Client info:
        ///////////////////////////////////////////////////////////////////
        Text {
            id: clientLabel
            x: 76
            y: 148
            width: 120
            height: 25
            color: "#f6f3f3"
            text: qsTr("Client : ")
            font.family: "Verdana"
            font.underline: false
            font.bold: false
            font.pixelSize: 20
            horizontalAlignment: Text.AlignRight
            font.italic: false
            verticalAlignment: Text.AlignVCenter
        }
        Text {
            id: clientAddressLabel
            x: 225
            y: 148
            width: 120
            height: 25
            color: "#f6f3f3"
            text: qsTr( th.getClientAddress() )
            font.family: "Verdana"
            font.underline: false
            font.bold: false
            font.pixelSize: 20
        }

        ///////////////////////////////////////////////////////////////////
        //Sender info/options:
        ///////////////////////////////////////////////////////////////////
        Switch{
            id: senderToggleButton
            x: 225
            y: 163
            width: 70
            height: 70
            checked: th.getSending()
            objectName: "senderToggleButton"
            style: SwitchStyle {
                groove: Rectangle {
                        implicitWidth: 70
                        implicitHeight: 20
                        border.color: "gray"
                        border.width: 1
                        color: senderToggleButton.checked ? "green" : "gray"
                }
            }
            onClicked: {
                if( checked ){
                    settingsPannelContainer.triggerAction(AppSettings.executeToggleSenderOn);
                }else{
                    settingsPannelContainer.triggerAction(AppSettings.executeToggleSenderOff);
                }
            }
        }

        Text {
            id: text1
            x: 76
            y: 182
            width: 120
            height: 25
            color: "#f6f3f3"
            text: qsTr( "Sender : " )
            font.italic: false
            font.underline: false
            font.bold: false
            font.family: "Verdana"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignRight
            font.pixelSize: 20
        }
        Text {
            id: textField1
            x: 225
            y: 217
            width: 120
            height: 25
            color: "#f6f3f3"
            text: "(port " + th.getSendingPortVideo() + " (video))"
            font.italic: false
            font.underline: false
            font.bold: false
            font.family: "Verdana"
            font.pixelSize: 20
        }
        Text {
            id: textField2
            x: 225
            y: 252
            width: 120
            height: 25
            color: "#f6f3f3"
            text: "(port " + th.getSendingPortCommands() + " (commands))"
            font.italic: false
            font.underline: false
            font.bold: false
            font.family: "Verdana"
            font.pixelSize: 20
        }

        ///////////////////////////////////////////////////////////////////
        //Receiver info/options:
        ///////////////////////////////////////////////////////////////////
        Switch{
            id: receiverToggleButton
            x: 225
            y: 278
            width: 70
            height: 70
            checked: true
            objectName: "receiverToggleButton"
            style: SwitchStyle {
                groove: Rectangle {
                        implicitWidth: 70
                        implicitHeight: 20
                        border.color: "gray"
                        border.width: 1
                        color: receiverToggleButton.checked ? "green" : "gray"
                }
            }
            onClicked: {
                if( checked ){
                    settingsPannelContainer.triggerAction(AppSettings.executeToggleReceiverOn);
                }else{
                    settingsPannelContainer.triggerAction(AppSettings.executeToggleReceiverOff);
                }
            }
        }

        Text {
            id: text2
            x: 76
            y: 301
            width: 120
            height: 25
            color: "#f6f3f3"
            text: qsTr("Receiver : ")
            horizontalAlignment: Text.AlignRight
            font.bold: false
            verticalAlignment: Text.AlignVCenter
            font.underline: false
            font.italic: false
            font.pixelSize: 20
            font.family: "Verdana"
        }
        Text {
            id: textField3
            x: 226
            y: 337
            width: 120
            height: 25
            color: "#f6f3f3"
            text: "(port " + th.getReceivingPortVideo() + " (video))"
            font.italic: false
            font.underline: false
            font.bold: false
            font.family: "Verdana"
            font.pixelSize: 20
        }
        Text {
            id: textField4
            x: 225
            y: 372
            width: 120
            height: 25
            color: "#f6f3f3"
            text: "(port " + th.getReceivingPortCommands() + " (commands))"
            font.italic: false
            font.underline: false
            font.bold: false
            font.family: "Verdana"
            font.pixelSize: 20
        }

    }
}

