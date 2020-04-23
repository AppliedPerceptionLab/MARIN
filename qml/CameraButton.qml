import QtQuick 2.9

Item {
    id: cameraButtonItem
//    x: 0
//    y: 0
   // x: set in uimanager.cpp
   // y: set in uimanager.cpp
   // width: set in uimanager.cpp
//    height: 70
//    width: 70
    visible: true

    signal takePicture()

//    Rectangle {
//        id: filterThree
//        x: 13
//        y: 6
//        width: 25
//        height: 25
//        color: "#808080"
//        radius: 12
//        border.color: "#808080"
//        border.width: 0
//        objectName: "vessels"

//        property bool isActive: false;
//        property int filterType: AppSettings.vesselsFilter
//    }

    Rectangle {
        x: 0
//        anchors.fill: parent
        y: 0
//        x: 500
        id: cameraButton
        width: 70
        height: 70
        color: "#FFFF0000"
        radius: 49
        border.color: "#FF0000"
        //z: 1
        clip: true
        border.width: 5

        MouseArea{
//            x: 0
//            anchors.fill: parent
//            y: 0
            width: 70
            height: 70
            onClicked:
                cameraButtonItem.takePicture()
        }

    }
}



