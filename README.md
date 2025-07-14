cmake -B build
cd build
cmake --build .
ctest --output-on-failure

git submodule update --init --recursive
