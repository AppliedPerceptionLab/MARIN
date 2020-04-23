import QtQuick 2.9

Item {
    id: reregisterPanel
   // x: set in uimanager.cpp
   // y: set in uimanager.cpp
   // width: set in uimanager.cpp
    height: 70
    visible: true

    signal enableMode(int mode)
    signal disableMode(int mode)

    Rectangle {
            id: filtersContainer

            color: "#363940"
            radius: 4
            anchors.fill: parent
            border.color: "#363940"
            border.width: 0

            function onClickFilters(object, activeColor, inactiveColor){
                if(object.isActive){
                    object.color = inactiveColor;
                    object.isActive = false;
                    console.log(object.filterType);
                    reregisterPanel.disableMode(object.filterType);
                } else{
                    object.color = activeColor;
                    object.isActive = true;
                    reregisterPanel.enableMode(object.filterType);
                }
            }

            Rectangle {
                id: filterOneWrapper
                x: 100
                y: 5
                width: 114
                height: 35
                color: "#363940"
                radius: 3
                border.color: "#363940"
                border.width: 2

                MouseArea{
                    x: parent.x
                    y: parent.y
                    anchors.fill: parent
                    onClicked:
                        filtersContainer.onClickFilters(filterOne, "#bbc754", "#808080");
                }

                Text {
                    id: text3
                    x: 44
                    y: 6
                    width: 62
                    height: 23
                    color: "#808080"
                    text: qsTr("Rotate")
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                    font.letterSpacing: 1
                    font.wordSpacing: 0
                    font.family: "Verdana"
                    font.bold: true
                    font.pixelSize: 13
                }

                Rectangle {
                    id: filterOne
                    x: 13
                    y: 6
                    width: 25
                    height: 25
                    color: "#bbc754"
                    radius: 12
                    border.width: 0
                    border.color: "#bbc754"
                    objectName: "rotate"

                    property bool isActive: true
                    property int filterType: AppSettings.rotateMode
                }
            }

            Rectangle {
                id: filterTwoContainer
                x: 205
                y: 5
                width: 97
                height: 35
                color: "#363940"
                radius: 1
                border.color: "#363940"
                border.width: 2

                MouseArea{
                    x: 0
                    anchors.fill: parent
                    y: parent.y
                    onClicked:
                        filtersContainer.onClickFilters(filterTwo, "#ffbc03", "#808080");
                }

                Text {
                    id: text4
                    x: 42
                    y: 6
                    width: 60
                    height: 23
                    color: "#808080"
                    text: qsTr("Translate")
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                    font.wordSpacing: 0
                    font.bold: true
                    font.family: "Verdana"
                    font.letterSpacing: 1
                    font.pixelSize: 13
                }

                Rectangle {
                    id: filterTwo
                    x: 13
                    y: 6
                    width: 25
                    height: 25
                    color: "#ffbc03"
                    radius: 12
                    border.color: "#ffbc03"
                    border.width: 0
                    objectName: "translate"

                    property bool isActive: true;
                    property int filterType: AppSettings.translateMode
                }
            }

            Rectangle {
                id: filterThreeContainer
                x: 331
                y: 5
                width: 122
                height: 35
                color: "#363940"
                radius: 3
                border.color: "#363940"
                border.width: 2



                MouseArea{
                    anchors.fill: parent
                    x: parent.x
                    y: parent.y
                    onClicked:
                        filtersContainer.onClickFilters(filterThree, "#808080", "#808080");
                }

                Text {
                    id: text5
                    x: 44
                    y: 6
                    width: 70
                    height: 23
                    color: "#808080"
                    text: qsTr("Reset")
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                    font.wordSpacing: 0
                    font.bold: true
                    font.family: "Verdana"
                    font.letterSpacing: 1
                    font.pixelSize: 13
                }

                Rectangle {
                    id: filterThree
                    x: 13
                    y: 6
                    width: 25
                    height: 25
                    color: "#808080"
                    radius: 12
                    border.color: "#808080"
                    border.width: 0
                    objectName: "reset"

                    property bool isActive: false;
                    property int filterType: AppSettings.resetReregistration
                }
            }

            Text {
                id: filterLabel
                x: 18
                y: 13
                width: 78
                height: 23
                color: "#808080"
                text: qsTr("Modes :")
                font.wordSpacing: 0
                font.bold: true
                font.family: "Verdana"
                font.letterSpacing: 1
                font.pixelSize: 15
            }
        }

}
