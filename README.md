cmake -B build
cd build
cmake --build .
ctest --output-on-failure
./tests/runTests   

git submodule update --init --recursive
