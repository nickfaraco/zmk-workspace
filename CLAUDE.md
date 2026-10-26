# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

A personal ZMK firmware config, forked from [urob/zmk-config](https://github.com/urob/zmk-config) (remote `urob`; `origin` is the user's fork). Our commits are kept rebased on top of `urob/main`. `README.md`, `AGENTS.md` and `docs/` are upstream's: their nix/direnv/just setup and HRM/combo explanations apply here, but **their keymap structure does not** (no `ZMK_BASE_LAYER` adapters, no per-board 34-key entry keymaps, different layers). This file supersedes them where they differ.

Targets (see `build.yaml`):

- **chocofi**: split, `nice_nano//zmk` (nice!nano v2), wireless
- **zilpzalp**: unibody, `xiao_rp2040//zmk`, wired, 28 physical keys

ZMK tracks a pinned commit of `main` (Zephyr 4.1, HWMv2 board names, `name@rev//zmk` syntax), not a release tag. `config/west.yml` is maintained by `pin-west`: never hand-edit revisions. Use `just bump-west` instead, and run `pin-west pin` after adding or removing a module. Custom behaviors come from urob's modules (helpers, auto-layer, adaptive-key, leader-key, tri-state, unicode), checked out under `modules/zmk/`.

## Commands

The environment comes from `nix` + `direnv` (`.envrc` → `use flake`). If `just`/`west` are missing or stale, prefix commands with `nix develop --command`.

```sh
just init                 # first-time: west init/update/zephyr-export
just sync                 # west update after manifest changes
just list                 # list build targets parsed from build.yaml
just build all            # build every target; firmware lands in firmware/*.uf2
just build zilpzalp -p    # targets matching expr (case-insensitive); extra args go to west (-p = pristine)
just draw                 # regenerate draw/base.svg + draw/overview.svg via keymap-drawer
just bump-west            # pin-west bump + sync (update ZMK/modules)
just bump-nix             # nix flake update (Zephyr SDK / python deps)
just format <files|dir>   # dts-format (near no-op on ZMK_LAYER grids)
just clean
```

Validate keymap changes by building. `just test` is a snapshot harness for developing the ZMK *modules*, not this keymap. Build warnings that come from ZMK/Zephyr itself and can be ignored: `Deprecated symbol KSCAN`, the `behavior_tap_dance.c` const warning, `ZMK_USB` on the peripheral half, the RP2040 dtc `memory@20000000` and `SETTINGS_NVS` notes.

CI: `.github/workflows/build-nix.yml` builds on pushes/PRs touching `config/**` or `build.yaml`. `bump-west.yml` opens weekly pin-bump PRs.

## Keymap architecture

All boards share a single keymap: **`config/base.keymap`**. Each per-board keymap (`chocofi.keymap`, `zilpzalp.keymap`, `dummy.keymap`) only sets feature `#define`s and then `#include "base.keymap"`:

- `WIRELESS` (1/0): enables the BT/output bindings (`BTL1`/`BTL2` on the Adj layer) and BT macros in `macros.dtsi`
- `MOUSE_KEYS`: includes `common/mouse.dtsi` and the Mouse layer (the board's `.conf` also needs `CONFIG_ZMK_POINTING=y`)
- `ALPHA_13332` / `ALPHA_23332` / neither: selects how the outer "extra" keys (`LTL`, `LBL`, `LBR`, `RTR`, `RBL`, `RBR`) are bound, in `common/extra_keys.h`. `ALPHA_13332` sets them to `&none`, which leaves 28 active keys. Both current boards use it, and the letters they would carry (V, Q, Z…) are reached through combos.
- `DUMMY_KEEB`: used by `dummy.keymap`, which `just draw` parses, to show every key

The keymap is always written as a full **36-key 3x5+3 grid** using `zmk-helpers` key labels (`LT0…RH2`, from `zmk-helpers/key-labels/36.h`, which also provides `KEYS_L`/`KEYS_R`/`THUMBS`). Boards with fewer keys map their physical matrix onto that grid in their shield overlay. For example, `config/hardware/boards/shields/zilpzalp/zilpzalp.overlay` defines `square_transform`, which places nonexistent keys on unused row 9. When you change bindings, keep every layer at 36 positions, and refer to keys in combos/HRMs by label, never by numeric index.

Layers: indices are `#define`d at the top of `base.keymap` (`BASE`, `BASE_WIN`, `NUM`, `NUMF`, `NAV`, `NAVW`, `GAME`, `ADJ`, `MSE`) and used in bindings/combos. The `ZMK_LAYER(...)` blocks must appear in the same order, under *mixed-case* names (`Base`, `Num`, `Nav`, …). An all-caps name like `NUM` would be macro-expanded to its index, and the display name would become a number. `just draw`'s overview recipe in the `Justfile` refers to the layers by these names. `Base_W`/`Nav_W` are macOS→Windows overlays that swap GUI/CTRL on homerow mods and shortcuts, and `Nav_W` is a conditional layer (`BASE_WIN` + `NAV`). Macros and shortcut `#define`s assume macOS by default (e.g. `undo = LG(Z)`).

`config/common/` holds the shared pieces included by `base.keymap`: `combos.dtsi` (each combo lists the layers it is active on), `macros.dtsi`, `leader.dtsi` (leader sequences + unicode), `mouse.dtsi` (`ZMK_POINTING_DEFAULT_*` speeds, `&mmv`/`&msc` tuning), `extra_keys.h`.

Homerow mods (`hml`/`hmr`) use urob's "timeless" HRM settings (large tapping-term, `require-prior-idle-ms`, positional hold-tap on the opposite hand + thumbs). Read the README's HRM section before tuning them.

Board-level Kconfig goes in `config/<board>.conf`. The custom shields (neither ships with ZMK) live in `config/hardware/boards/shields/`. They're exposed as a Zephyr module through `config/zephyr/module.yml` (`board_root: hardware`). Locally, Zephyr scans every west project for modules, including the manifest project `config/`. The shields must stay inside `config/`, because CI (urob's `build-user-config` action) builds from an isolated copy of `config/` alone. The module file can't sit at the repo root anyway, because the root's `zephyr/` directory is the west-managed Zephyr checkout. Don't use `config/boards/` directly, which ZMK has deprecated.

## Upgrading ZMK

When bumping ZMK, check the [ZMK blog](https://zmk.dev/blog) for breaking changes and the keymap-upgrader rules in `zmk/docs/src/keymap-upgrade/` (deprecated keycodes, behaviors, node names, `label` properties). Then rebuild all targets.
