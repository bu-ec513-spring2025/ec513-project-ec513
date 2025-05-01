#!/bin/bash

echo "running benchmark"

build/X86/gem5.opt \
../../x86-spec-cpu2017-benchmarks-warmup.py \
--image ../disk-image/spec-2017/spec-2017-image/spec-2017 \
--partition 1 \
--benchmark 523.xalancbmk_r  \
--size test




