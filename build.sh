echo Generating filter files from input
./filter_preprocessor
rm -r sources/filters
mv temp/filters sources/
rmdir temp

echo Building kernel
mkdir -p build >/dev/null 2>&1
cd build
cmake ..
make -j$(nproc)
objcopy --srec-forceS3 rpiscgms -O srec rpiscgms.srec
echo "G" >> rpiscgms.srec
echo Build finished

