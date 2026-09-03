/*
 * nice!view "gem" pet animation.
 *
 * Drives the frog art (see assets/frog.c) on the right / peripheral display:
 * a slow idle loop that randomly breaks into a short "eat" or "jump" action.
 * Frame cadence is kept low on purpose for a retro feel (and to limit display
 * refreshes / battery use).
 *
 * Pixel art: glooo89 -- "1-bit a frog set"
 *   https://glooo89.itch.io/1-bit-a-frog-set
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdlib.h>
#include <zephyr/kernel.h>

#include <lvgl.h>

#include "animation.h"

LV_IMG_DECLARE(frog_idle_1);
LV_IMG_DECLARE(frog_idle_2);
LV_IMG_DECLARE(frog_eat_1);
LV_IMG_DECLARE(frog_eat_2);
LV_IMG_DECLARE(frog_eat_3);
LV_IMG_DECLARE(frog_jump_1);
LV_IMG_DECLARE(frog_jump_2);
LV_IMG_DECLARE(frog_jump_3);

enum pet_state { PET_IDLE = 0, PET_EAT, PET_JUMP };

struct pet_anim {
    lv_obj_t *art;
    lv_timer_t *timer;
    enum pet_state state;
    uint8_t frame;
    uint8_t idle_loops;
};

static struct pet_anim anim;

/* Idle: a slow two-frame breathing loop. */
static const lv_img_dsc_t *idle_imgs[] = {&frog_idle_1, &frog_idle_2};
static const uint32_t idle_hold_ms[] = {850, 850};

/* Eat: reach forward, then gape. */
static const lv_img_dsc_t *eat_imgs[] = {&frog_eat_1, &frog_eat_2, &frog_eat_3};
static const uint32_t eat_hold_ms[] = {280, 340, 260};

/* Jump: spring up, hang at the top, then land. */
static const lv_img_dsc_t *jump_imgs[] = {&frog_jump_1, &frog_jump_2, &frog_jump_3};
static const uint32_t jump_hold_ms[] = {240, 360, 260};

static const lv_img_dsc_t **state_imgs(void) {
    switch (anim.state) {
    case PET_EAT:
        return eat_imgs;
    case PET_JUMP:
        return jump_imgs;
    case PET_IDLE:
    default:
        return idle_imgs;
    }
}

static uint8_t state_img_count(void) {
    switch (anim.state) {
    case PET_EAT:
    case PET_JUMP:
        return 3;
    case PET_IDLE:
    default:
        return 2;
    }
}

static uint32_t state_hold_ms(uint8_t frame) {
    switch (anim.state) {
    case PET_EAT:
        return eat_hold_ms[frame];
    case PET_JUMP:
        return jump_hold_ms[frame];
    case PET_IDLE:
    default:
        return idle_hold_ms[frame];
    }
}

static void set_frame(void) {
    lv_img_set_src(anim.art, state_imgs()[anim.frame]);
    lv_timer_set_period(anim.timer, state_hold_ms(anim.frame));
    lv_timer_reset(anim.timer);
}

static void pick_action(void) {
    /* Roughly half of the triggered actions are a jump, half an eat. */
    if ((rand() % 2) == 0) {
        anim.state = PET_EAT;
    } else {
        anim.state = PET_JUMP;
    }
    anim.frame = 0;
    set_frame();
}

static void pet_tick(lv_timer_t *timer) {
    ARG_UNUSED(timer);

    if (anim.state == PET_IDLE) {
        if (++anim.frame >= state_img_count()) {
            anim.frame = 0;
            anim.idle_loops++;

            /* Every ~3 idle loops, ~1/3 chance to break into an action. */
            if (anim.idle_loops >= 3 && (rand() % 3) == 0) {
                anim.idle_loops = 0;
                pick_action();
                return;
            }
        }
    } else {
        if (++anim.frame >= state_img_count()) {
            /* Action finished: back to the idle loop. */
            anim.state = PET_IDLE;
            anim.frame = 0;
            anim.idle_loops = 0;
        }
    }

    set_frame();
}

void draw_animation(lv_obj_t *canvas) {
    anim.art = lv_img_create(canvas);
    lv_img_set_src(anim.art, &frog_idle_1);

    /* Same footprint/placement the previous gem art used. */
    lv_obj_align(anim.art, LV_ALIGN_TOP_LEFT, 36, 0);

#if IS_ENABLED(CONFIG_NICE_VIEW_GEM_ANIMATION)
    srand(k_uptime_get_32());

    anim.state = PET_IDLE;
    anim.frame = 0;
    anim.idle_loops = 0;

    anim.timer = lv_timer_create(pet_tick, idle_hold_ms[0], NULL);
#endif
}
