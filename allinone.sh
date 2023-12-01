#!/bin/sh

find ./ -type  f  -name "*.hpp" | sed 's/^\.\/include\///' | sed 's/^/#include </' | sed 's/$/>/' | sed '/#include <com\/sun\/star\/drawing\/CaptionEscapeDirection.hpp>/ s/^/\/\/ /'