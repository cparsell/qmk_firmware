#if defined(RGB_MATRIX_FRAMEBUFFER_EFFECTS) && !defined(DISABLE_RGB_MATRIX_TYPING_HEATMAP)
RGB_MATRIX_EFFECT(TYPING_HEATMAP)
#    ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

#        ifndef RGB_MATRIX_TYPING_HEATMAP_DECREASE_DELAY_MS
#            define RGB_MATRIX_TYPING_HEATMAP_DECREASE_DELAY_MS 25
#        endif

void process_rgb_matrix_typing_heatmap(keyrecord_t* record) {

    uint8_t row   = record->event.key.row;
    uint8_t col   = record->event.key.col;
    uint8_t m_row = row-1;
    uint8_t p_row = row+1;
    uint8_t m_col = col-1;
    uint8_t p_col = col+1;


    if (m_col < col) g_rgb_frame_buffer[row][m_col] = qadd8(g_rgb_frame_buffer[row][m_col], 6);
    g_rgb_frame_buffer[row][col] = qadd8(g_rgb_frame_buffer[row][col], 25);
    if (p_col < MATRIX_COLS) g_rgb_frame_buffer[row][p_col] = qadd8(g_rgb_frame_buffer[row][p_col], 6);

    if (p_row < MATRIX_ROWS) {
        if (m_col < col) g_rgb_frame_buffer[p_row][m_col] = qadd8(g_rgb_frame_buffer[p_row][m_col], 4);
        g_rgb_frame_buffer[p_row][col] = qadd8(g_rgb_frame_buffer[p_row][col], 6);
        if (p_col < MATRIX_COLS) g_rgb_frame_buffer[p_row][p_col] = qadd8(g_rgb_frame_buffer[p_row][p_col], 4);
    }

    if (m_row < row) {
        if (m_col < col) g_rgb_frame_buffer[m_row][m_col] = qadd8(g_rgb_frame_buffer[m_row][m_col], 4);
        g_rgb_frame_buffer[m_row][col] = qadd8(g_rgb_frame_buffer[m_row][col], 6);
        if (p_col < MATRIX_COLS) g_rgb_frame_buffer[m_row][p_col] = qadd8(g_rgb_frame_buffer[m_row][p_col], 4);
    }
}

// A timer to track the last time we decremented all heatmap values.
static uint16_t heatmap_decrease_timer;
// Whether we should decrement the heatmap values during the next update.
static bool decrease_heatmap_values;


bool TYPING_HEATMAP(effect_params_t* params) {
    // Modified version of RGB_MATRIX_USE_LIMITS to work off of matrix row / col size
    uint8_t led_min = RGB_MATRIX_LED_PROCESS_LIMIT * params->iter;
    uint8_t led_max = led_min + RGB_MATRIX_LED_PROCESS_LIMIT;
      //RGB_MATRIX_USE_LIMITS(led_min, led_max);
    //CP - added fade in states
    //fade_in;
    //fade_out = ;
    //uint16_t fcount = 0;
    //RGB_MATRIX_USE_LIMITS(led_min, led_max);
    if (led_max > sizeof(g_rgb_frame_buffer)) led_max = sizeof(g_rgb_frame_buffer);

    if (params->init) {
        //HSV hsv2 = {140 - qsub8(36, 85), rgb_matrix_config.hsv.s - (36 - (36/3)), scale8((qadd8(170, 36) - 170) * 3, rgb_matrix_config.hsv.v )};
        //RGB rgb = rgb_matrix_hsv_to_rgb(hsv);
        //rgb_matrix_set_color_all(hsv2.h, hsv2.s, hsv2.h);
        rgb_matrix_set_color_all(0, scale8(12, rgb_matrix_config.hsv.v), scale8(10, rgb_matrix_config.hsv.v));

        //fcount = 0;
        //fade_in=true;
        //fade_out=false;
        memset(g_rgb_frame_buffer, 0, sizeof g_rgb_frame_buffer);
    }

    // The heatmap animation might run in several iterations depending on
    // `RGB_MATRIX_LED_PROCESS_LIMIT`, therefore we only want to update the
    // timer when the animation starts.
    if (params->iter == 0) {
        decrease_heatmap_values = timer_elapsed(heatmap_decrease_timer) >= RGB_MATRIX_TYPING_HEATMAP_DECREASE_DELAY_MS;

        // Restart the timer if we are going to decrease the heatmap this frame.
        if (decrease_heatmap_values) {
            heatmap_decrease_timer = timer_read();
        }
    }


    /*
    HSV      hsv2  = rgb_matrix_config.hsv;
    uint16_t timeb = scale16by8(g_rgb_timer, rgb_matrix_config.speed / 8);
    fcount  = qadd8(fcount, 1);
    if (fcount>500) {fade_in=false;}
    hsv2.v         = scale8(abs8(sin8(timeb) - 128) * 2, hsv2.v);
    RGB rgb2       = rgb_matrix_hsv_to_rgb(hsv2);
    */
    //return led_max < DRIVER_LED_TOTAL;
    //end fade math

    // Render heatmap & decrease
    for (int i = led_min; i < led_max; i++) {
        RGB_MATRIX_TEST_LED_FLAGS();
        uint8_t row = i % MATRIX_ROWS;
        uint8_t col = i / MATRIX_ROWS;
        uint8_t val = g_rgb_frame_buffer[row][col];
        //if (1 << USB_LED_CAPS_LOCK) {
        //  qadd8(g_rgb_frame_buffer[3][0], 25);
        //}
        // set the pixel colour

        uint8_t led[LED_HITS_TO_REMEMBER];

        uint8_t led_count = rgb_matrix_map_row_column_to_led(row, col, led);
        for (uint8_t j = 0; j < led_count; ++j) {
            if (!HAS_ANY_FLAGS(g_led_config.flags[led[j]], params->flags)) continue;
            //S value: rgb_matrix_config.hsv.s - (val - (val/2))
            //v value: scale8((qadd8(170, val) - 170) * 3, rgb_matrix_config.hsv.v )
            HSV hsv = {
              130 - qsub8(val, 85),
              255 - (val*1.5),
              scale8(  (qadd8(170, val) - 175) * (3 + (val/500)), rgb_matrix_config.hsv.v) };

            RGB rgb = rgb_matrix_hsv_to_rgb(hsv);
            //if (rgb.r < 8) rgb.r = scale8(1, rgb_matrix_config.hsv.v);
          //  if (rgb.g < 40) rgb.g = scale8(35, rgb_matrix_config.hsv.v);
            //if (rgb.b < 38) rgb.b = scale8(32, rgb_matrix_config.hsv.v);


            //if (fade_in) {
          //    RGB_MATRIX_TEST_LED_FLAGS();
          //    rgb_matrix_set_color(i, rgb2.r, rgb2.g, rgb2.b);
          //  } else {
          //RGB_MATRIX_TEST_LED_FLAGS();
            rgb_matrix_set_color(led[j], rgb.r, rgb.g, rgb.b);
          //  }
            //rgb_matrix_indicators_kb();


        }

        if (decrease_heatmap_values) {
            g_rgb_frame_buffer[row][col] = qsub8(val, 1);
            if (g_rgb_frame_buffer[row][col]<40) g_rgb_frame_buffer[row][col] = 40;
        }
    }

    return led_max < sizeof(g_rgb_frame_buffer);
}

#    endif  // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#endif      // defined(RGB_MATRIX_FRAMEBUFFER_EFFECTS) && !defined(DISABLE_RGB_MATRIX_TYPING_HEATMAP)
