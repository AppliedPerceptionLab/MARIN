import QtQuick 2.9


Item {
    id: root
    x:79
    y:0

    Rectangle {
        id: rectangle1
        color: "black"
        //opacity: 1
        //z: -1
        anchors.fill: parent


        Image {
            id: name
            anchors.fill: parent
            //source: "../images/planes.png"
           //fillMode: Image.PreserveAspectFit
            anchors.left: parent.left
            anchors.right: parent.right


        }
    }

}

