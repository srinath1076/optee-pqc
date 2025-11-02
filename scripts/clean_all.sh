#!/bin/bash
find .. -type f -name "*.o" -delete
find .. -type f -name "*.a" -delete
find .. -type f -name "*.so" -delete
rm -rf ../liboqs/build-ta ../ta/CMakeFiles ../host/CMakeFiles