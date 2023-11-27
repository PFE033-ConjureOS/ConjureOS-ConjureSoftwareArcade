#!/bin/bash

build_dir="$1"

# copier le folder des script python dans le deployment
cp -r python_script $build_dir//src//app//release

# exécuter windeployqt (s'assurer que windeployqt est dans PATH, exemple: C:\Qt\5.15.15\msvc2019_64\bin\)
#windeployqt  --release --qmldir $build_dir --no-translations --no-opengl-sw $build_dir\src\app\release

# exécuté ainsi à la racine du projet Qt:
  # ./ConjureDeploy.sh [dossier du build]