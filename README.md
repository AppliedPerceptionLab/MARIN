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

### Thomas build instructions:
Below is a step-by-step guide for compiling MARIN on an iPad Pro (12.9-inch) (5th generation) using Xcode 13.4 on a MacBook Pro (13-inch, 2017): 

#### OpenIGTLink + OpenH264

On a Mac with Xcode installed, clone the *OpenIGTLink* project by executing the following commands within a base directory *../nist*:

```bash
git clone https://github.com/openigtlink/OpenIGTLink.git
mkdir OpenIGTLink-build
```

<aside>
💡 At the time of writing, the latest version of *OpenIGTLink* is:

- commit 523957b9647059f1a3253da59397cad6046bfe29 (**HEAD -> master**, **origin/master**, **origin/HEAD**)
- Merge: a9e5c6c 87ed15d
- Author: leochan2009 [leochan2009@hotmail.com](mailto:leochan2009@hotmail.com)
- Date: Tue Jul 5 09:24:58 2022 -0400
    
    Merge pull request #253 from Sunderlandkyl/v1_unpack_error
    
    BUG: Fix error when unpacking messages with v1 header
    
</aside>

Then, in the the folder *../nist/OpenIGTLink/SuperBuild,* to the file *External_openh264.cmake*, change line 12 to:

```
SET (OpenH264_INCLUDE_DIR "${CMAKE_BINARY_DIR}/Deps/openh264/codec/api/wels" CACHE PATH "H264 source directory" FORCE)
```

In the same folder, to the file *FindOpenH264.cmake*, change line 28 to:

```
PATH_SUFFIXES include/wels codec/api/wels
```

Next, using CMake (3.23.1), configure the *OpenIGTLink* project with Xcode as the generator and with video streaming and H264 enabled: ** 

