/* TODO
  * Figure out what to do about new Chrome window shortcut - get rid of it or implement Windows mode?
*/

#include QMK_KEYBOARD_H

// Mac utility function keycodes
#define MAC_DL     LCTL(KC_LEFT)        // Move leftward one space
#define MAC_DR     LCTL(KC_RGHT)        // Move rightward one space
#define MAC_SW     LCTL(KC_UP)          // Activate Mission Control
#define MAC_SD     KC_F11               // Show the desktop
#define MAC_SS     LSFT(LGUI(KC_4))     // Take a screenshot (selection)
#define MAC_COPY   LGUI(KC_C)           // Copy
#define MAC_CUT    LGUI(KC_X)           // Cut
#define MAC_PASTE  LGUI(KC_V)           // Paste

// Windows utility function keycodes
#define WIN_DL     LGUI(LCTL(KC_LEFT))  // Move leftward one desktop
#define WIN_DR     LGUI(LCTL(KC_RGHT))  // Move rightward one desktop
#define WIN_SW     LGUI(KC_TAB)         // Activate Task View
#define WIN_SD     LGUI(KC_D)           // Show the desktop
#define WIN_SS     LSFT(LGUI(KC_S))     // Take a screenshot (Snipping Tool)
#define WIN_COPY   LCTL(KC_C)           // Copy
#define WIN_CUT    LCTL(KC_X)           // Cut
#define WIN_PASTE  LCTL(KC_V)           // Paste

// Layers
enum {
    _BASE,
    _DRAG_LOCK,
    _DPI_CONTROL,
    _BTN4_FUNCTIONS,
    _BTN5_FUNCTIONS,
    _SYSTEM,
};

// Custom keycodes
enum {
    DRAG_LOCK_ON = SAFE_RANGE,
    DRAG_LOCK_OFF,
    TOGGLE_MAC_WINDOWS,
    SHOW_WINDOWS,
    SHOW_DESKTOP,
    SCREENSHOT,
    CUT,
    COPY,
    PASTE,
};

// Tap Dance keycodes
enum {
    TD_BTN2,  // 1: KC_BTN2; 2: KC_ENT; 3: New Chrome window; Hold: _DPI_CONTROL
    TD_BTN4,  // 1: KC_BTN4; 2: Desktop left; Hold: _BTN4_FUNCTIONS + DRAG_SCROLL + horizontal wheel scroll
    TD_BTN5,  // 1: KC_BTN5; 2: Desktop right; Hold: _BTN5_FUNCTIONS
};

// Keymap
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // Base layer
    [_BASE] = LAYOUT(
        KC_BTN1, SHOW_WINDOWS, TD(TD_BTN2),
        TD(TD_BTN4), TD(TD_BTN5)
    ),

    // Special layer for drag lock state
    [_DRAG_LOCK] = LAYOUT(
        DRAG_LOCK_OFF, _______, _______,
        _______, _______
    ),

    // Cycle trackball DPI
    [_DPI_CONTROL] = LAYOUT(
        XXXXXXX, DPI_CONFIG, _______,
        XXXXXXX, XXXXXXX
    ),

    // Drag scroll, horizontal scroll with wheel, drag lock and utility functions
    [_BTN4_FUNCTIONS] = LAYOUT(
        DRAG_LOCK_ON, SHOW_DESKTOP, CUT,
        _______, MO(_SYSTEM)
    ),

    // Utility functions
    [_BTN5_FUNCTIONS] = LAYOUT(
        KC_BTN3, SCREENSHOT, COPY,
        PASTE, _______
    ),

    // Toggle between Mac and Windows modes, reset and bootloader
    [_SYSTEM] = LAYOUT(
        QK_RBT, QK_BOOT, TOGGLE_MAC_WINDOWS,
        _______, _______
    ),
};

// Mac / Windows mode
static bool mac = true;

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

// Handle custom keycodes
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case DRAG_LOCK_ON:  // Enable drag lock
            if (record->event.pressed) {
                register_code16(KC_BTN1);
                layer_on(_DRAG_LOCK);
            }
            return false;
        case DRAG_LOCK_OFF:  // Disable drag lock
            if (!record->event.pressed) {
                layer_off(_DRAG_LOCK);
                unregister_code16(KC_BTN1);
            }
            return false;
        case TOGGLE_MAC_WINDOWS:  // Toggle between Mac and Windows modes
            if (record->event.pressed) {
                mac ^= 1;
            }
            return false;
        case SHOW_WINDOWS:  // Show all windows
            if (record->event.pressed) {
                register_code16(mac ? MAC_SW : WIN_SW);
            } else {
                unregister_code16(mac ? MAC_SW : WIN_SW);
            }
            return false;
        case SHOW_DESKTOP:  // Show the desktop
            if (record->event.pressed) {
                register_code16(mac ? MAC_SD : WIN_SD);
            } else {
                unregister_code16(mac ? MAC_SD : WIN_SD);
            }
            return false;
        case SCREENSHOT:  // Take a screenshot
            if (record->event.pressed) {
                register_code16(mac ? MAC_SS : WIN_SS);
            } else {
                unregister_code16(mac ? MAC_SS : WIN_SS);
            }
            return false;
        case CUT:
            if (record->event.pressed) {
                register_code16(mac ? MAC_CUT : WIN_CUT);
            } else {
                unregister_code16(mac ? MAC_CUT : WIN_CUT);
            }
            return false;
        case COPY:
            if (record->event.pressed) {
                register_code16(mac ? MAC_COPY : WIN_COPY);
            } else {
                unregister_code16(mac ? MAC_COPY : WIN_COPY);
            }
            return false;
        case PASTE:
            if (record->event.pressed) {
                register_code16(mac ? MAC_PASTE : WIN_PASTE);
            } else {
                unregister_code16(mac ? MAC_PASTE : WIN_PASTE);
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

// Horizonal scroll with wheel on _BTN4_FUNCTIONS layer
bool encoder_update_user(uint8_t index, bool clockwise) {
    if (IS_LAYER_ON(_BTN4_FUNCTIONS)) {
        tap_code(clockwise ? KC_WH_L : KC_WH_R);
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
        SEND_STRING(SS_LGUI(" ") SS_DELAY(200) "chrome" SS_DELAY(200) SS_TAP(X_ENT) SS_DELAY(200) SS_LGUI("n"));
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
        register_code16(mac ? MAC_DL : WIN_DL);
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
            layer_on(_BTN4_FUNCTIONS);
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
            unregister_code16(mac ? MAC_DL : WIN_DL);
            break;
        case TD_SINGLE_HOLD:
            layer_off(_BTN4_FUNCTIONS);
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
        register_code16(mac ? MAC_DR : WIN_DR);
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
            layer_on(_BTN5_FUNCTIONS);
            break;
        default:
            break;
    }
}

// Mouse button 5 Tap Dance on-dance-reset callback
static void btn5_td_reset(tap_dance_state_t *state, void *user_data) {
    switch (btn5_td_action) {
        case TD_DOUBLE_TAP:
            unregister_code16(mac ? MAC_DR : WIN_DR);
            break;
        case TD_SINGLE_HOLD:
            layer_off(_BTN5_FUNCTIONS);
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
