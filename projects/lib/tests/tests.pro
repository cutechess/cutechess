TEMPLATE = subdirs
SUBDIRS = chessboard tb sprt mersenne tournamentplayer tournamentpair polyglotbook xboardengine
win32 {
    SUBDIRS += pipereader
}
