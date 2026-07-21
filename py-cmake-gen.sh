#!/bin/bash
set -e  # exit immediately on any failure, don't silently continue

cd "$(dirname "$0")"  # ensure we're always running relative to this script's location

echo "Activating venv..."
source .venv/bin/activate

echo "Installing bpy (editable)..."
pip install --no-build-isolation -e .

echo "Generating stubs..."
python -m pybind11_stubgen bpy -o .

echo "Done."