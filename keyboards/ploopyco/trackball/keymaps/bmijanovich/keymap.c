#include QMK_KEYBOARD_H

// Layers
enum {
    _BASE,
    _DRAG_LOCK_CONTROL,
    _SCROLL_CONTROL,
    _DPI_CONTROL,
    _FUNCTIONS,
};

// Custom keycode for enabling rolling off layer and DPI_CONFIG button
enum {
    DPI_TOG = PLOOPY_SAFE_RANGE,
    DRAG_LOCK_ON,
    DRAG_LOCK_OFF,
};

// Mac utility function keycodes
#define KC_LS LCTL(KC_LEFT)  // Move leftward one space
#define KC_RS LCTL(KC_RGHT)  // Move rightward one space
#define KC_MS LCTL(KC_UP)  // Activate Mission Control
#define KC_DT KC_F11  // Show desktop
#define KC_SS LSFT(LCMD(KC_4))  // Take a screenshot

// Track drag scrolling state
static bool drag_scroll_active = false;




/* Drag Locking

As of version 0.13.26, QMK resets all mouse buttons on a layer change. We want to hold down KC_BTN1 to implement
a "drag lock" feature, but this requires some gymnastics to disable layer changes when in this state. We'll track
state here and refer to it before changing layers.
*/
// Keep track of drag lock state
static bool drag_lock_active = false;

// Check drag lock state before enabling layer
static void check_layer_on(uint8_t layer) {
    if (!drag_lock_active) {
        layer_on(layer);
    }
}

// Check if layer is on before disabling it
static void check_layer_off(uint8_t layer) {
    if (IS_LAYER_ON(layer)) {
        layer_off(layer);
    }
}

// Use lower-level API to hold left click
static void hold_left_click(bool hold) {
    report_mouse_t current_report = pointing_device_get_report();
    if (hold) {
        current_report.buttons |= 1;
    }
    else {
        current_report.buttons &= ~1;
    }
    pointing_device_set_report(current_report);
    pointing_device_send();
}
/* End Drag Locking */




/* Functions for sending custom keycodes

QMK functions can't register custom keycodes, but we can setup a keyrecord_t and call process_record_kb() directly.

Unknowns:
    * Do we need to set the column and row for each keycode?
    * Could reusing the same keyrecord_t struct cause problems with keycodes clobbering each other?
*/
// Dummy keyrecord_t for hooking process_record_kb() with custom keycodes
static keyrecord_t dummy_record = {
    .event = {
        .key = {
            .col = 0,
            .row = 0,
            },
        .pressed = false,
        .time = 0,
    },
    .tap = {0},
};

// Setup dummy_record for process_record_kb()
void setup_dummy_record(uint8_t col, uint8_t row, bool pressed) {
    dummy_record.event.key.col = col;
    dummy_record.event.key.row = row;
    dummy_record.event.pressed = pressed;
    dummy_record.event.time = timer_read();
}

// Register a custom keycode with process_record_kb()
void register_custom_keycode(uint16_t keycode, uint8_t col, uint8_t row) {
    setup_dummy_record(col, row, true);
    process_record_kb(keycode, &dummy_record);
}

// Unregister a custom keycode with process_record_kb()
void unregister_custom_keycode(uint16_t keycode, uint8_t col, uint8_t row) {
    setup_dummy_record(col, row, false);
    process_record_kb(keycode, &dummy_record);
}

// Tap a custom keycode with process_record_kb()
void tap_custom_keycode(uint16_t keycode, uint8_t col, uint8_t row) {
    register_custom_keycode(keycode, col, row);
    wait_ms(10);
    unregister_custom_keycode(keycode, col, row);
}
/* End functions for sending custom keycodes */




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

// Suports single/double/triple taps and single hold. Favors instant hold when interrupted.
td_action_t get_tap_dance_action(qk_tap_dance_state_t *state) {
    if (state->count == 1) return (state->pressed) ? TD_SINGLE_HOLD : TD_SINGLE_TAP;
    else if (state->count == 2) return TD_DOUBLE_TAP;
    else if (state->count == 3) return TD_TRIPLE_TAP;
    else return TD_UNKNOWN;
}

