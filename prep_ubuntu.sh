#!/bin/bash

set -e
apt-get update
apt-get install software-properties-common
add-apt-repository ppa:ubuntu-toolchain-r/test
apt-get update

# install dependencies
apt-get install git-core build-essential g++-7 zlib1g-dev libasound2-dev liblilv-dev npm libsqlite3-dev
npm install -g inliner

if [ ! -f /usr/bin/node ]; then
  ln -fs /usr/bin/nodejs /usr/bin/node
fi
