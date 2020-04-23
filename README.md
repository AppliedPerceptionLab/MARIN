# MARIN: Mobile Augmented Reality Interactive Neuronavigator  

MARIN is an application that can be used in conjunction with a neuronavigation platform to enable in situ AR guidance on a mobile device. It was developed by [Étienne Léger](https://ap-lab.ca/people/etienneleger/), at the [Applied Perception Lab](https://ap-lab.ca/).

If you use MARIN in your research, cite: 
> Léger, É., Reyes, J., Drouin, S., Popa, T., Hall, J. A., Collins, D. L., Kersten-Oertel, M., "MARIN: an Open Source Mobile Augmented Reality Interactive Neuronavigation System", International Journal of Computer Assisted Radiology and Surgery (2020). https://doi.org/10.1007/s11548-020-02155-6

DISCLAIMER: MARIN is a research tool: It is not intended for normal clinical use, and is not FDA nor CE approved.

MARIN has been tested to work on iOS and in conjunction with [Ibis](https://github.com/IbisNeuronav/Ibis) (with the additional [MARIN plugins](https://github.com/AppliedPerceptionLab/IbisPluginsExtraMARIN)). It should however be easily portable to Android as well, as it relies on Qt to handle mobile input/output, graphics, etc. All dependencies can be built for Android. It should also be possible to use it with Slicer or another neuronavigation platform (provided all necessary plugins be added to that platform), as it uses the [OpenIGTLink protocol](http://openigtlink.org/) for communication.

## Build Instructions:  

### Dependencies  
* [Qt](https://www.qt.io/) >= 5.14.2  
(prior to that version, QTBUG-79935 will cause problems.)  
* [libyuv](https://chromium.googlesource.com/libyuv/libyuv/)  
* [OpenIGTLink](https://github.com/openigtlink/OpenIGTLink)  
* [OpenH264](https://github.com/cisco/openh264)  (Can be built through the SuperBuild of OpenIGTLink.)

It is up to the user to build these libraries and place them in the lib directories, where MARIN can find them. The expected structure is:  
./lib/common/libcommon.a  
./lib/libyuv/libyuv.a  
./lib/openIGTLink/libOpenIGTLink.a  
./lib/processing/libprocessing.a  
./lib/welsdec/libwelsdec.a  
./lib/welsenc/libwelsenc.a  

### Configuration
Configuration files are placed in the configs folder. It is up to the user to set the desired parameters in those files to fit their specific network and mobile device. 

### iOS-specific build instructions:  

######   OpenIGTLink  
git clone https://github.com/openigtlink/OpenIGTLink.git  
mkdir build_oigtl  
cd build_oigtl  
cmake -G Xcode -DOPENIGTLINK_USE_H264=true ../openigtlink  
cd in oh264 directory and build for arm:  
make OS=ios ARCH=arm64  
As of last version, it is needed to add "build_ios/Deps/openh264/codec/common/inc" to the header include paths in xcode for the common project.  

Open the project in Xcode  
Change base SDK to iOS  
Add all subprojects to the OpenIGTLink project (and add them to target dependencies)  
(projects are openh264, commons, processing, welsenc and welsdec)  
(they should be in ⁨build_oigtl/Deps⁩/openh264⁩/codec⁩/build⁩/iOS⁩/)  
In the Xcode interface, select OpenIGTLink target and build it  
Then copy all created .a to the corresponding locations in the MARIN lib folder  
