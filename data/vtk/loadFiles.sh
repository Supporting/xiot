#!/bin/bash

VTKX3DLOADER=/home/kristian/development/build/projects/collaviz/xiot/branches/vtk_integration/debug/bin/vtkX3DLoader

find . -name "*.x3d" | while read i
do
  echo "Loading $i"
  $VTKX3DLOADER $i
done