// Reset and allow immediate repeat of a Tap Dance
void hard_reset_tap_dance(qk_tap_dance_state_t *state) {
    state->pressed = false;  // reset_tap_dance() will short circuit without this and we need it to complete
    state->finished = true;  // Don't know if this is needed, but it can't hurt
    reset_tap_dance(state);    
}
/* End shared Tap Dance configuration */




/*
Mouse button 2 Tap Dance configuration
  * Single tap: Right click (BTN2)
  * Double tap: Enter (KC_ENT)
  * Triple tap: Open a new Chrome window
  * Single hold: Switch to _DPI_CONTROL layer
*/
static td_action_t btn2_td_action = TD_NONE;

void btn2_td_tap(qk_tap_dance_state_t *state, void *user_data) {
    btn2_td_action = get_tap_dance_action(state);
    if (btn2_td_action == TD_TRIPLE_TAP) {
        SEND_STRING(SS_LCMD(" ") SS_DELAY(200) "chrome" SS_DELAY(200) SS_TAP(X_ENT) SS_DELAY(200) SS_LCMD("n"));
        hard_reset_tap_dance(state);
    }
}

void btn2_td_finished(qk_tap_dance_state_t *state, void *user_data) {
    btn2_td_action = get_tap_dance_action(state);
    switch (btn2_td_action) {
        case TD_SINGLE_TAP:
            tap_code16(KC_BTN2);
            break;
        case TD_DOUBLE_TAP:
            tap_code(KC_ENT);
            break;
        case TD_SINGLE_HOLD:
            check_layer_on(_DPI_CONTROL);
            break;
        default:
            break;
    }
}

void btn2_td_reset(qk_tap_dance_state_t *state, void *user_data) {
    if (btn2_td_action == TD_SINGLE_HOLD) {
        check_layer_off(_DPI_CONTROL);
    }
    btn2_td_action = TD_NONE;
}
/* End mouse button 2 Tap Dance configuration */




/*
Mouse button 4 Tap Dance configuration
  * Single tap: Back (BTN4)
  * Double tap: Move leftward one space (KC_LS)
  * Single hold: Enable trackball scrolling (DRAG_SCROLL)
*/
static td_action_t btn4_td_action = TD_NONE;

void btn4_td_tap(qk_tap_dance_state_t *state, void *user_data) {
    btn4_td_action = get_tap_dance_action(state);
    if (btn4_td_action == TD_DOUBLE_TAP) {
        tap_code16(KC_LS);
        hard_reset_tap_dance(state);
    }
}

void btn4_td_finished(qk_tap_dance_state_t *state, void *user_data) {
    btn4_td_action = get_tap_dance_action(state);
    switch (btn4_td_action) {
        case TD_SINGLE_TAP:
            tap_code16(KC_BTN4);
            break;
        case TD_SINGLE_HOLD:
            check_layer_on(_SCROLL_CONTROL);
            register_custom_keycode(DRAG_SCROLL, 3, 0);
            drag_scroll_active = true;
            break;
        default:
            break;
    }
}

void btn4_td_reset(qk_tap_dance_state_t *state, void *user_data) {
    if (btn4_td_action == TD_SINGLE_HOLD) {
        check_layer_off(_SCROLL_CONTROL);
        unregister_custom_keycode(DRAG_SCROLL, 3, 0);
        drag_scroll_active = false;
    }
    btn4_td_action = TD_NONE;
}
/* End mouse button 4 Tap Dance configuration */




/*
Mouse button 5 Tap Dance configuration
  * Single tap: Forward (BTN5)
  * Double tap: Move rightward one space (KC_RS)
  * Single hold: Switch to _FUNCTIONS layer
*/
static td_action_t btn5_td_action = TD_NONE;

