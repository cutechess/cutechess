#!/bin/bash

mkdir -p package
docker build -t cutechess-deb .
docker run -v $PWD/package:/package -t cutechess-deb

