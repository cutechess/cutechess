# Changelog

All notable changes to Cute Chess will be documented in this file.
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [1.3.1](https://github.com/cutechess/cutechess/releases/tag/v1.3.1) - 2023-07-30

### Added

- CodeQL and Coverity code analysis support (#763)

### Fixed

- Windows: Terminal window pops up with release builds (#765)
- Crash when terminating match with SIGINT (#764)

## [1.3.0](https://github.com/cutechess/cutechess/releases/tag/v1.3.0) - 2023-07-28

### Added

- New variants:
    - Knight-Relay Chess by @ddugovic
    - Cfour (7x6, line up 4 to win) by @alwey
    - Tic-tac-toe by @alwey
    - Gomoku Free-Syle (5 in a row to win) by @alwey
    - Gomoku (15x15, exactly 5 in a row to win) by @alwey
- Qt 6 support (Qt 5 support is unchanged)
- CMake build support
    - Library detection for both Qt 5 and Qt 6
    - `make install` target for Unix-like operating systems
    - CTest compatible test suite
    - Default build type is release with debug info (RelWithDebInfo)
- The `--version` command-line option includes basic system information to aid
  debugging
- Unit tests now cover parts of the Xboard protocol parsing
- HTML and text versions of the manual pages are included with the Windows
  installer and zip releases
- `release-tool`, a developer tool, for creating consistent releases
- Initial `clang-format` configuration
- AppImage generation for releases
- Setting to scale chess engine timeouts periods by @kiudee
- Draw nicer move arrows by @kdave

### Fixed

- Non-Latin 1 characters included in PGN tags (#423)
- Modern Windows theme is now applied correctly
- FRC: Fixed incorrect "illegal move" adjudications when castling (#616, #637, #755, #756)
- Fixes for Shogi:
    - Fix result strings for Shogi, Dobutsu Shogi and Judkins Shogi
    - ShogiBoard: Assert both sides have exactly one King
    - Add logic to ShogiBoard to keep track of perpetuals
    - Add comment describing the 27-point rule to break Impassé (FESA rule 5.3)
    - Fix Chick drop rules in Dobutsu Shogi
    - Fix symbol of promoted Chick in Dobutsu Shogi
    - Dobutsu: Modify adjudication of "try" win by Lion on farthest rank
- Implement Dirichlet prior for SPRT; handle case where one or more of the counts (WDL) is 0 (#360)

### Changed

- Minimum supported Qt version is 5.15
- Rename the manual `engines.json.5` to `cutechess-engines.json.5`
- Update QCustomPlot to 2.1.1
- `libcutechess` is always built statically with CMake: API / ABI stability of
  the library cannot be guaranteed in the future due to lack of developer
  resources
- Default build now includes debug symbols

### Removed

- QMake build system was removed in favor of CMake
- Travis and AppVeyor CI systems were removed in favor of GitHub Actions
- Dynamic linking of `libcutechess`
