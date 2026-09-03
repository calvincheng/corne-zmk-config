# nice_view_gem (vendored + customized)

This shield was vendored from `M165437/nice-view-gem` (`v0.3.0`, upstream
`zmkfirmware/zmk` nice_view) into this user config so the art on the right /
peripheral display can be customized.

## What changed vs. upstream

- `assets/crystal.c` (the gem/crystal animation) is replaced by `assets/frog.c`.
- `widgets/animation.c` now runs a small pet animation: a slow two-frame idle
  loop that randomly plays a short "eat" or "jump" action. Frame pacing is low
  on purpose (retro feel, fewer display refreshes).
- The frog's ground frames sit on the bottom edge of the display (like a pet
  resting on the "floor"); the jump frames rise above that baseline.

## Art credits

Pixel art: **glooo89 — "1-bit a frog set"**
https://glooo89.itch.io/1-bit-a-frog-set

The frames are the set's canonical 16x16 sprites: the `example_frog_idle.gif`,
`example_frog_eat.gif` and `example_frog_jump.gif` files are 160x160 renders
where each sprite pixel is a 10x10 block. They are extracted at true pixel size
and upscaled ×4 nearest-neighbor, rendered as the white frog on the display.
See the header comment in `assets/frog.c` for the color mapping and layout.

## Tuning

- Enable/disable the animation and get a static frog: `CONFIG_NICE_VIEW_GEM_ANIMATION`
  in `config/corne.conf`.
- Idle/action pacing and how often actions happen are constants in
  `widgets/animation.c` (`idle_hold_ms`, `eat_hold_ms`, `jump_hold_ms`,
  `pet_tick()`).
- To swap in different art, replace `assets/frog.c` and the `LV_IMG_DECLARE` /
  frame arrays in `widgets/animation.c`.

## Upstream

The upstream nice-view shield that this is based on:
https://github.com/zmkfirmware/zmk/tree/v0.3.0/app/boards/shields/nice_view
