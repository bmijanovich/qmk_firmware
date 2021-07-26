#include QMK_KEYBOARD_H

// Utility function keycodes
#define KC_LS LCTL(KC_LEFT) // Move leftward one space
#define KC_RS LCTL(KC_RGHT) // Move rightward one space
#define KC_MS KC_F8 // Activate Mission Control
#define KC_SS LSFT(LCMD(KC_4)) // Take a screenshot




/* Shared configuration for Tap Dance */
// Tap Dance keycodes
enum {
    TD_BTN2,
    TD_BTN4,
    TD_BTN5,
};

// Tap Dance actions
typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_DOUBLE_TAP,
    TD_TRIPLE_TAP,
    TD_SINGLE_HOLD,
} td_action_t;

//Suports single/double/triple taps and single hold. Favors instant hold when interrupted.
td_action_t get_tap_dance_action(qk_tap_dance_state_t *state) {
    if (state->count == 1) return (state->pressed) ? TD_SINGLE_HOLD : TD_SINGLE_TAP;
    else if (state->count == 2) return TD_DOUBLE_TAP;
    else if (state->count == 3) return TD_TRIPLE_TAP;
    else return TD_UNKNOWN;
}
/* End shared Tap Dance configuration */




/*
Mouse button 2 Tap Dance configuration
  * Single tap: Right click (BTN2)
  * Double tap: Enter (KC_ENT)
  * Triple tap: Backspace (KC_BSPC)
*/
static td_action_t btn2_td_action = TD_NONE;

void btn2_td_finished(qk_tap_dance_state_t *state, void *user_data) {
    btn2_td_action = get_tap_dance_action(state);
    switch (btn2_td_action) {
        case TD_SINGLE_TAP:
            tap_code16(KC_BTN2);
            break;
        case TD_DOUBLE_TAP:
            tap_code(KC_ENT);
            break;
        case TD_TRIPLE_TAP:
            tap_code(KC_BSPC);
            break;
        default:
            break;
    reset_tap_dance(state);
    }
}
/* End mouse button 2 Tap Dance configuration */




/*
Mouse button 4 Tap Dance configuration
  * Single tap: Back (BTN4)
  * Double tap: Move leftward one space (KC_LS)
  * Triple tap: Cycle trackball DPI (DPI_CONFIG)
  * Single hold: Enable trackball scrolling (DRAG_SCROLL)
*/
static keyrecord_t btn4_record = {
    .event = {
        .key = {
            .col = 3,
            .row = 0
            },
        .pressed = false,
        .time = 0
    },
    .tap = {0}
};

static td_action_t btn4_td_action = TD_NONE;

void btn4_td_finished(qk_tap_dance_state_t *state, void *user_data) {
    btn4_td_action = get_tap_dance_action(state);
    switch (btn4_td_action) {
        case TD_SINGLE_TAP:
            tap_code16(KC_BTN4);
            break;
        case TD_DOUBLE_TAP:
            tap_code16(KC_LS);
            break;
        case TD_TRIPLE_TAP:
            btn4_record.event.pressed = true;
            btn4_record.event.time = timer_read();
            process_record_kb(DPI_CONFIG, &btn4_record);
            wait_ms(10);
            btn4_record.event.pressed = false;
            btn4_record.event.time = timer_read();
            process_record_kb(DPI_CONFIG, &btn4_record);
            break;
        case TD_SINGLE_HOLD:
            btn4_record.event.pressed = true;
            btn4_record.event.time = timer_read();
            process_record_kb(DRAG_SCROLL, &btn4_record);
            break;
        default:
            break;
    }
}

void btn4_td_reset(qk_tap_dance_state_t *state, void *user_data) {
    if (btn4_td_action == TD_SINGLE_HOLD) {
        btn4_record.event.pressed = false;
        btn4_record.event.time = timer_read();
        process_record_kb(DRAG_SCROLL, &btn4_record);
    }
    btn4_td_action = TD_NONE;
}
/* End mouse button 4 Tap Dance configuration */




/*
Mouse button 5 Tap Dance configuration
  * Single tap: Forward (BTN5)
  * Double tap: Move rightward one space (KC_RS)
  * Single hold: Switch to layer 1
*/
static td_action_t btn5_td_action = TD_NONE;

void btn5_td_finished(qk_tap_dance_state_t *state, void *user_data) {
    btn5_td_action = get_tap_dance_action(state);
    switch (btn5_td_action) {
        case TD_SINGLE_TAP:
            tap_code16(KC_BTN5);
            break;
        case TD_DOUBLE_TAP:
            tap_code16(KC_RS);
            break;
        case TD_SINGLE_HOLD:
            layer_on(1);
            break;
        default:
            break;
    }
}

void btn5_td_reset(qk_tap_dance_state_t *state, void *user_data) {
    if (btn5_td_action == TD_SINGLE_HOLD) {
        layer_off(1);
    }
    btn5_td_action = TD_NONE;
}
/* End mouse button 5 Tap Dance configuration */




// Associate tap dance keys with their functionality
qk_tap_dance_action_t tap_dance_actions[] = {
    [TD_BTN2] = ACTION_TAP_DANCE_FN(btn2_td_finished),
    [TD_BTN4] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, btn4_td_finished, btn4_td_reset),
    [TD_BTN5] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, btn5_td_finished, btn5_td_reset),
};

// Keymap
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT( // Base layer
        KC_BTN1, KC_MS, TD(TD_BTN2),
          TD(TD_BTN4), TD(TD_BTN5)
    ),
    [1] = LAYOUT( // Utility functions
        KC_BTN3, KC_SS, _______,
          RESET, _______
    )
};

// Need a longer TAPPING_TERM for slower ring and pinky fingers
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case TD(TD_BTN4):
            return TAPPING_TERM + 50;
        case TD(TD_BTN5):
            return TAPPING_TERM + 50;
        default:
            return TAPPING_TERM;
    }
}


/* TODO:
  * Scroll wheel horizontal scroll when drag scroll enabled
  * Remove RESET when keymap finalized
*/