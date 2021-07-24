// safe range starts at `PLOOPY_SAFE_RANGE` instead.
#include QMK_KEYBOARD_H

#define KC_LS LCTL(KC_LEFT) /* Move leftward one space */
#define KC_RS LCTL(KC_RGHT) /* Move rightward one space */
#define KC_MS KC_F8 /* Activate Mission Control */
#define KC_SS LSFT(LCMD(KC_4)) /* Take a screenshot */
#define KC_TM LSFT(LCMD(KC_M)) /* Toggle mute in Microsoft Teams */

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT( /* Base */
        KC_BTN1, KC_MS, LT(1, KC_BTN2),
          KC_BTN4, LT(2, KC_BTN5)
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
  * Troubleshoot button two layer issue
  * Drag scroll on hold button 4
  * Scroll wheel horizontal scroll when drag scroll enabled
  * Enter on double click button 2
  * Remove RESET if not needed
*/