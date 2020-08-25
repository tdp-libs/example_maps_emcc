
This example program demonstrates using tp_maps to build a Javascript 3D display for use on web 
pages. Emscripten is used to compile C++ to web assembly.

## Prerequisites 
* tp_tools [Installation instructions](https://github.com/tdp-libs/tp_tools)
* [Emscripten](https://github.com/tdp-libs/tp_maps_emcc)

## Clone
```
mkdir maps_emcc
cd maps_emcc
git clone https://github.com/tdp-libs/example_maps_emcc.git
cp example_maps_emcc/Makefile.top Makefile
tdpUpdate
cd ..

```

## Prepare Emscripten
Depending on how you installed Emscripten you will need to add it to your env. See the 
[README.md](https://github.com/tdp-libs/tp_maps_emcc) for more details.
```
source ./emsdk_env.sh

```

## Build
This builds the js binary and creates a symlink to the example html document.
```
cd maps_emcc
make -j`nproc`
ln -snf ../maps_emcc/example_maps_emcc/htm ../build-example_maps_emcc/htm
cd ..

```

## Run
For this to work properly you need to serve these files from a web server, the following runs a 
simple HTTP server to serve the files.
```
cd build-example_maps_emcc/htm
python -m SimpleHTTPServer
cd ..

```
The previous command will run a web server and will print out the port that it is running on, open
that port in a web browser. 

For example open: http://localhost:8000
