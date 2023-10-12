# MARIN: Mobile Augmented Reality Interactive Neuronavigator  

![MARIN displaying segmented vessels over a phantom head](https://github.com/AppliedPerceptionLab/MARIN/assets/17100565/43fd0dee-5f2f-4210-a970-ab4fc2cbbd10)

MARIN displaying segmented vessels over a phantom head.

MARIN is an application that can be used in conjunction with a neuronavigation platform to enable in situ AR guidance on a mobile device. It was developed by [Étienne Léger](https://ap-lab.ca/people/etienneleger/), at the [Applied Perception Lab](https://ap-lab.ca/).

If you use MARIN in your research, cite: 
> Léger, É., Reyes, J., Drouin, S., Popa, T., Hall, J. A., Collins, D. L., Kersten-Oertel, M., "MARIN: an Open Source Mobile Augmented Reality Interactive Neuronavigation System", International Journal of Computer Assisted Radiology and Surgery (2020). https://doi.org/10.1007/s11548-020-02155-6

DISCLAIMER: MARIN is a research tool: It is not intended for normal clinical use, and is not FDA nor CE approved.

MARIN has been tested to work on iOS and in conjunction with [Ibis](https://github.com/IbisNeuronav/Ibis) (with the additional [MARIN plugins](https://github.com/AppliedPerceptionLab/IbisPluginsExtraMARIN)). It should however be easily portable to Android as well, as it relies on Qt to handle mobile input/output, graphics, etc. All dependencies can be built for Android. It should also be possible to use it with Slicer or another neuronavigation platform (provided all necessary plugins be added to that platform), as it uses the [OpenIGTLink protocol](http://openigtlink.org/) for communication.

## Build Instructions:  

For detailed build instructions, see the [wiki](https://github.com/AppliedPerceptionLab/MARIN/wiki/Build-instructions).
