/* TODO:
  * OS switching - special layer accessed by pressing button 4 then button 5, then toggle with right click - Mac default
*/

#include QMK_KEYBOARD_H

// Mac utility function keycodes
#define KC_LS LCTL(KC_LEFT)  // Move leftward one space
#define KC_RS LCTL(KC_RGHT)  // Move rightward one space
#define KC_MS LCTL(KC_UP)  // Activate Mission Control
#define KC_DT KC_F11  // Show desktop
#define KC_SS LSFT(LCMD(KC_4))  // Take a screenshot
#define KC_MAC_COPY LCMD(KC_C)  // Copy
#define KC_MAC_CUT LCMD(KC_X)  // Cut
#define KC_MAC_PASTE LCMD(KC_V)  // Paste

// Layers
enum {
    _BASE,
    _DRAG_LOCK_CONTROL,
    _SCROLL_CONTROL,
    _DPI_CONTROL,
    _FUNCTIONS,
};

// Keycodes for drag lock feature
enum {
    DRAG_LOCK_ON = SAFE_RANGE,
    DRAG_LOCK_OFF,
};

// Tap Dance keycodes
enum {
    TD_BTN2,  // 1: KC_BTN2; 2: KC_ENT; 3: New Chrome window; Hold: _DPI_CONTROL
    TD_BTN4,  // 1: KC_BTN4; 2: KC_LS; Hold: _SCROLL_CONTROL + DRAG_SCROLL
    TD_BTN5,  // 1: KC_BTN5; 2: KC_RS; Hold: _FUNCTIONS
};

// Keymap
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(  // Base layer
        KC_BTN1, KC_MS, TD(TD_BTN2),
          TD(TD_BTN4), TD(TD_BTN5)
    ),
    [_DRAG_LOCK_CONTROL] = LAYOUT(  // Special layer for drag lock state
        DRAG_LOCK_OFF, _______, _______,
          _______, _______
    ),
    [_SCROLL_CONTROL] = LAYOUT(  // Drag scroll, horizontal scroll with wheel, and enable drag lock
        DRAG_LOCK_ON, KC_DT, KC_MAC_CUT,
          _______, XXXXXXX
    ),
    [_DPI_CONTROL] = LAYOUT(  // Cycle trackball DPI
        XXXXXXX, DPI_CONFIG, _______,
          XXXXXXX, XXXXXXX
    ),
    [_FUNCTIONS] = LAYOUT(  // Utility functions
        KC_BTN3, KC_SS, KC_MAC_COPY,
          KC_MAC_PASTE, _______
    )
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

// Track Tap Dance state separately for each Tap Dance key
static td_action_t btn2_td_action = TD_NONE;
static td_action_t btn4_td_action = TD_NONE;
static td_action_t btn5_td_action = TD_NONE;

// Fake keyrecord_t for hooking process_record_kb() with custom keycodes
static keyrecord_t fake_record;

// QMK userspace callback functions
bool process_record_user(uint16_t keycode, keyrecord_t *record);
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record);
bool encoder_update_user(uint8_t index, bool clockwise);

// Tap Dance callback functions
static td_action_t get_tap_dance_action(tap_dance_state_t *state);
static void btn2_td_tap(tap_dance_state_t *state, void *user_data);
static void btn2_td_finished(tap_dance_state_t *state, void *user_data) ;
static void btn2_td_reset(tap_dance_state_t *state, void *user_data);
static void btn4_td_tap(tap_dance_state_t *state, void *user_data);
static void btn4_td_finished(tap_dance_state_t *state, void *user_data);
static void btn4_td_reset(tap_dance_state_t *state, void *user_data);
static void btn5_td_tap(tap_dance_state_t *state, void *user_data);
static void btn5_td_finished(tap_dance_state_t *state, void *user_data);
static void btn5_td_reset(tap_dance_state_t *state, void *user_data);

// Associate tap dance keys with their functionality
tap_dance_action_t tap_dance_actions[] = {
    [TD_BTN2] = ACTION_TAP_DANCE_FN_ADVANCED(btn2_td_tap, btn2_td_finished, btn2_td_reset),
    [TD_BTN4] = ACTION_TAP_DANCE_FN_ADVANCED(btn4_td_tap, btn4_td_finished, btn4_td_reset),
    [TD_BTN5] = ACTION_TAP_DANCE_FN_ADVANCED(btn5_td_tap, btn5_td_finished, btn5_td_reset),
};

// Functions for sending custom keycodes, since QMK functions can't register them
static void setup_fake_record(uint8_t col, uint8_t row, bool pressed);
static void register_custom_keycode(uint16_t keycode, uint8_t col, uint8_t row);
static void unregister_custom_keycode(uint16_t keycode, uint8_t col, uint8_t row);



/* QMK userspace callback functions */

// Handle drag locking
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case DRAG_LOCK_ON:  // Enable drag lock
            if (record->event.pressed) {
                register_code16(KC_BTN1);
                layer_on(_DRAG_LOCK_CONTROL);
            }
            return false;
        case DRAG_LOCK_OFF:  // Disable drag lock
            if (!record->event.pressed) {
                layer_off(_DRAG_LOCK_CONTROL);
                unregister_code16(KC_BTN1);
            }
            return false;
        default:
            return true;
    }
}

// Use a longer TAPPING_TERM for slower ring and pinky fingers
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

