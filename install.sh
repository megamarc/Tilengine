#!/bin/bash
platform=linux
target=samples
shared="libTilengine.so.3"
cp lib/$platform/$shared /usr/local/lib
ln -sf /usr/local/lib/$shared /usr/local/lib/libTilengine.so
chmod a+x $target/$platform/*
cp $target/$platform/* $target
cp jni/$platform/libTilengineJNI.* $target
cp jni/class/*.class $target
cp python/*.py $target
cd $target
