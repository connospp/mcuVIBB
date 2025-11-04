#!/bin/sh

UI_FILES=""
for f in *.ui
do 
    UI_FILES="${UI_FILES} ${f}"
done

echo "designer-qt5 ${UI_FILES}"
designer-qt5 ${UI_FILES}
