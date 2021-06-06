#include QMK_KEYBOARD_H

//Encoder Cmd-Tab support
bool is_cmd_tab_active = false;
uint16_t cmd_tab_timer = 0;

//Mac text editing shortcuts
#define KC_GL LGUI(KC_LEFT)
#define KC_GR LGUI(KC_RGHT)
#define KC_GU LGUI(KC_UP)
#define KC_GD LGUI(KC_DOWN)

//Shift lock
#define KC_LSCL LSFT_T(KC_CAPS)
#define KC_RSCL RSFT_T(KC_CAPS)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [0] = LAYOUT(
    //┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
       KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                               KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
       KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                               KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSLS,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
       LT(2,KC_ESC), KC_A, KC_S,  KC_D,    KC_F,    KC_G,                               KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
    //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
       KC_LSPO, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_SPC,           RGB_TOG, KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSPC,
    //└────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                      KC_LGUI, MO(1),   KC_SPC,                    KC_ENT,  KC_RCTL, KC_RALT
                                  // └────────┴────────┴────────┘                 └────────┴────────┴────────┘
    ),

    [1] = LAYOUT(
    //┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
       KC_PSCR, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                              KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_DEL,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
       XXXXXXX, XXXXXXX, KC_MPRV, KC_MPLY, KC_MNXT, KC_LBRC,                            KC_RBRC, KC_GL,   KC_UP,   KC_GR,   KC_GU,   KC_F11,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
       _______, XXXXXXX, KC_MUTE, KC_VOLD, KC_VOLU, KC_EQL,                             KC_MINS, KC_LEFT, KC_DOWN, KC_RGHT, KC_GD,   KC_F12,
    //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
       KC_LSCL, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_RSCL,
    //└────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                      _______, _______, XXXXXXX,                   XXXXXXX, _______, _______
                                  // └────────┴────────┴────────┘                 └────────┴────────┴────────┘
    ),

    [2] = LAYOUT(
    //┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
       KC_PSCR, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                              KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_DEL,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
       XXXXXXX, XXXXXXX, KC_MPRV, KC_MPLY, KC_MNXT, KC_LBRC,                            KC_RBRC, KC_HOME, KC_UP,   KC_END,  KC_PGUP, KC_F11,
    //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
       _______, XXXXXXX, KC_MUTE, KC_VOLD, KC_VOLU, KC_EQL,                             KC_MINS, KC_LEFT, KC_DOWN, KC_RGHT, KC_PGDN, KC_F12,
    //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
       KC_LSCL, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_RSCL,
    //└────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                      _______, _______, XXXXXXX,                   XXXXXXX, _______, _______
                                  // └────────┴────────┴────────┘                 └────────┴────────┴────────┘
    ),
};

bool encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 1) { /* Right encoder */
        if (IS_LAYER_ON(2)) { /* Switch browser tabs */
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