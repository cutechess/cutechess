FROM debian:wheezy-backports

# Install dependencies and build tools
RUN \
  apt-get update && apt-get install -y \
    git \
    build-essential \
    qt5-default \
    qt5-qmake \
    libqt5svg5-dev \
    ruby-dev \
    rpm && \
  gem install fpm

# Download and build cutechess
RUN \
  git clone https://github.com/cutechess/cutechess.git && \
  cd /cutechess && \
  qmake && \
  make -j4

COPY menu /cutechess_menu
COPY cutechess.desktop /cutechess.desktop

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
  cp /cutechess/projects/cli/cutechess-cli usr/games && \
  cp /cutechess/projects/gui/cutechess usr/games && \
  cp /cutechess_menu/cutechess usr/share/menu && \
  cp /cutechess/projects/gui/res/icons/cutechess_128x128.png usr/share/pixmaps/cutechess.png && \
  cp /cutechess/projects/gui/res/icons/cutechess_32x32.xpm usr/share/pixmaps/cutechess.xpm && \
  cp /cutechess.desktop usr/share/applications && \
  cp /cutechess/docs/cutechess-cli.6 usr/share/man/man6 && \
  gzip usr/share/man/man6/cutechess-cli.6 && \
  cp /cutechess/docs/engines.json.5 usr/share/man/man5 && \
  gzip usr/share/man/man5/engines.json.5 && \
  cp /cutechess/COPYING usr/share/doc/cutechess/copyright && \
  cp /cutechess/README.md usr/share/doc/cutechess/README && \
  mkdir /finished_pkg && \
  fpm -s dir -t deb -C /cutechess_pkg \
    -a "amd64" \
    --license "GPLv3" \
    --url "https://github.com/cutechess/cutechess" \
    -n "cutechess" \
    -v "20160822+0.8.3+0.9.0" \
    --iteration 1 \
    --category "games" \
    -m "Ilari Pihlajisto <ilari.pihlajisto@mbnet.fi>" \
    --description "Commandline and graphical interface for playing chess" \
    -d "libc6 (>= 2.19)" \
    -d "libgcc1 (>= 1:4.9.2)" \
    -d "libqt5svg5 (>= 5.3.2)" \
    -d "libqt5core5a (>= 5.3.2)" \
    -d "libqt5gui5 (>= 5.3.2)" \
    -d "libqt5widgets5 (>= 5.3.2)" \
    -d "libqt5printsupport5 (>= 5.3.2)" \
    -d "libqt5concurrent5 (>= 5.3.2)" \
    -d "libstdc++6 (>= 4.9.2)" && \
  mv /cutechess_pkg/*.deb /finished_pkg/ && \
  fpm -s dir -t rpm -C /cutechess_pkg \
    -a "x86_64" \
    --license "GPLv3" \
    --url "https://github.com/cutechess/cutechess" \
    -n "cutechess" \
    -v "20160822+0.8.3+0.9.0" \
    --iteration 1 \
    --category "Amusements/Games/Board/Chess" \
    -m "Ilari Pihlajisto <ilari.pihlajisto@mbnet.fi>" \
    --description "Commandline and graphical interface for playing chess" \
    -d "qt5-qtbase >= 5.3.2" \
    -d "qt5-qtsvg >= 5.3.2" && \
  mv /cutechess_pkg/*.rpm /finished_pkg/

# Copy the .deb package to the host
CMD cp /finished_pkg/cutechess*.* /package

