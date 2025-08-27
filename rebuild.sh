#!/bin/bash

BUILD_DIR="build"
EXECUTABLE="game"

echo "Cleaning previous build..."
rm -rf $BUILD_DIR/*
cd $BUILD_DIR

echo "Configuring project..."
cmake ..

echo "Building project..."
make -j$(nproc)

echo "Build complete!"

if [ -f "$EXECUTABLE" ]; then
    echo "Running $EXECUTABLE..."
    ./$EXECUTABLE
else
    echo "Error: Executable not found!"
fi
