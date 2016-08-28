#!/bin/bash

mkdir -p package
docker build -t cutechess-pkg .
docker run -v $PWD/package:/package -t cutechess-pkg

