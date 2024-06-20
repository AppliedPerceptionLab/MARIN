#ifndef constants_h
#define constants_h

//----------------------------------------------------------------//
//                    USER-DEFINED PARAMETERS                     //
//----------------------------------------------------------------//

//Define image capture size:
//(it has to be supported by the camera of the device)
#define CAMERA_WIDTH 1024
#define CAMERA_HEIGHT 768
//Define camera capture format:
//(it needs to be supported by the camera of the device)
#define CAMERA_FORMAT QVideoFrame::Format_Invalid
//Define camera focus mode:
//(it needs to be supported by the camera of the device)
#define FOCUS_MODE ContinuousFocus

//Define size of image that will be received from the neuronav platform:
#define AUGMENTATION_WIDTH 800
#define AUGMENTATION_HEIGHT 600

#define HIGH_DPI_SCALING true

//set send mode and parameters:
//available send modes are:
//RC_BITRATE_MODE    ---    for constant bitrate
//RC_QUALITY_MODE    ---    for constant quality
//RC_TIMESTAMP_MODE
#define IMAGE_SEND_MODE RC_TIMESTAMP_MODE
//when on constant bitrate, we also need to set the targeted bitrate:
#define TARGET_BIT_RATE 8000000
//this is ~977 kB/s

#define RATIO_INNER_OUTER_AR_WINDOW 0.8

//Parameter to select which color is the alpha channel
// 1 : red ; 2 : green ; 3 : blue ; 4 : none
//this hack is used at the moment since H264 can't send RGBA images, so one of the colour channels is used as alpha.
#define ALPHA_CHANNEL_COLOUR 2

#define SERVER_NAME "IBIS"
#define THIS_DEVICE_NAME "MARIN"

//These parameters should be defined by user to reflect the network setup:
#define SERVER_ADDRESS "192.168.1.12"                           //Address of machine on which the neuronav platform runs

enum VideoModes { VIDEOMODE_UNDEFINED, H264, I420, Image_BGRA, Image_RGB, Image_Y };
#define VIDEO_MODE VideoModes::H264
enum TransmissionProtocol { PROTOCOL_UNDEFINED, TCP, UDP };
#define IMAGE_TRANSMISSION_PROTOCOL TransmissionProtocol::UDP
#define COMMANDS_TRANSMISSION_PROTOCOL TransmissionProtocol::TCP
//TODO: Add protocol switch for other streams

#define PORT_SEND_VIDEO 18951                                 // port to send video to
#define PORT_SEND_COMMANDS 18947                              // port to send commands to
#define VIDEO_RECEIVER_PORT 18946                               //UDP port to receive video from
#define COMMANDS_RECEIVER_PORT 18949                            //TCP port to receive commands/status updates from

#define INITIALLY_SENDING_VIDEO false
//----------------------------------------------------------------//
//----------------------------------------------------------------//




//----------------------------------------------------------------//
//                        DATA STRUCTURES                         //
//----------------------------------------------------------------//
//        (this part wouldn't typically need to be changed)

//Commands structure definition:
enum CommandName { COMMAND_UNDEFINED, ToggleAnatomy, ToggleQuadView, NavigateSlice, ReregisterAR, RotateView, FreezeFrame, ResetReregistration, ArbitraryCommand };
struct Command {    CommandName c ;
                    double param1;
                    double param2;
                    double param3;
                    int param4;
                    int param5;
                    std::string param6;
};
//      Description of parameters used in commands:
//      - ToggleAnatomy: param4 holds the anatomy ID number and param5 a boolean setting the visibility
//      - ToggleQuadview: param4 holds a boolean setting quad view ( true is enabled and false disabled )
//      - ReregisterAR: param1 and param2 are translation in x and y and param3 is rotation angle in degrees
//      - RotateView: param1 and param2 are translation in x and y and param3 is the zoom factor
//      - FreezeFrame: param4 holds a boolean seeting the freeze frame ( true is enabled and false disabled )
//      - ResetReregistration: no parameters required
//      - ArbitraryCommand: sends a string

//Display modes definitions:
enum DisplayMode { DEFAULT, AugmentationOnly, CameraOnly, AugmentedReality };
//Slice views:
enum SliceView { Transverse, Coronal, Sagittal };

#endif