void btn5_td_tap(qk_tap_dance_state_t *state, void *user_data) {
    btn5_td_action = get_tap_dance_action(state);
    if (btn5_td_action == TD_DOUBLE_TAP) {
        tap_code16(KC_RS);
        hard_reset_tap_dance(state);
    }
}

void btn5_td_finished(qk_tap_dance_state_t *state, void *user_data) {
    btn5_td_action = get_tap_dance_action(state);
    switch (btn5_td_action) {
        case TD_SINGLE_TAP:
            tap_code16(KC_BTN5);
            break;
        case TD_SINGLE_HOLD:
            check_layer_on(_FUNCTIONS);
            break;
        default:
            break;
    }
}

void btn5_td_reset(qk_tap_dance_state_t *state, void *user_data) {
    if (btn5_td_action == TD_SINGLE_HOLD) {
        check_layer_off(_FUNCTIONS);
    }
    btn5_td_action = TD_NONE;
}
/* End mouse button 5 Tap Dance configuration */




// Associate tap dance keys with their functionality
qk_tap_dance_action_t tap_dance_actions[] = {
    [TD_BTN2] = ACTION_TAP_DANCE_FN_ADVANCED(btn2_td_tap, btn2_td_finished, btn2_td_reset),
    [TD_BTN4] = ACTION_TAP_DANCE_FN_ADVANCED(btn4_td_tap, btn4_td_finished, btn4_td_reset),
    [TD_BTN5] = ACTION_TAP_DANCE_FN_ADVANCED(btn5_td_tap, btn5_td_finished, btn5_td_reset),
};

// Keymap
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(  // Base layer
        KC_BTN1, KC_MS, TD(TD_BTN2),
          TD(TD_BTN4), TD(TD_BTN5)
    ),
    [_DRAG_LOCK_CONTROL] = LAYOUT(  // Special layer for drag lock state
        DRAG_LOCK_OFF, _______, KC_DT,
          _______, _______
    ),
    [_SCROLL_CONTROL] = LAYOUT(  // Drag scroll, horizontal wheel scroll, and enable drag lock
        DRAG_LOCK_ON, KC_DT, XXXXXXX,
          _______, XXXXXXX
    ),
    [_DPI_CONTROL] = LAYOUT(  // Cycle trackball DPI
        XXXXXXX, DPI_TOG, _______,
          XXXXXXX, XXXXXXX
    ),
    [_FUNCTIONS] = LAYOUT(  // Utility functions
        KC_BTN3, KC_SS, XXXXXXX,
          RESET, _______
    )
};

// Horizontal scrolling with wheel while DRAG_SCROLL enabled
void process_wheel_user(report_mouse_t* mouse_report, int16_t h, int16_t v) {
    if (drag_scroll_active) {
        mouse_report->h = -v;
        pointing_device_set_report(*mouse_report);
        pointing_device_send();
    }
    else {
        mouse_report->h = h;
        mouse_report->v = v;
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_BTN1:  // Update drag lock state
            if (record->event.pressed && layer_state == _BASE) {  // Sometimes this happens on other layers when quickly rolling through Tap Dance
                drag_lock_active = true;
            }
            else {
                drag_lock_active = false;
            }
            return true;
        case DRAG_LOCK_ON:  // Enable drag lock
            if (!record->event.pressed) {
                drag_lock_active = true;
                layer_clear();
                layer_on(_DRAG_LOCK_CONTROL);
                hold_left_click(true);
            }
            return false;
        case DRAG_LOCK_OFF:  // Disable drag lock
            if (!record->event.pressed) {
                hold_left_click(false);
                layer_off(_DRAG_LOCK_CONTROL);
                drag_lock_active = false;
            }
            return false;
        case DPI_TOG:  // Enable rolling off layer and DPI_CONFIG button
            if (!record->event.pressed) {
                tap_custom_keycode(DPI_CONFIG, 1, 0);
            }
            return false;
        default:
            return true;
    }
}

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
  * Figure out conflicting Tap Dance keys, and assign copy/cut/paste
  * Figure out conflicting Tap Dance layers and prevent getting stuck
  * OS switching
  * Remove RESET when keymap finalized
*/