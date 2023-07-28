FROM ubuntu:focal

RUN apt-get update && apt-get install -y software-properties-common && add-apt-repository ppa:beineri/opt-qt-5.15.2-focal
RUN apt-get update && apt-get install -y git qt515base qt515svg build-essential cmake mesa-common-dev libglu1-mesa-dev && rm -rf /var/lib/apt/lists/*

ENV QT_BASE_DIR=/opt/qt515
ENV QTDIR=$QT_BASE_DIR
ENV PATH=$QT_BASE_DIR/bin:$PATH
ENV LD_LIBRARY_PATH=$QT_BASE_DIR/lib/x86_64-linux-gnu:$QT_BASE_DIR/lib:$LD_LIBRARY_PATH
ENV PKG_CONFIG_PATH=$QT_BASE_DIR/lib/pkgconfig:$PKG_CONFIG_PATH

# Install dependencies and build tools
RUN \
  apt-get update && apt-get install -y \
    ruby-dev \
    rpm \
    bsdmainutils \
    mandoc && \
  gem install fpm

# Download and build cutechess
RUN git clone https://github.com/cutechess/cutechess.git /cutechess
WORKDIR /cutechess
RUN mkdir build && \
  cd build && \
  cmake .. -DCMAKE_INSTALL_PREFIX=/usr && \
  cmake --build . && \
  make install DESTDIR=/cutechess

# Create .deb and .rpm packages
RUN \
  mkdir -p /cutechess_pkg && \
  cd /cutechess_pkg && \
  mkdir -p usr/games && \
  mkdir -p usr/share/menu && \
  mkdir -p usr/share/pixmaps && \
  mkdir -p usr/share/applications && \
  mkdir -p usr/share/man/man6 && \
  mkdir -p usr/share/man/man5 && \
  mkdir -p usr/share/doc/cutechess && \
  cp /cutechess/usr/bin/cutechess-cli usr/games && \
  cp /cutechess/usr/bin/cutechess usr/games && \
  cp /cutechess/dist/linux/menu/cutechess usr/share/menu && \
  cp /cutechess/projects/gui/res/icons/cutechess_128x128.png usr/share/pixmaps/cutechess.png && \
  cp /cutechess/projects/gui/res/icons/cutechess_32x32.xpm usr/share/pixmaps/cutechess.xpm && \
  cp /cutechess/dist/linux/cutechess.desktop usr/share/applications && \
  cp /cutechess/docs/cutechess-cli.6 usr/share/man/man6 && \
  gzip usr/share/man/man6/cutechess-cli.6 && \
  cp /cutechess/docs/cutechess-engines.json.5 usr/share/man/man5 && \
  gzip usr/share/man/man5/cutechess-engines.json.5 && \
  cp /cutechess/COPYING usr/share/doc/cutechess/copyright && \
  cp /cutechess/README.md usr/share/doc/cutechess/README && \
  mkdir /finished_pkg && \
  export CUTECHESS_VERSION=$(cat /cutechess/.version) && \
  export TODAY=$(date +%Y%m%d) && \
  fpm -s dir -t deb -C /cutechess_pkg \
    -a "amd64" \
    --license "GPLv3" \
    --url "https://github.com/cutechess/cutechess" \
    -n "cutechess" \
    -v "$TODAY+$CUTECHESS_VERSION" \
    --iteration 1 \
    --category "games" \
    -m "Ilari Pihlajisto <ilaripih@gmail.com>" \
    --description "Commandline and graphical interface for playing chess" \
    -d "libc6 (>= 2.27)" \
    -d "libgcc1 (>= 1:8.3.0)" \
    -d "libqt5svg5 (>= 5.15.0)" \
    -d "libqt5core5a (>= 5.15.0)" \
    -d "libqt5gui5 (>= 5.15.0)" \
    -d "libqt5widgets5 (>= 5.15.0)" \
    -d "libqt5printsupport5 (>= 5.15.0)" \
    -d "libqt5concurrent5 (>= 5.15.0)" \
    -d "libstdc++6 (>= 8.3.0)" && \
  mv /cutechess_pkg/*.deb /finished_pkg/ && \
  fpm -s dir -t rpm -C /cutechess_pkg \
    -a "x86_64" \
    --license "GPLv3" \
    --url "https://github.com/cutechess/cutechess" \
    -n "cutechess" \
    -v "$TODAY+$CUTECHESS_VERSION" \
    --iteration 1 \
    --category "Amusements/Games/Board/Chess" \
    -m "Ilari Pihlajisto <ilaripih@gmail.com>" \
    --description "Commandline and graphical interface for playing chess" \
    -d "qt5-qtbase >= 5.15.0" \
    -d "qt5-qtsvg >= 5.15.0" && \
  mv /cutechess_pkg/*.rpm /finished_pkg/

# Create .tar.gz package for cutechess-cli
RUN \
  cd /cutechess && \
  mkdir -p /cutechess_pkg/cutechess-cli && \
  cd /cutechess_pkg && \
  mkdir -p ./cutechess-cli/lib && \
  cp $QT_BASE_DIR/lib/libQt5Core.so.5 cutechess-cli/lib/ && \
  cp /cutechess/usr/bin/cutechess-cli cutechess-cli/ && \
  cp /cutechess/COPYING cutechess-cli/ && \
  cp /cutechess/docs/man-style.css cutechess-cli/ && \
  cp /cutechess/tools/clop-cutechess-cli.py cutechess-cli/ && \
  cp /cutechess/dist/linux/cutechess-cli.sh cutechess-cli/ && \
  cp /cutechess/docs/cutechess-cli.6.html cutechess-cli/ && \
  cp /cutechess/docs/cutechess-cli.6.txt cutechess-cli/ && \
  cp /cutechess/docs/cutechess-engines.json.5.html cutechess-cli/ && \
  cp /cutechess/docs/cutechess-engines.json.5.txt cutechess-cli/ && \
  tar -zcvf cutechess-cli-linux64.tar.gz cutechess-cli && \
  export CUTECHESS_VERSION=$(cat /cutechess/.version) && \
  mv cutechess-cli-linux64.tar.gz /finished_pkg/cutechess-cli-$CUTECHESS_VERSION-linux64.tar.gz

# Copy the .deb package to the host
CMD cp /finished_pkg/cutechess*.* /package

