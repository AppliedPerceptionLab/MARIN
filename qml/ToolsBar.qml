import QtQuick 2.9

Item {
    id: tools
    x: 0
    y: 0
    width: 79
    height: 580
    visible: true

    signal triggerAction(int action)

    Rectangle {
        id: toolsContainer
        anchors.fill: parent
        color: "#363940"
        objectName: "toolsContainerName"

        ActionButton{
            id: photoRegistrationButtonId
            x: 17
            y: 35
            img_source_on: "../images/camera_on.png"
            img_source_off:"../images/camera.png"
            objectName: "settingsButton"

            function executeAction(){
                if(isActive){
                    tools.triggerAction(AppSettings.startPhotoRegistration);
                } else {
                    tools.triggerAction(AppSettings.stopPhotoRegistration);
                }
            }
        }

        ActionButton{
            id: settingsButtonId
            x: 17
            y: 100
            img_source_on: "../images/gear_on.png"
            img_source_off:"../images/gear.png"
            objectName: "settingsButton"

            function executeAction(){
                if(isActive){
                    tools.triggerAction(AppSettings.executeOpenSettingsPanel);
                } else {
                    tools.triggerAction(AppSettings.executeCloseSettingsPanel);
                }
            }
        }

        ActionButton{
            x: 17
            y: 165
            img_source_on: "../images/brain_on.png"
            img_source_off:"../images/brain.png"
            objectName: "anatomyPlanesButton"

            function executeAction(){
                if(isActive){
                    tools.triggerAction(AppSettings.executeOpenAnatomyPlanes);
                } else {
                    tools.triggerAction(AppSettings.executeCloseAnatomyPlanes);
                }
            }
        }

        ActionButton{
            x: 17
            y: 230
            img_source_on: "../images/filters_on.png"
            img_source_off:"../images/filters.png"
            objectName: "filtersButton"

            function executeAction(){
                if(isActive){
                    tools.triggerAction(AppSettings.executeShowFilters);
                } else {
                    tools.triggerAction(AppSettings.executeHideFilters);
                }
            }
        }

        DisplayModeButton{
            x: 17
            //y: 515
            y: 295
            img_augmentation: "../images/augmentation_only.png"
            img_video: "../images/video_only.png"
            img_AR: "../images/video_augmentation.png"
            objectName: "displayModeButton"

            function executeAction(){
                if( displayMode == 1 ){
                    tools.triggerAction(AppSettings.executeSetModeAugmentationOnly);
                }else if( displayMode == 2 ) {
                    tools.triggerAction(AppSettings.executeSetModeVideoOnly);
                }else if( displayMode == 3 ){
                    tools.triggerAction(AppSettings.executeSetModeAR);
                }else{
                    console.log("Something weird happened.")
                }
            }
        }

        ActionButton{
            x: 17
            y: 360
            img_source_on: "../images/reregister_on.png"
            img_source_off:"../images/reregister.png"
            objectName: "ReregisterButton"
            visible: true

            function executeAction(){
                if(isActive){
                    tools.triggerAction(AppSettings.executeStartReregister);
                } else {
                    tools.triggerAction(AppSettings.executeStopReregister);
                }
            }
        }

        ActionButton{
            x: 17
            y: 425
            img_source_on: "../images/hand_on.png"
            img_source_off: "../images/hand.png"
            objectName: "PickPoints"
            visible: true

            function executeAction(){
                if(isActive){
                    tools.triggerAction(AppSettings.executeStartPickPoints);
                } else {
                    tools.triggerAction(AppSettings.executeStopPickPoints);
                }
            }
        }

        ActionButton{
            x: 17
            y: 490
            img_source_on: "../images/eye_on.png"
            img_source_off: "../images/eye.png"
            objectName: "FreezeFrame"
            visible: true

            function executeAction(){
                if(isActive){
                    tools.triggerAction(AppSettings.executeStartFreeze);
                } else {
                    tools.triggerAction(AppSettings.executeStopFreeze);
                }
            }
        }

        ActionButton{
            x: 17
            y: 555
            img_source_on: "../images/placeholder.png"
            img_source_off: "../images/placeholder.png"
            objectName: "Placeholder1"
            visible: true

            function executeAction(){
                if(isActive){
//                    tools.triggerAction(AppSettings.executeStartFreeze);
                } else {
//                    tools.triggerAction(AppSettings.executeStopFreeze);
                }
            }
        }

        ActionButton{
            x: 17
            y: 620
            img_source_on: "../images/placeholder.png"
            img_source_off: "../images/placeholder.png"
            objectName: "Placeholder2"
            visible: true

            function executeAction(){
                if(isActive){
//                    tools.triggerAction(AppSettings.executeStartFreeze);
                } else {
//                    tools.triggerAction(AppSettings.executeStopFreeze);
                }
            }
        }

        ActionButton{
            x: 17
            y: 685
            img_source_on: "../images/exit.png"
            img_source_off: "../images/exit.png"
            objectName: "ExitButton"
            visible: true

            function executeAction()
            {
                if(isActive){
                    tools.triggerAction(AppSettings.executeExit);
                }
            }
        }
    }
}



