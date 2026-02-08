// Copyright 2023 Danny Nguyen (@nooges)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

// --- Layer definitions ---
enum custom_layer {
    _MAIN,
    _FN1,
    _FN2
};

// --- LED index defines ---
// Left half key LEDs (from keyboard.json layout order)
#define LED_ESC   0   // [0,0]
#define LED_R    10   // [1,4]
#define LED_T     9   // [1,5]
#define LED_F    19   // [2,4]
#define LED_G    20   // [2,5]
#define LED_V    22   // [3,4]
#define LED_B    21   // [3,5]

// Right half key LEDs
#define LED_H    54   // [7,5]
#define LED_J    53   // [7,4]
#define LED_K    52   // [7,3]
#define LED_L    51   // [7,2]

// Total LED count
#define LED_TOTAL 68

// --- RGB color constants ---
#define CLR_DIM_YELLOW 40, 35, 0
#define CLR_BLUE       0, 0, 180
#define CLR_OFF        0, 0, 0

// --- Startup animation state ---
static bool     startup_active    = false;
static uint32_t startup_timer     = 0;
static uint8_t  startup_led_index = 0;
#define STARTUP_LED_DELAY 35  // ms per LED

// --- TG layer tracking ---
static bool layer1_toggled = false;
static bool layer2_toggled = false;

// --- Blink timing ---
#define BLINK_INTERVAL 500  // ms

// --- Keymaps ---
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  [_MAIN] = LAYOUT(
  //┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
     QK_GESC, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                               KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
  //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
     KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                               KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_DEL,
  //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
     KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                               KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
  //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
     KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_HOME,          KC_END,  KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
  //└────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                    KC_LGUI, TL_LOWR, KC_ENT,                    KC_SPC,  TL_UPPR, KC_RALT
                                // └────────┴────────┴────────┘                 └────────┴────────┴────────┘
  ),

  [_FN1] = LAYOUT(
  //┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
     KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,                            KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_PGUP,
  //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
     KC_GRV,  _______, KC_UP,   _______, QK_BOOT, _______,                            _______, KC_P7,   KC_P8,   KC_P9,   KC_P0,   KC_PGDN,
  //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
     KC_DEL,  KC_LEFT, KC_DOWN, KC_RGHT, _______, KC_LBRC,                            KC_RBRC, KC_P4,   KC_P5,   KC_P6,   KC_PLUS, KC_PIPE,
  //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
     RGB_MOD, EE_CLR,  _______, _______, _______, KC_LCBR, KC_LPRN,          KC_RPRN, KC_RCBR, KC_P1,   KC_P2,   KC_P3,   KC_MINS, _______,
  //└────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                    _______, _______, KC_DEL,                    KC_DEL,  _______, KC_P0
                                // └────────┴────────┴────────┘                 └────────┴────────┴────────┘
  ),

  [_FN2] = LAYOUT(
  //┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
     KC_F12,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                              KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,
  //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
     RGB_TOG, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,                            KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, QK_BOOT,
  //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
     RGB_MOD, KC_MPRV, KC_MNXT, KC_VOLU, KC_PGUP, KC_UNDS,                            KC_EQL,  KC_HOME, RGB_HUI, RGB_SAI, RGB_VAI, KC_BSLS,
  //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
     KC_MUTE, KC_MSTP, KC_MPLY, KC_VOLD, KC_PGDN, KC_MINS, KC_LPRN,          _______, KC_PLUS, KC_END,  RGB_HUD, RGB_SAD, RGB_VAD, EE_CLR,
  //└────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                    _______, _______, _______,                   _______, _______, _______
                                // └────────┴────────┴────────┘                 └────────┴────────┴────────┘
  )
};

// --- Startup animation init ---
void keyboard_post_init_user(void) {
    startup_active    = true;
    startup_timer     = timer_read32();
    startup_led_index = 0;
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
    rgb_matrix_sethsv_noeeprom(0, 0, 0);
}

