import QtQuick 2.9

Item {
    id: tools
    width: 100
    height: 100
    visible: true
    z: 100

    Rectangle {
        id: profileContainer
        width: 100
        height: 100
        color: "#00000000"
        radius: 49
        border.color: "#333030"
        //z: 1
        clip: true
        border.width: 5

        Image {
            id: image
            width: 95
            height: 95
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            anchors.top: parent.top
            enabled: true
            fillMode: Image.PreserveAspectFit
            z: -1
            visible: true
            clip: false
            source: "../images/man.png"
        }
    }
}



