// safe range starts at `PLOOPY_SAFE_RANGE` instead.
#include QMK_KEYBOARD_H

// Tap Dance states
typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD,
    TD_DOUBLE_TAP
} td_action_t;

//Favors instant hold when interrupted.
td_action_t get_tap_dance_action(qk_tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->pressed) return TD_SINGLE_HOLD;
        else return TD_SINGLE_TAP;
    }
    else if (state->count == 2) return TD_DOUBLE_TAP;
    else return TD_UNKNOWN;
}



static td_action_t lt_btn2_ent_td_action = TD_NONE;

//Send right click on single tap, Enter on double tap, and switch to layer 1 when held
void lt_btn2_ent_finished(qk_tap_dance_state_t *state, void *user_data) {
    lt_btn2_ent_td_action = get_tap_dance_action(state);
    switch (lt_btn2_ent_td_action) {
        case TD_SINGLE_TAP:
            tap_code16(KC_BTN2);
            break;
        case TD_SINGLE_HOLD:
            layer_on(1);
            break;
        case TD_DOUBLE_TAP:
            tap_code(KC_ENT);
            break;
        default:
            break;
    }
}

//Turn off layer 1 if the button was held
void lt_btn2_ent_reset(qk_tap_dance_state_t *state, void *user_data) {
    // If the key was held down and now is released then switch off the layer
    if (lt_btn2_ent_td_action == TD_SINGLE_HOLD) {
        layer_off(1);
    }
    lt_btn2_ent_td_action = TD_NONE;
}




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

static td_action_t dragscroll_back_td_action = TD_NONE;

//Send back on single tap and enable drag scroll when held
void dragscroll_back_finished(qk_tap_dance_state_t *state, void *user_data) {
    dragscroll_back_td_action = get_tap_dance_action(state);
    switch (dragscroll_back_td_action) {
        case TD_SINGLE_TAP:
            tap_code16(KC_BTN4);
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

//Turn off drag scroll if the button was held
void dragscroll_back_reset(qk_tap_dance_state_t *state, void *user_data) {
    // If the key was held down and now is released then switch off the layer
    if (dragscroll_back_td_action == TD_SINGLE_HOLD) {
        btn4_record.event.pressed = false;
        btn4_record.event.time = timer_read();
        process_record_kb(DRAG_SCROLL, &btn4_record);
    }
    dragscroll_back_td_action = TD_NONE;
}




static td_action_t lt_fwd_td_action = TD_NONE;

//Send forward on single tap and switch to layer 2 when held
void lt_fwd_finished(qk_tap_dance_state_t *state, void *user_data) {
    lt_fwd_td_action = get_tap_dance_action(state);
    switch (lt_fwd_td_action) {
        case TD_SINGLE_TAP:
            tap_code16(KC_BTN5);
            break;
        case TD_SINGLE_HOLD:
            layer_on(2);
            break;
        default:
            break;
    }
}

//Turn off layer 2 if the button was held
void lt_fwd_reset(qk_tap_dance_state_t *state, void *user_data) {
    // If the key was held down and now is released then switch off the layer
    if (lt_fwd_td_action == TD_SINGLE_HOLD) {
        layer_off(2);
    }
    lt_fwd_td_action = TD_NONE;
}




// Tap Dance keycodes
enum {
    LT_BTN2_ENT,
    DRAGSCROLL_BACK,
    LT_FWD,
};

// Associate tap dance keys with their functionality
qk_tap_dance_action_t tap_dance_actions[] = {
    [LT_BTN2_ENT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, lt_btn2_ent_finished, lt_btn2_ent_reset),
    [DRAGSCROLL_BACK] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dragscroll_back_finished, dragscroll_back_reset),
    [LT_FWD] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, lt_fwd_finished, lt_fwd_reset),
};




#define KC_LS LCTL(KC_LEFT) /* Move leftward one space */
#define KC_RS LCTL(KC_RGHT) /* Move rightward one space */
#define KC_MS KC_F8 /* Activate Mission Control */
#define KC_SS LSFT(LCMD(KC_4)) /* Take a screenshot */
#define KC_TM LSFT(LCMD(KC_M)) /* Toggle mute in Microsoft Teams */

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT( /* Base */
        KC_BTN1, KC_MS, TD(LT_BTN2_ENT),
          TD(DRAGSCROLL_BACK), TD(LT_FWD)
    ),
    [1] = LAYOUT( /* Switch spaces */
        _______, RESET, _______,
          KC_LS, KC_RS
    ),
    [2] = LAYOUT( /* Utility functions */
        KC_BTN3, KC_SS, KC_TM,
          DPI_CONFIG, _______
    )
};

/* TODO:
  * Figure out why drag scroll gets stuck sometimes when using layers
  * Scroll wheel horizontal scroll when drag scroll enabled
  * Remove RESET if not needed
  * Adjust tapping term per key
*/