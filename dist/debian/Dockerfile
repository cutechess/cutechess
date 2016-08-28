FROM debian:wheezy-backports

# Install dependencies and build tools
RUN \
  apt-get update && apt-get install -y \
    build-essential \
    qt5-default \
    qt5-qmake \
    libqt5svg5-dev \
    imagemagick

# Download and build cutechess
RUN \
  apt-get install -y git && \
  git clone https://github.com/cutechess/cutechess.git && \
  cd /cutechess && \
  qmake && \
  make -j4

COPY control /cutechess_deb_control
COPY menu /cutechess_menu
COPY cutechess.desktop /cutechess.desktop

# Create .deb package
RUN \
  mkdir -p /cutechess_deb/cutechess && \
  cd /cutechess_deb/cutechess && \
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
  convert usr/share/pixmaps/cutechess.png usr/share/pixmaps/cutechess.xpm && \
  cp /cutechess.desktop usr/share/applications && \
  cp /cutechess/docs/cutechess-cli.6 usr/share/man/man6 && \
  gzip usr/share/man/man6/cutechess-cli.6 && \
  cp /cutechess/docs/engines.json.5 usr/share/man/man5 && \
  gzip usr/share/man/man5/engines.json.5 && \
  cp /cutechess/COPYING usr/share/doc/cutechess/copyright && \
  cp /cutechess/README.md usr/share/doc/cutechess/README && \
  mkdir -p DEBIAN && \
  mv /cutechess_deb_control DEBIAN/control && \
  cd /cutechess_deb && \
  mv cutechess cutechess_20160822+0.8.3+0.9.0-1_amd64 && \
  dpkg-deb --build cutechess_20160822+0.8.3+0.9.0-1_amd64

# Copy the .deb package to the host
CMD cp /cutechess_deb/cutechess*.deb /package

