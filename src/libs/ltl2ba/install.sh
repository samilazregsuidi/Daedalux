# delete old build if exists
[ -d build ] && rm -rf build
# create new build directory and build
mkdir build && cd build
cmake ..
make

# Move the built files to the install directory
mkdir -p ../../bin
cp -r ltl2ba ../../bin
# delete the build directory
cd ..
rm -rf build
