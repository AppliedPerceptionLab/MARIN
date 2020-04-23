import QtQuick 2.9

Image {
    id: actionImage
    width: 45
    height: 45
    fillMode: Image.PreserveAspectFit
    source: actionImage.img_source_off
    onEnabledChanged: changeImageOnClick()

    property bool isActive: false
    property bool isAnimationEnabled: false
    property string img_source_on: ""
    property string img_source_off: ""

    SequentialAnimation {
        SequentialAnimation {
            running: actionImage.isActive && actionImage.isAnimationEnabled

            NumberAnimation { target: actionImage; property: "opacity"; to: 1.0; duration: 1000 }
            NumberAnimation { target: actionImage; property: "opacity"; to: 0.4; duration: 1000 }
        }
        PauseAnimation { duration: 500 }
        running: isActive && actionImage.isAnimationEnabled
        loops: Animation.Infinite
    }


    function changeImage()
    {
        if (actionImage.isActive){
                actionImage.source = actionImage.img_source_off
            }else{
                actionImage.source = actionImage.img_source_on
            }
        actionImage.isActive = !actionImage.isActive;
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            actionImage.changeImage();

            if (typeof parent.executeAction === "function") {
                parent.executeAction();
            }

        }
    }

}





