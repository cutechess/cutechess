#!/bin/bash

mkdir -p package
docker build --no-cache -t cutechess-pkg .
docker run -v $PWD/package:/package -t cutechess-pkg

