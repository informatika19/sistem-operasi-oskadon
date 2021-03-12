#!/bin/sh
dd if=/dev/zero of=map.img bs=512 count=1
dd if=/dev/zero of=files.img bs=512 count=2
dd if=/dev/zero of=sectors.img bs=512 count=1