// Horizonal scroll with wheel on _SCROLL_CONTROL layer
bool encoder_update_user(uint8_t index, bool clockwise) {
    if (IS_LAYER_ON(_SCROLL_CONTROL)) {
        report_mouse_t mouse_report = pointing_device_get_report();
        mouse_report.h = clockwise ? -1 : 1;
        pointing_device_set_report(mouse_report);
        pointing_device_send();
        return false;
    } else {
        return true;
    }
}



/* Tap Dance callback functions */

// Suports single/double/triple taps and single hold; favors instant hold when interrupted
static td_action_t get_tap_dance_action(tap_dance_state_t *state) {
    if (state->count == 1) return (state->pressed) ? TD_SINGLE_HOLD : TD_SINGLE_TAP;
    else if (state->count == 2) return TD_DOUBLE_TAP;
    else if (state->count == 3) return TD_TRIPLE_TAP;
    else return TD_UNKNOWN;
}

// Mouse button 2 Tap Dance on-each-tap callback
static void btn2_td_tap(tap_dance_state_t *state, void *user_data) {
    btn2_td_action = get_tap_dance_action(state);
    if (btn2_td_action == TD_TRIPLE_TAP) {
        SEND_STRING(SS_LCMD(" ") SS_DELAY(200) "chrome" SS_DELAY(200) SS_TAP(X_ENT) SS_DELAY(200) SS_LCMD("n"));
        state->finished = true;
    }
}

// Mouse button 2 Tap Dance on-dance-finished callback
static void btn2_td_finished(tap_dance_state_t *state, void *user_data) {
    btn2_td_action = get_tap_dance_action(state);
    switch (btn2_td_action) {
        case TD_SINGLE_TAP:
            tap_code16(KC_BTN2);
            break;
        case TD_DOUBLE_TAP:
            register_code(KC_ENT);
            break;
        case TD_SINGLE_HOLD:
            layer_on(_DPI_CONTROL);
            break;
        default:
            break;
    }
}

// Mouse button 2 Tap Dance on-dance-reset callback
static void btn2_td_reset(tap_dance_state_t *state, void *user_data) {
    switch (btn2_td_action) {
        case TD_DOUBLE_TAP:
            unregister_code(KC_ENT);
            break;
        case TD_SINGLE_HOLD:
            layer_off(_DPI_CONTROL);
            break;
        default:
            break;
    }
    btn2_td_action = TD_NONE;
}

// Mouse button 4 Tap Dance on-each-tap callback
static void btn4_td_tap(tap_dance_state_t *state, void *user_data) {
    btn4_td_action = get_tap_dance_action(state);
    if (btn4_td_action == TD_DOUBLE_TAP) {
        register_code16(KC_LS);
        state->finished = true;
    }
}

// Mouse button 4 Tap Dance on-dance-finished callback
static void btn4_td_finished(tap_dance_state_t *state, void *user_data) {
    btn4_td_action = get_tap_dance_action(state);
    switch (btn4_td_action) {
        case TD_SINGLE_TAP:
            tap_code16(KC_BTN4);
            break;
        case TD_SINGLE_HOLD:
            layer_on(_SCROLL_CONTROL);
            register_custom_keycode(DRAG_SCROLL, 3, 0);
            break;
        default:
            break;
    }
}

// Mouse button 4 Tap Dance on-dance-reset callback
static void btn4_td_reset(tap_dance_state_t *state, void *user_data) {
    switch (btn4_td_action) {
        case TD_DOUBLE_TAP:
            unregister_code16(KC_LS);
            break;
        case TD_SINGLE_HOLD:
            layer_off(_SCROLL_CONTROL);
            unregister_custom_keycode(DRAG_SCROLL, 3, 0);
            break;
        default:
            break;
    }
    btn4_td_action = TD_NONE;
}

// Mouse button 5 Tap Dance on-each-tap callback
static void btn5_td_tap(tap_dance_state_t *state, void *user_data) {
    btn5_td_action = get_tap_dance_action(state);
    if (btn5_td_action == TD_DOUBLE_TAP) {
        register_code16(KC_RS);
        state->finished = true;
    }
}

// Mouse button 5 Tap Dance on-dance-finished callback
static void btn5_td_finished(tap_dance_state_t *state, void *user_data) {
    btn5_td_action = get_tap_dance_action(state);
    switch (btn5_td_action) {
        case TD_SINGLE_TAP:
            tap_code16(KC_BTN5);
            break;
        case TD_SINGLE_HOLD:
            layer_on(_FUNCTIONS);
            break;
        default:
            break;
    }
}

// Mouse button 5 Tap Dance on-dance-reset callback
static void btn5_td_reset(tap_dance_state_t *state, void *user_data) {
    switch (btn5_td_action) {
        case TD_DOUBLE_TAP:
            unregister_code16(KC_RS);
            break;
        case TD_SINGLE_HOLD:
            layer_off(_FUNCTIONS);
            break;
        default:
            break;
    }
    btn5_td_action = TD_NONE;
}



/* Functions for sending custom keycodes */

// Setup fake_record for process_record_kb()
static void setup_fake_record(uint8_t col, uint8_t row, bool pressed) {
    fake_record.event.key.col = col;
    fake_record.event.key.row = row;
    fake_record.event.pressed = pressed;
    fake_record.event.time = timer_read();
}

// Register a custom keycode with process_record_kb()
static void register_custom_keycode(uint16_t keycode, uint8_t col, uint8_t row) {
    setup_fake_record(col, row, true);
    process_record_kb(keycode, &fake_record);
}

// Unregister a custom keycode with process_record_kb()
static void unregister_custom_keycode(uint16_t keycode, uint8_t col, uint8_t row) {
    setup_fake_record(col, row, false);
    process_record_kb(keycode, &fake_record);
}
