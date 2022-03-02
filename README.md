# About ofxLSL
tested on ubuntu 20.04 using liblsl 1.14

# INSTALL LINUX#
* Clone labstreaminglayer and submodules from from [Github](https://github.com/sccn/labstreaminglayer "https://github.com/sccn/labstreaminglayer")
```
git clone --recursive git@github.com:sccn/labstreaminglayer.git
```
* Build and install with cmake-gui
```
cd labstreaminglayer &&
mkdir build && cd build
cmake-gui ..
```
```
make -j`nproc`
```
```
sudo make install 
```
```
sudo ldconfig
```
* Copy lib to addon (use cp -P to preserve Link)
`cp -P  LSL/liblsl/liblsl.* [your location of OF addons]/ofxLSL/libs/labstreaminglayer/lib/linux64`
* Copy include to addon (use cp -P to preserve Link)
`cp -r ../LSL/liblsl/include [your location of OF addons]/ofxLSL/libs/labstreaminglayer/`


# INSTALL MAC#

* download OSX lsl lybrary from [Github](https://github.com/sccn/liblsl/releases)
* Copy dylib to addon 'libs/labstreaminglayer/lib/osx/'
* Copy include to addon 'libs/labstreaminglayer/include/'
* build example with projectgenerator
* add `Run Script` to `Build Phases` with the following lines
```
rsync -aved "$OF_PATH/addons/ofxLSL/libs/labstreaminglayer/lib/osx/liblsl.1.14.0.dylib" "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/Frameworks/";
```
