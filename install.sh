#!/bin/bash
platform=linux
target=samples
shared="libTilengine.so.2"
cp lib/$platform/$shared /usr/local/lib
ln -sf /usr/local/lib/$shared /usr/local/lib/libTilengine.so
chmod a+x $target/$platform/*
cp $target/$platform/* $target
cp Java/$platform/libTilengineJNI.* $target
cp Java/class/*.class $target
cp Python/*.py $target
cd $target
