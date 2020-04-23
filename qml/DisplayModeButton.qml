import QtQuick 2.9

Image {
    id: actionImage
    width: 45
    height: 45
    fillMode: Image.PreserveAspectFit
    source: actionImage.img_AR
    onEnabledChanged: changeImageOnClick()

    //display modes are 1: AugmentationOnly ;; 2: CameraOnly ;; 3: AugmentedReality
    property int displayMode: 3
    property string img_augmentation: ""
    property string img_video: ""
    property string img_AR: ""

    function changeImage(){
        displayMode = displayMode % 3 + 1;
        if( displayMode == 1 ){
            actionImage.source = actionImage.img_augmentation
        }else if( displayMode == 2 ){
                actionImage.source = actionImage.img_video
        }else if( displayMode == 3 ){
            actionImage.source = actionImage.img_AR
        }else{ //DEFAULT, but shouldn't happen anyway
            actionImage.source = actionImage.img_AR
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            actionImage.changeImage();
            if( typeof parent.executeAction === "function" ){
                parent.executeAction();
            }
        }
    }

}





