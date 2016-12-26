TEMPLATE = subdirs
SUBDIRS = chessboard tb sprt mersenne tournamentplayer tournamentpair polyglotbook
win32 {
    SUBDIRS += pipereader
}
