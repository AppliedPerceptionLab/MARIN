pragma Singleton
import QtQuick 2.9

Item {
    property int wHeight
    property int wWidth

//    property string welcomeImagePath: "/images/ibis.png"
    property string welcomeImagePath: "/images/network.png"
    property string networkImagePath: "/images/network.png"
    readonly property int tumourFilter: 1
    readonly property int skinFilter: 2
    readonly property int vesselsFilter: 3

    readonly property int executeOpenSettingsPanel: 1
    readonly property int executeCloseSettingsPanel: 2
    readonly property int executeOpenAnatomyPlanes: 3
    readonly property int executeCloseAnatomyPlanes: 4
    readonly property int executeShowFilters: 5
    readonly property int executeHideFilters: 6
    readonly property int executeStartReregister: 7
    readonly property int executeStopReregister: 8
    readonly property int executeSetModeAugmentationOnly: 9
    readonly property int executeSetModeVideoOnly: 10
    readonly property int executeShowServerConnectionError: 11
    readonly property int executeHideServerConnectionError: 12
    readonly property int executeExit: 13
    readonly property int executeStartFreeze: 14
    readonly property int executeStopFreeze: 15
    readonly property int executeToggleSenderOn: 16
    readonly property int executeToggleSenderOff: 17
    readonly property int executeToggleReceiverOn: 18
    readonly property int executeToggleReceiverOff: 19
    readonly property int executeSetModeAR: 20;
    readonly property int startPhotoRegistration: 21;
    readonly property int stopPhotoRegistration: 22;
    readonly property int executeStartPickPoints: 23;
    readonly property int executeStopPickPoints: 24;

    readonly property color backgroundColor: "#2d3037"

    readonly property int rotateMode: 1
    readonly property int translateMode: 2
    readonly property int resetReregistration: 3

    // Colors
   /* readonly property color buttonColor: "#202227"
    readonly property color buttonPressedColor: "#6ccaf2"
    readonly property color disabledButtonColor: "#555555"
    readonly property color viewColor: "#202227"
    readonly property color delegate1Color: Qt.darker(viewColor, 1.2)
    readonly property color delegate2Color: Qt.lighter(viewColor, 1.2)
    readonly property color textColor: "#ffffff"
    readonly property color textDarkColor: "#232323"
    readonly property color disabledTextColor: "#777777"
    readonly property color sliderColor: "#6ccaf2"
    readonly property color errorColor: "#ba3f62"
    readonly property color infoColor: "#3fba62"

    // Font sizes
    property real microFontSize: hugeFontSize * 0.2
    property real tinyFontSize: hugeFontSize * 0.4
    property real smallTinyFontSize: hugeFontSize * 0.5
    property real smallFontSize: hugeFontSize * 0.6
    property real mediumFontSize: hugeFontSize * 0.7
    property real bigFontSize: hugeFontSize * 0.8
    property real largeFontSize: hugeFontSize * 0.9
    property real hugeFontSize: (wWidth + wHeight) * 0.03
    property real giganticFontSize: (wWidth + wHeight) * 0.04

    // Some other values
    property real fieldHeight: wHeight * 0.08
    property real fieldMargin: fieldHeight * 0.5
    property real buttonHeight: wHeight * 0.08
    property real buttonRadius: buttonHeight * 0.1

    // Some help functions
    function widthForHeight(h, ss)
    {
        return h/ss.height * ss.width;
    }



*/

    function heightForWidth(w, ss)
    {
        return w/ss.width * ss.height;
    }




}