// --- TG keycode tracking ---
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        if (keycode == TG(_FN1)) {
            layer1_toggled = !layer1_toggled;
        } else if (keycode == TG(_FN2)) {
            layer2_toggled = !layer2_toggled;
        }
    }
    return true;
}

// --- Per-layer RGB indicator logic ---
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    // --- Startup scan animation ---
    if (startup_active) {
        // Turn off all LEDs in this range
        for (uint8_t i = led_min; i < led_max; i++) {
            rgb_matrix_set_color(i, CLR_OFF);
        }

        // Light only the current LED with a pseudo-random color
        if (startup_led_index >= led_min && startup_led_index < led_max) {
            uint8_t seed = (uint8_t)(startup_timer & 0xFF);
            uint8_t hue  = (startup_led_index * 37 + seed) % 256;
            HSV hsv      = {hue, 255, 200};
            RGB rgb      = hsv_to_rgb(hsv);
            rgb_matrix_set_color(startup_led_index, rgb.r, rgb.g, rgb.b);
        }

        // Advance to next LED on timer
        if (timer_elapsed32(startup_timer) > STARTUP_LED_DELAY) {
            startup_timer = timer_read32();
            startup_led_index++;
            if (startup_led_index >= LED_TOTAL) {
                startup_active = false;
                rgb_matrix_mode_noeeprom(RGB_MATRIX_CYCLE_LEFT_RIGHT);
                rgb_matrix_sethsv_noeeprom(0, 255, 120);
            }
        }
        return false;
    }

    // --- Layer indicator logic ---
    uint8_t highest_layer = get_highest_layer(layer_state);

    if (highest_layer == _FN1 || highest_layer == _FN2) {
        // Turn off all LEDs in this range
        for (uint8_t i = led_min; i < led_max; i++) {
            rgb_matrix_set_color(i, CLR_OFF);
        }

        // ESC indicator: solid blue (MO) or blinking blue (TG)
        bool is_toggled = (highest_layer == _FN1) ? layer1_toggled : layer2_toggled;
        bool show_esc;
        if (is_toggled) {
            // Blink: on for 500ms, off for 500ms
            show_esc = (timer_read() % (BLINK_INTERVAL * 2)) < BLINK_INTERVAL;
        } else {
            show_esc = true;  // Solid
        }

        if (show_esc && LED_ESC >= led_min && LED_ESC < led_max) {
            rgb_matrix_set_color(LED_ESC, CLR_BLUE);
        }

        // Layer-specific highlighted keys
        if (highest_layer == _FN1) {
            // Symbol layer: R, T, F, G, V, B
            if (LED_R >= led_min && LED_R < led_max) rgb_matrix_set_color(LED_R, CLR_DIM_YELLOW);
            if (LED_T >= led_min && LED_T < led_max) rgb_matrix_set_color(LED_T, CLR_DIM_YELLOW);
            if (LED_F >= led_min && LED_F < led_max) rgb_matrix_set_color(LED_F, CLR_DIM_YELLOW);
            if (LED_G >= led_min && LED_G < led_max) rgb_matrix_set_color(LED_G, CLR_DIM_YELLOW);
            if (LED_V >= led_min && LED_V < led_max) rgb_matrix_set_color(LED_V, CLR_DIM_YELLOW);
            if (LED_B >= led_min && LED_B < led_max) rgb_matrix_set_color(LED_B, CLR_DIM_YELLOW);
        } else {
            // Navigation layer: H, J, K, L
            if (LED_H >= led_min && LED_H < led_max) rgb_matrix_set_color(LED_H, CLR_DIM_YELLOW);
            if (LED_J >= led_min && LED_J < led_max) rgb_matrix_set_color(LED_J, CLR_DIM_YELLOW);
            if (LED_K >= led_min && LED_K < led_max) rgb_matrix_set_color(LED_K, CLR_DIM_YELLOW);
            if (LED_L >= led_min && LED_L < led_max) rgb_matrix_set_color(LED_L, CLR_DIM_YELLOW);
        }

        return false;
    }

    // Layer 0: no overrides, default animation plays
    return false;
}
