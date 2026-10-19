# Fathom

[Fathom](https://github.com/jdart1/Fathom) probes Syzygy tablebases for tablebase adjudication.

Vendored version: jdart1/Fathom commit [c9c6fef](https://github.com/jdart1/Fathom/commit/c9c6fef) (2025-12-23).

`src/tbprobe.c`, `src/tbprobe.h`, `src/tbchess.c` and `src/stdendian.h` are unmodified copies from that commit. Only `tbprobe.c` is compiled, since it includes `tbchess.c`.

`src/tbconfig.h` differs from upstream only in defining:

- `TB_NO_HELPER_API`, since cutechess uses only the probe functions
- `TB_NO_HW_POP_COUNT`, so builds don't depend on the CPU's popcount instruction

To update, copy those files and `LICENSE` from a newer Fathom commit, reapply the two `tbconfig.h` defines, and update the commit above.