![fig1](https://github.com/Tribs2800/MARIN/assets/58409651/97f462da-6d24-46b3-a0ce-902db49fbe5b)

Be sure that the *OpenH264_INCLUDE_DIR* is pointing to the *wels* folder rather than the outdated *svc* folder. Then, within the *../nist/OpenIGTLink-build* directory, clone the openh264 repository and build it by executing the following commands:   

```bash
cd OpenIGTLink-build/Deps
git clone https://github.com/cisco/openh264.git
```

<aside>
💡 At the time of writing, the latest version of openh264 is:

- commit 2e637867315ffeda3cd8970825ec86acc3fc4a30 (**HEAD -> master**, **origin/openh264v2.3.0**, **origin/master**, **origin/HEAD**)
- Author: guangwei [GuangweiWang@users.noreply.github.com](mailto:GuangweiWang@users.noreply.github.com)
- Date: Fri Jun 10 09:51:46 2022 +0800
    
    update release note (#3539)
    
</aside>

Then, build the openh264 project for the OS and architecture of your target device (which in this case is an iPad Pro (12.9-inch) (5th generation)) as follows:

```bash
cd openh264
make OS=ios ARCH=arm64
```

At this points, the following libraries should have been created:

- *../nist/OpenIGTLink-build/Deps/openh264/**libcommon.a***
- *../nist/OpenIGTLink-build/Deps/openh264/**libdecoder.a***
- *../nist/OpenIGTLink-build/Deps/openh264/**libencoder.a***
- *../nist/OpenIGTLink-build/Deps/openh264/**libopenh264.a***
- *../nist/OpenIGTLink-build/Deps/openh264/**libprocessing.a***

Now, back in CMake, configure once again and generate the *OpenIGTLink* project. Verify that the Xcode project file *../nist/OpenIGTLink-build/OpenIGTLink.xcodeproj* was created and open it in Xcode. Select the *OpenIGTLink* project in the project navigator (left pane). From the drop down menu in the main pane, change the default *ALL_BUILD* target to the ***OpenIGTLink* project**. Under *Build Settings → Architectures,* change the *Base SDK* to *iOS* and *c*hange *Architectures* to *Standard Architectures (arm64, armv7)*. Plug your iPad to your Mac and ensure that the your are building the OpenIGTLink project on your iPad (c.f. top window of the main pane in the next figure which should read OpenIGTLink > Your iPad). Then, add the following Xcode projects:

- ../nist/OpenIGTLink-build/Deps/openh264/codec/build/iOS/common/common.xcodeproj
- ../nist/OpenIGTLink-build/Deps/openh264/codec/build/iOS/dec/welsdec/welsdec.xcodeproj
- ../nist/OpenIGTLink-build/Deps/openh264/codec/build/iOS/enc/welsenc/welsenc.xcodeproj
- ../nist/OpenIGTLink-build/Deps/openh264/codec/build/iOS/openh264/openh264.xcodeproj
- ../nist/OpenIGTLink-build/Deps/openh264/codec/build/iOS/processing/processing.xcodeproj

as subprojects by dragging the files in the left pane under the *OpenIGTLink* project. Your Xcode window should look like this:

![fig2](https://github.com/Tribs2800/MARIN/assets/58409651/d8ed8d45-a99f-4942-a8d5-5c0a79274515)

Next, add the subprojects as target dependencies in the *Build Phases* of the ***OpenIGTLink* target** which you must select from the drop down menu in the main pane:

![fig3](https://github.com/Tribs2800/MARIN/assets/58409651/5deb38f5-bc42-41ef-abb0-7c954abb03f8)

Then, for the *OpenIGTLink*, *common*, *welsdec*, *welsenc*, *openh264*, **and *processing* projects and targets, change *Targeted Device Families* to iPad (with code number 2) and the *iOS Deployment Target* to the iOS version of your iPad (e.g. iOS 15.5) under *Build Settings → Deployment*:

![fig4](https://github.com/Tribs2800/MARIN/assets/58409651/9d96a04b-06cb-47ac-842f-7cd375d51809)

Next,

1. To the *OpenIGTLink* target, add the path ../nist/OpenIGTLink-build/Deps/openh264/codec/api/wels to the *Header Search Paths* under *Build Settings→Search Paths* (if not already listed)
2. To the *common* target, add the path ../nist/OpenIGTLink-build/Deps/openh264/codec/common/inc to the *Header Search Paths* under *Build Settings→Search Paths*
3. To the *welsdec* target, add the path ../nist/OpenIGTLink-build/Deps/openh264/codec/decoder/core/inc to the *Header Search Paths* under *Build Settings→Search Paths*

Finally, validate all project settings in the *Issues Navigator* (default left pane) and select *Product→Build* from the Xcode interface. If all goes well (fingers crossed), the following libraries will be created:

- *../nist/OpenIGTLink-build/lib/Debug/**libOpenIGTLink.a***
- *../nist/OpenIGTLink-build/Deps/openh264/codec/build/IOS/common/build/Debug-iphoneos/**libcommon.a***
- *../nist/OpenIGTLink-build/Deps/openh264/codec/build/IOS/dec/welsdec/build/Debug-iphoneos/**libwelsdec.a***
- *../nist/OpenIGTLink-build/Deps/openh264/codec/build/IOS/enc/welsenc/build/Debug-iphoneos/**libwelsenc.a***
- *../nist/OpenIGTLink-build/Deps/openh264/codec/build/IOS/processing/build/Debug-iphoneos/**libprocessing.a***

#### Libyuv

Since I ran out of storage on my Mac, the *libyuv* source code was downloaded and built on an external hard drive. The following commands were executed within the base directory /Volumes/Expansion/ThomasMac/Documents:

```bash
# uncomment if depot tools is not installed on your system
#git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
##replace path below to the location where depot tools is installed on your system
#export PATH=/Volumes/Expansion/ThomasMac/Documents/depot_tools:$PATH

gclient config --name src https://chromium.googlesource.com/libyuv/libyuv
gclient sync
vim .gclient # or text editor of choice
```

Add “; target_os=[’ios’];” to the end of .gclient as follows:

![fig5](https://github.com/Tribs2800/MARIN/assets/58409651/f48e771a-067d-4cc3-83f7-c5977e687c11)

Then, install Python on your system, run ../*Applications/Python X.X/Install Certificates.command* and execute the following:

```bash
gclient sync
cd src
gn gen out/Debug "--args=is_debug=true target_os=\"ios\" ios_enable_code_signing=false target_cpu=\"arm64\""
ninja -C out/Debug libyuv
```

This will create the following libraries:

- *../Volumes/Expansion/ThomasMac/Documents/src/out/Debug/obj/**libyuv_internal.a***
- *../Volumes/Expansion/ThomasMac/Documents/src/out/Debug/obj/**libyuv_neon.a***

##### Installing QT

5.14.2 ≤ Version ≤ 5.15

[Download Qt: Get Qt Online Installer](https://www.qt.io/download-qt-installer?hsCtaTracking=99d9dd4f-5681-48d2-b096-470725510d34%7C074ddad0-fdef-4e53-8aa8-5e8a876d6ab4)

*QT* was also downloaded and built on the external hard drive in the directory: */Volumes/Expansion/ThomasMac/Qt*

##### Building MARIN

Within a base directory ../nist, clone the *MARIN* repository by **executing the following commands:

```bash
git clone https://github.com/AppliedPerceptionLab/MARIN.git
cd MARIN
mkdir lib
cd ..
```

<aside>
💡 At the time of writing, the latest version of MARIN is:

- commit 79ddde6ab861e0dce7ff2e6b275c341371135754 (**HEAD -> master**, **origin/master**, **origin/HEAD**)
- Author: Étienne Léger [17100565+errollgarner@users.noreply.github.com](mailto:17100565+errollgarner@users.noreply.github.com)
- Date: Thu Apr 23 15:26:49 2020 -0400
    
    Updated readme.
    

**This version compiles but fails to display the iPad camera. A fork has been created and will be pushed to the master branch soon.**

</aside>

Then, copy the libraries created in previous sections to *MARIN/lib* folder using the following commands:

```bash
# provided that all libraries have been created
mkdir *MARIN/lib/openIGTLink* && cp *OpenIGTLink-build/lib/Debug/libOpenIGTLink.a ****MARIN/lib/openIGTLink*
mkdir *MARIN/lib/common* && cp *OpenIGTLink-build/Deps/openh264/codec/build/IOS/common/build/Debug-iphoneos/libcommon.a ****MARIN/lib/common*
#mkdir *MARIN/lib/welsdec* && cp *OpenIGTLink-build/Deps/openh264/codec/build/IOS/dec/welsdec/build/Debug-iphoneos/libwelsdec.a ****MARIN/lib/welsdec*
mkdir *MARIN/lib/welsdec* && cp *OpenIGTLink-build/Deps/openh264/libdecoder.a ****MARIN/lib/welsdec/libwelsdec.a*
mkdir *MARIN/lib/welsenc* && cp *OpenIGTLink-build/Deps/openh264/codec/build/IOS/enc/welsenc/build/Debug-iphoneos/libwelsenc.a ****MARIN/lib/welsenc*
mkdir *MARIN/lib/processing* && cp *OpenIGTLink-build/Deps/openh264/codec/build/IOS/processing/build/Debug-iphoneos/libprocessing.a ****MARIN/lib/processing*

#replace the appropriate paths below to locations where the libyuv libraries are located on your system 
**mkdir *MARIN/lib/libyuv_internal* && cp */Volumes/Expansion/ThomasMac/Documents/src/out/Debug/obj/libyuv_internal.a MARIN/lib/libyuv_internal*
mkdir *MARIN/lib/libyuv_neon* && cp */Volumes/Expansion/ThomasMac/Documents/src/out/Debug/obj/libyuv_neon.a MARIN/lib/libyuv_neon*
```

Then, in .*./nist/MARIN/**MARIN.pro*** (starting on line 42), specify the local paths to *OpenIGTLink*, *OpenIGTLink-build* and *libyuv:*

```
INCLUDEPATH += \
    ../OpenIGTLink/Source \
    ../OpenIGTLink/Source/VideoStreaming \
    ../OpenIGTLink/Source/igtlutil \
    ../OpenIGTLink-build \
    ../OpenIGTLink-build/lib/Debug \
    ../OpenIGTLink-build/Deps/openh264 \
    ../OpenIGTLink-build/Deps/openh264/codec \
    ../OpenIGTLink-build/Deps/openh264/codec/api/wels \
    /Volumes/Expansion/ThomasMac/Documents/src/include \
    /Volumes/Expansion/ThomasMac/Documents/src/include/libyuv
```

Make sure to replace ../OpenIGTLink-build/Deps/openh264/codec/api/**svc** → **wels**. Also in .*./nist/MARIN/**MARIN.pro***, change the way MARIN looks for the *libyuv* dependencies by modifying the code on lines 58-69 as follows:

```
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/libyuv_internal/release/ -lyuv_internal
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/libyuv_internal/debug/ -lyuv_internal
else:unix: LIBS += -L$$PWD/lib/libyuv_internal/ -lyuv_internal

INCLUDEPATH += $$PWD/lib/libyuv_internal
DEPENDPATH += $$PWD/lib/libyuv_internal

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/libyuv_internal/release/libyuv_internal.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/libyuv_internal/debug/libyuv_internal.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/libyuv_internal/release/yuv_internal.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/libyuv_internal/debug/yuv_internal.lib
else:unix: PRE_TARGETDEPS += $$PWD/lib/libyuv_internal/libyuv_internal.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/libyuv_neon/release/ -lyuv_neon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/libyuv_neon/debug/ -lyuv_neon
else:unix: LIBS += -L$$PWD/lib/libyuv_neon/ -lyuv_neon

INCLUDEPATH += $$PWD/lib/libyuv_neon
DEPENDPATH += $$PWD/lib/libyuv_neon

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/libyuv_neon/release/libyuv_neon.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/libyuv_neon/debug/libyuv_neon.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/libyuv_neon/release/yuv_neon.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/libyuv_neon/debug/yuv_neon.lib
else:unix: PRE_TARGETDEPS += $$PWD/lib/libyuv_neon/libyuv_neon.a
```

Next, convert the .*./nist/MARIN/**MARIN.pro*** file to an Xcode project file using the following command:

```bash
cd MARIN

#replace path below to the location where Qt is installed on your system
#Note: the command below is specific to the iOS operating system
/Volumes/Expansion/ThomasMac/Qt/5.15.2/ios/bin/qmake -spec macx-ios-clang MARIN.pro
```

Open the *../nist/MARIN/MARIN.xcodeproj* Xcode project file and select the *MARIN* project in the project navigator (left pane). To the ***MARIN* project**, under *Build Settings → Architectures,* change the *Base SDK* to *iOS* and *c*hange all *Architectures* to *Standard Architectures*. Make sure that the same parameters are set for the ***MARIN* target**. Plug your iPad to your Mac and ensure that the your are building the OpenIGTLink project on your iPad (c.f. top window of the main pane in the next figure which should read *MARIN* > Your iPad). 

Next, open the *../nist/MARIN/MARIN.xcodeproj* Xcode project file and select the **MARIN project** in the project navigator (left pane). Under *Build Settings → Architectures,* change the *Base SDK* to *iOS* and change all architectures to to *Standard Architectures*. Apply these same settings to the **MARIN target**. Then, plug your iPad device and ensure that you are building the *MARIN* project on your iPad (c.f. top window of the main pane in the next figure which should read *MARIN* > Your iPad).

![fig6](https://github.com/Tribs2800/MARIN/assets/58409651/1e992f86-1e75-442f-8c72-c303a9ee2425)

Then, to the *MARIN* project, change *Targeted Device Families* to iPad (with code number 2) and the *iOS Deployment Target* to the iOS version of your iPad (e.g. iOS 15.5) under *Build Settings → Deployment.*

![fig7](https://github.com/Tribs2800/MARIN/assets/58409651/55407fef-bee9-4f64-8db3-226e33857fd7)

Also to the *MARIN* project, change Build Settings → Build Options → Enable Bitcode to *No*. Make sure that these changes also take effect on the *MARIN* target. 

Next,

1. In .*./nist/MARIN/src/**glwidget.h***: on line 10, change <receiver.h> → “receiver.h” and on line 12, change <constants.h> → “constants.h”
2. In *../nist/MARIN/src/**glwidget.cpp***: on line 2, change <qmlmainwindow.h> → “qmlmainwindow.h”

Then, to .*./nist/MARIN/src/**receiver.cpp***, modify lines 90 and 106 such that the *Receive* class accepts a third argument *timeout* as follows:

```cpp
//line 90 (on original file)
bool timeout(false);
int r = commandsSocket->Receive( headerMsg->GetPackPointer(), headerMsg->GetPackSize(), timeout);

//...

//line 106 (on original file)
bool timeout(false);
int sizei = commandsSocket->Receive(statusMsg->GetPackBodyPointer(), statusMsg->GetPackBodySize(), timeout);

//note warning of implicit conversion loses integer precision
//function returns 'int' but should return 'igtlUint64' 
//(aka 'unsigned long long') 
```

Next, set the desired network and mobile device parameters in the *configs* folder of *MARIN*. 

In the Xcode interface, click on the hammer icon in the center pane (c.f. figure below) and change *Legacy Build System (Deprecated)* to *New Build System (Default).* 

<img width="382" alt="fig8" src="https://github.com/Tribs2800/MARIN/assets/58409651/093030a5-0910-41b0-9d7f-2b7c5895ec7e">

![fig9](https://github.com/Tribs2800/MARIN/assets/58409651/42af3490-fde7-4257-985e-c485ffe7c54a)


Finally, validate the project settings of MARIN in the *Issues Navigator* (default left pane), wait for Xcode to process all files (c.f. message left of hammer on the figure above) and select *Product→Build* from the Xcode interface. If Xcode complains about not being able to find certain files in MARIN/ which exists in that directory, proceed by selecting *Product→Clean Build Folder* in the Xcode interface and re-run *Product→Build*.
