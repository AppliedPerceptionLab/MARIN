import QtQuick 2.9
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.3

Item {
    id: root
    x:0
    y:0

    signal closeMessage(int action)

    Rectangle {
        id: rectangle
        x: 96
        y: 76
        width: 448
        height: 328
        color: "#363940"
        border.color: "#272a30"
        border.width: 0
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        Text {
            id: messageText1
            x: 13
            y: 99
            height: 60
            anchors.top: connectionImageId.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            text: qsTr("No network connection")
            font.bold: true
            fontSizeMode: Text.FixedSize
            horizontalAlignment: Text.AlignHCenter
            anchors.rightMargin: 0
            anchors.leftMargin: 0
            anchors.topMargin: 23
            font.pixelSize: 30
            color: "#f6f3f3"
        }

        Text {
            id: messageText2
            x: -90
            y: 135
            height: 65
            anchors.top: messageText1.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            text: qsTr("Make sure the network connection is enabled in this device and your network connection is active.")
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.wordSpacing: 0
            wrapMode: Text.WordWrap
            anchors.rightMargin: 63
            anchors.topMargin: -23
            anchors.leftMargin: 63
            font.family: "Verdana"
            font.pixelSize: 16
            color: "#f6f3f3"
        }


        Rectangle {
            id: closeButtonId
            color: "#f05b5b"
            radius: 8
            border.color: "#f05b5b"
            border.width: 1
            x: 151
            y: 264
            width: 146
            height: 38

            Text {
                id: name
                width: 146
                height: 38
                color: "#f6f3f3"

                text: qsTr("Close")
                font.pointSize: 18
                font.bold: true
                styleColor: "#f11919"
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font.family: "Tahoma"
            }

            MouseArea {
                anchors.fill: parent
                onClicked: root.closeMessage(AppSettings.executeShowServerConnectionError);
            }
        }

        Image {
            id: connectionImageId
            x: 174
            anchors.top: parent.top
            width: 110
            height: 80
            source: "../images/network.png"
            anchors.horizontalCenterOffset: 0
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 31

        }
    }

    Rectangle {
        id: rectangle1
        color: "#000000"
        opacity: 0.8
        z: -1
        anchors.fill: parent
    }

}

