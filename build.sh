#!/usr/bin/env bash

set -e

OS="$(uname | tr '[:upper:]' '[:lower:]')"
ARCH="$(uname -m)"
PLATFORM="${OS}-${ARCH}"

BUILD_TYPE="debug"
DO_GENERATE=true
DO_CLEAN=false
DO_BUILD=true
GENERATOR="Ninja"

while [[ $# -gt 0 ]]; do
  case $1 in
    -d|--debug)
      BUILD_TYPE="debug"
      shift
      ;;
    -r|--release)
      BUILD_TYPE="release"
      shift
      ;;
    -c|--clean)
      DO_CLEAN=true
      shift
      ;;
    -b|--build-only)
      DO_GENERATE=false
      shift
      ;;
    *)
      echo "Unknown argument: $1"
      echo "Usage: ./build.sh [-d|-r|-c|-b]"
      exit 1
      ;;
  esac
done

BUILD_DIR="_build/${PLATFORM}/${BUILD_TYPE}"
GEN_ARGS="-DCMAKE_BUILD_TYPE=$(echo "$BUILD_TYPE" | tr '[:lower:]' '[:upper:]')"
GENERATOR_ARG="-G ${GENERATOR}"

if [ "$DO_CLEAN" = true ]; then
  echo "Cleaning build directory: $BUILD_DIR"
  rm -rf "$BUILD_DIR"
  exit 0
fi

CACHE_FILE="${BUILD_DIR}/CMakeCache.txt"
if [ -f "$CACHE_FILE" ]; then
  if ! grep -q "CMAKE_GENERATOR:INTERNAL=${GENERATOR}" "$CACHE_FILE"; then
    echo "Generator mismatch: cleaning build directory to prevent CMake error."
    rm -rf "$BUILD_DIR"
  fi
fi

if [ "$DO_GENERATE" = true ]; then
  echo "Generating build system in: $BUILD_DIR"
  cmake -S . -B "$BUILD_DIR" $GENERATOR_ARG \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=1 \
    $GEN_ARGS
fi

if [ "$DO_BUILD" = true ]; then
  echo "Building..."
  cmake --build "$BUILD_DIR" -- -j"$(nproc)"
fi
