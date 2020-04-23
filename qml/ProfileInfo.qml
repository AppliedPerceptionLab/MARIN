import QtQuick 2.9

Item {
    id: tools
    width: 200
    height: 38
    visible: true
    z: 30
    property alias patientIdText: patientId.text
    property alias patientInfoText: patientInfo.text

    Rectangle{
        width: parent.width
        height: parent.height
        color: "#00000000"

        Text {
            id: patientId
            x: 0
            y: 0
            width: 200
            height: 29
            color: "#f76f72"
            text: qsTr("Jonatan Reyes")
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            font.family: "Verdana"
            font.bold: true
            font.pixelSize: 19

        }

        Text {
            id: patientInfo
            x: 0
            y: 23
            width: 200
            height: 16
            color: "#6e7a85"
            text: qsTr("DOB: 08/13/1980")
            wrapMode: Text.WrapAnywhere
            horizontalAlignment: Text.AlignLeft
            font.bold: true
            font.family: "Verdana"
            font.pixelSize: 12
            //objectName: patientDOB
        }

    }











}



