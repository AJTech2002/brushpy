#!/bin/bash
# Generates an Xcode project in xcode-build/ from the CMakeLists.txt.
# Open xcode-build/BrushPY.xcodeproj to build, run, and use the Metal debugger.

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
OUT="$SCRIPT_DIR/xcode-build"

cmake -S "$SCRIPT_DIR" -B "$OUT" -G Xcode

echo ""
echo "Done. Open the project with:"
echo "  open \"$OUT/BrushPY.xcodeproj\""
