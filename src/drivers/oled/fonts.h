/*
 * fonts.h
 *
 *  Created on: 17/08/2022
 *      Author: Rohan
 */

#ifndef DRIVERS_ALL_CPUS_OLED_LOW_LEVEL_ALL_CPUS_FONTS_H_
#define DRIVERS_ALL_CPUS_OLED_LOW_LEVEL_ALL_CPUS_FONTS_H_

#include "r_typedefs.h"

 typedef struct
{
	uint16_t glyph_index;
	uint8_t w_px;
}lv_font_glyph_dsc_t;

extern const uint8_t font_20px[];
extern const lv_font_glyph_dsc_t font_20px_desc[];

extern const uint8_t font_9px[];
extern const lv_font_glyph_dsc_t font_9px_desc[];

extern const uint8_t font_8px[];
extern const lv_font_glyph_dsc_t font_8px_desc[];

extern const uint8_t font_apple[];
extern const lv_font_glyph_dsc_t font_apple_desc[];

extern const uint8_t font_red_alert[];
extern const lv_font_glyph_dsc_t font_red_alert_desc[];

extern const uint8_t font_helvetica_20px[];
extern const lv_font_glyph_dsc_t font_helvetica_20px_desc[];

extern const uint8_t font_metric_semibold_20px[];
extern const lv_font_glyph_dsc_t font_metric_semibold_20px_desc[];

extern const uint8_t font_metric_bold_13px[];
extern const lv_font_glyph_dsc_t font_metric_bold_13px_desc[];

extern const uint8_t font_metric_semibold_13px[];
extern const lv_font_glyph_dsc_t font_metric_semibold_13px_desc[];

extern const uint8_t font_metric_semibold_9px[];
extern const lv_font_glyph_dsc_t font_metric_semibold_9px_desc[];

#endif /* DRIVERS_ALL_CPUS_OLED_LOW_LEVEL_ALL_CPUS_FONTS_H_ */
