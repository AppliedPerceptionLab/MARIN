#ifndef constants_h
#define constants_h

//----------------------------------------------------------------//
//                    USER-DEFINED PARAMETERS                     //
//----------------------------------------------------------------//

//Define image capture size:
//(it has to be supported by the camera of the device)
//largest resolution that can be encoded with openh264 is 3840 x 2160
//ipad mini (A2993): (3840, 2160), (3264, 2448), (2592, 1944), (1920, 1440), (1920, 1080), (1440, 1080), (1280, 720), (1024, 768), (960, 540), (640, 480), (480, 360), (352, 288), (192, 144)
//ipad pro 5th gen (A2378): (3840, 2160), (3264, 2448), (2592, 1944), (1920, 1440), (1920, 1080), (1440, 1080), (1280, 720), (1024, 768), (960, 540), (640, 480), (480, 360), (352, 288), (192, 144)
#define CAMERA_WIDTH 1024
#define CAMERA_HEIGHT 768
//Define camera capture format:
//(it needs to be supported by the camera of the device)
#define CAMERA_FORMAT QVideoFrameFormat::Format_NV12
//Define camera focus mode:
//(it needs to be supported by the camera of the device)
#define FOCUS_MODE QCamera::FocusModeAutoNear

//Define size of image that will be received from the neuronav platform:
#define AUGMENTATION_WIDTH 800
#define AUGMENTATION_HEIGHT 600

//set send mode and parameters:
#define OH264_CONFIG_FILE_PATH ""
//available send modes are:
//RC_BITRATE_MODE    ---    for constant bitrate
//RC_QUALITY_MODE    ---    for constant quality
//RC_TIMESTAMP_MODE
#define IMAGE_SEND_MODE RC_TIMESTAMP_MODE
//when on constant bitrate, we also need to set the targeted bitrate (in kbps):
#define TARGET_BIT_RATE 2500000
#define ENCODER_SPEED LOW_COMPLEXITY
#define MIN_QP 0
#define MAX_QP 51
#define LOSSLESS_VIDEO_TRANSMISSION true

#define RATIO_INNER_OUTER_AR_WINDOW 0.8

// Whether to display the camera image where it has strong gradients (over augmented anatomy)
#define USE_GRADIENT_PASSTHROUGH true
// Whether to display the augmentation only in the user defined ellipse in screen space (if false, will display the whole augmentation image)
#define USE_TRANSPARENCY true

//Parameter to select which color is the alpha channel
// 1 : red ; 2 : green ; 3 : blue ; 4 : none
//this hack is used at the moment since H264 can't send RGBA images, so one of the colour channels is used as alpha.
#define ALPHA_CHANNEL_COLOUR 2

#define SERVER_NAME "IBIS"
#define THIS_DEVICE_NAME "MARIN"

//These parameters should be defined by user to reflect the network setup:
#define SERVER_ADDRESS "192.168.1.12"                           //Address of machine on which the neuronav platform runs

enum VideoModes { VIDEOMODE_UNDEFINED, Video_H264, Video_I420, Image_BGRA, Image_RGB, Image_Y };
#define VIDEO_MODE VideoModes::Video_I420
enum TransmissionProtocol { PROTOCOL_UNDEFINED, TCP, UDP };
#define IMAGE_TRANSMISSION_PROTOCOL TransmissionProtocol::TCP
#define COMMANDS_TRANSMISSION_PROTOCOL TransmissionProtocol::TCP
//TODO: Add protocol switch for other streams

#define PORT_SEND_VIDEO 18951                                   // port to send video to
#define PORT_SEND_COMMANDS 18947                                // port to send commands to
#define VIDEO_RECEIVER_PORT 18946                               // port to receive video from
#define COMMANDS_RECEIVER_PORT 18949                            // TCP port to receive commands/status updates from

#define INITIALLY_SENDING_VIDEO true
//TODO: Add toggles for other streams
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
