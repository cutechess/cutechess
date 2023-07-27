# Changelog

All notable changes to Cute Chess will be documented in this file.
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [Unreleased](https://github.com/cutechess/cutechess/tree/master/)

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

### Fixed

- Non-Latin 1 characters included in PGN tags (#423)
- Modern Windows theme is now applied correctly

### Changed

- Minimum supported Qt version is 5.15
- Rename the manual `engines.json.5` to `cutechess-engines.json.5`
- Update QCustomPlot to 2.1.1
- `libcutechess` is always built statically with CMake: API / ABI stability of
  the library cannot be guaranteed in the future due to lack of developer
  resources

### Removed

- QMake build system was removed in favor of CMake
- Travis and AppVeyor CI systems were removed in favor of GitHub Actions
- Dynamic linking of `libcutechess`
