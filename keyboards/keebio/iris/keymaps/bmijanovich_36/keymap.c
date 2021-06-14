#include QMK_KEYBOARD_H

//Modifier state for remapping Tab and Grave when switching apps/windows
uint8_t mod_state;

//Encoder Cmd-Tab support
bool is_cmd_tab_active = false;
uint16_t cmd_tab_timer = 0;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [0] = LAYOUT(
    //┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                            XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
       XXXXXXX, KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                               KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    XXXXXXX,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
       XXXXXXX, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                               KC_H,    KC_J,    KC_K,    KC_L,    KC_QUOT, XXXXXXX,
    //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
       XXXXXXX, LSFT_T(KC_Z), KC_X, KC_C,  KC_V,    KC_B,    LT(1, KC_SPC),    RGB_TOG, KC_N,    KC_M,    KC_COMM, KC_DOT,  RSFT_T(KC_SLSH), XXXXXXX,
    //└────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                               LOPT_T(KC_ESC), LGUI_T(KC_TAB), LT(1, KC_SPC), LT(2, KC_BSPC), RGUI_T(KC_ENT), RCTL_T(KC_DEL)
                                  // └────────┴────────┴────────┘                 └────────┴────────┴────────┘
    ),

    [1] = LAYOUT(
    //┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                            XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
       XXXXXXX, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                               KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    XXXXXXX,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
       XXXXXXX, MO(3),   KC_MPRV, KC_MPLY, KC_MNXT, KC_EQL,                             KC_MINS, KC_LEFT, KC_UP,   KC_DOWN, KC_RGHT, XXXXXXX,
    //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
       XXXXXXX, KC_LSPO, KC_MUTE, KC_VOLD, KC_VOLU, KC_GRV,  _______,          _______, KC_BSLS, KC_SCLN, KC_LBRC, KC_RBRC, KC_RSPC, XXXXXXX,
    //└────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                      _______, _______, _______,                   _______, _______, _______
                                  // └────────┴────────┴────────┘                 └────────┴────────┴────────┘
    ),

    [2] = LAYOUT(
    //┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                            XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
       XXXXXXX, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,                            KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, XXXXXXX,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
       XXXXXXX, MO(3),   KC_MPRV, KC_MPLY, KC_MNXT, KC_PLUS,                            KC_UNDS, KC_LEFT, KC_UP,   KC_DOWN, KC_RGHT, XXXXXXX,
    //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
       XXXXXXX, KC_LSPO, KC_MUTE, KC_VOLD, KC_VOLU, KC_TILD, _______,          _______, KC_PIPE, KC_COLN, KC_LCBR, KC_RCBR, KC_RSPC, XXXXXXX,
    //└────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                      _______, _______, _______,                   _______, _______, _______
                                  // └────────┴────────┴────────┘                 └────────┴────────┴────────┘
    ),

    [3] = LAYOUT(
    //┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                            XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
       XXXXXXX, _______, _______, _______, _______, _______,                            _______, _______, _______, _______, _______, XXXXXXX,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
       XXXXXXX, _______, _______, _______, _______, _______,                            _______, _______, _______, _______, _______, XXXXXXX,
    //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
       XXXXXXX, _______, _______, _______, _______, _______, _______,          _______, _______, _______, _______, _______, _______, XXXXXXX,
    //└────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                      _______, _______, _______,                   _______, _______, _______
                                  // └────────┴────────┴────────┘                 └────────┴────────┴────────┘
    ),

    [4] = LAYOUT(
    //┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
       XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                            XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
       XXXXXXX, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                              KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  XXXXXXX,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
       XXXXXXX, KC_F11,  KC_F12,  XXXXXXX, XXXXXXX, KC_CAPS,                            XXXXXXX, KC_HOME, KC_PGUP, KC_PGDN, KC_END,  XXXXXXX,
    //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
       XXXXXXX, KC_LSFT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______,          _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_RSFT, XXXXXXX,
    //└────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                      _______, _______, _______,                   _______, _______, _______
                                  // └────────┴────────┴────────┘                 └────────┴────────┴────────┘
    )
};

layer_state_t layer_state_set_user(layer_state_t state) {
  return update_tri_layer_state(state, 1, 2, 4);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
    case KC_E:
        mod_state = get_mods();
        //Check for Cmd (switch apps) or Ctrl (switch browswer tabs)
        if (((mod_state & MOD_BIT(KC_LCMD)) == MOD_BIT(KC_LCMD)) || ((mod_state & MOD_BIT(KC_RCTL)) == MOD_BIT(KC_RCTL))) {
            if (record->event.pressed) {
                register_code(KC_TAB);
            } else {
                unregister_code(KC_TAB);
            }
            return false;
        }
        break;
    case KC_G:
        mod_state = get_mods();
        //Check for Cmd (switch windows)
        if ((mod_state & MOD_BIT(KC_LCMD)) == MOD_BIT(KC_LCMD)) {
            if (record->event.pressed) {
                register_code(KC_GRV);
            } else {
                unregister_code(KC_GRV);
            }
            return false;
        }
        break;
    }
    return true;
};

bool get_tapping_force_hold(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LT(1, KC_SPC):
            return true;
        default:
            return false;
    }
}

bool encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 1) { /* Right encoder */
        if (IS_LAYER_ON(3)) { /* Switch browser tabs */
            if (clockwise) {
                tap_code16(LCTL(KC_TAB));
            } else {
                tap_code16(LSFT(LCTL(KC_TAB)));
            }
        } else if (IS_LAYER_ON(1)) { /* Switch Mac application windows */
            if (clockwise) {
                tap_code16(LGUI(KC_GRV));
            } else {
                tap_code16(LSFT(LGUI(KC_GRV)));
            }
        } else { /* Switch Mac applications */
            if (!is_cmd_tab_active) {
                is_cmd_tab_active = true;
                register_code(KC_LGUI);
            }
            if (clockwise) {
                cmd_tab_timer = timer_read();
                tap_code16(KC_TAB);
            } else {
                cmd_tab_timer = timer_read();
                tap_code16(LSFT(KC_TAB));
            }
        }
    }
    return true;
}

void matrix_scan_user(void) {
  if (is_cmd_tab_active) { /* Release Cmd for Cmd-Tab after 750ms */
    if (timer_elapsed(cmd_tab_timer) > 750) {
      unregister_code(KC_LGUI);
      is_cmd_tab_active = false;
    }
  }
}

#ifdef RGBLIGHT_ENABLE
void keyboard_post_init_user(void) {
    rgblight_sethsv(HSV_WHITE);
    rgblight_mode(RGBLIGHT_MODE_STATIC_LIGHT);
}
#endif