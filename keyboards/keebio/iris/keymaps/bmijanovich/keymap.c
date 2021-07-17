#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [0] = LAYOUT(
    //┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
       KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                               KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
       KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                               KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSLS,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
       LCTL_T(KC_ESC), KC_A, KC_S, KC_D,   KC_F,    KC_G,                               KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
    //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
       KC_LSPO, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    LT(1, KC_SPC), RSFT_T(KC_BSPC), KC_N, KC_M,  KC_COMM, KC_DOT,  KC_SLSH, KC_RSPC,
    //└────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                          LOPT_T(KC_ESC), LGUI_T(KC_TAB), LT(1, KC_SPC),      RSFT_T(KC_BSPC), RGUI_T(KC_ENT), RCTL_T(KC_DEL)
                                  // └────────┴────────┴────────┘                 └────────┴────────┴────────┘
    ),

    [1] = LAYOUT(
    //┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
       KC_PSCR, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                              KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_DEL,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
       XXXXXXX, XXXXXXX, KC_MPRV, KC_MPLY, KC_MNXT, KC_LBRC,                            KC_RBRC, KC_HOME, KC_UP,   KC_END,  KC_PGUP, KC_F11,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
       LCTL_T(KC_CAPS), XXXXXXX, KC_MUTE, KC_VOLD, KC_VOLU, KC_EQL,                     KC_MINS, KC_LEFT, KC_DOWN, KC_RGHT, KC_PGDN, KC_F12,
    //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
       _______, RGB_TOG, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______,          _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______,
    //└────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                      _______, _______, _______,                   _______, _______, _______
                                  // └────────┴────────┴────────┘                 └────────┴────────┴────────┘
    )
};

bool get_tapping_force_hold(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LT(1, KC_SPC):
            return true;
        default:
            return false;
    }
}

#ifdef RGBLIGHT_ENABLE
void keyboard_post_init_user(void) {
    rgblight_sethsv(HSV_WHITE);
    rgblight_mode(RGBLIGHT_MODE_STATIC_LIGHT);
}
#endif