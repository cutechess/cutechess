FROM debian:jessie

RUN \
  apt-get update && apt-get install -y \
    build-essential \
    qt5-default \
    qt5-qmake \
    libqt5svg5-dev

# Download and build cutechess
RUN \
  apt-get install -y git && \
  git clone https://github.com/cutechess/cutechess.git && \
  cd /cutechess && \
  qmake && \
  make -j4

COPY control /cutechess_deb_control

# Create .deb package
RUN \
  mkdir -p /cutechess_deb/cutechess && \
  cd /cutechess_deb/cutechess && \
  mkdir -p usr/games && \
  mkdir -p usr/share/man/man6 && \
  mkdir -p usr/share/man/man5 && \
  mkdir -p usr/share/doc/cutechess && \
  cp /cutechess/projects/cli/cutechess-cli usr/games && \
  cp /cutechess/projects/gui/cutechess usr/games && \
  cp /cutechess/docs/cutechess-cli.6 usr/share/man/man6 && \
  gzip usr/share/man/man6/cutechess-cli.6 && \
  cp /cutechess/docs/engines.json.5 usr/share/man/man5 && \
  gzip usr/share/man/man5/engines.json.5 && \
  cp /cutechess/COPYING usr/share/doc/cutechess/copyright && \
  cp /cutechess/README.md usr/share/doc/cutechess/README && \
  mkdir -p DEBIAN && \
  mv /cutechess_deb_control DEBIAN/control && \
  cd /cutechess_deb && \
  mv cutechess cutechess_0.9.0-1_amd64 && \
  dpkg-deb --build cutechess_0.9.0-1_amd64

# Copy the .deb package to the host
CMD cp /cutechess_deb/cutechess*.deb /package

