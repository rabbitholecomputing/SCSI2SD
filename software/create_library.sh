#!/bin/sh

libtool -static -o libscsi2sd-util.a build/mac/*.o
libtool -static -o libz.a build/mac/zlib/*.o
libtool -static -o libzipper.a build/mac/libzipper/*.o