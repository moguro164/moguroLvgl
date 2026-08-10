#include "home_screen.h"

#include <stdio.h>

static void fullpress_btn_event_cb(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);
  if(code == LV_EVENT_CLICKED || code == LV_EVENT_SHORT_CLICKED) {
    lv_obj_t *btn = lv_event_get_target(e);
    lv_obj_t *label = lv_obj_get_child(btn, 0);
    const char *text = label != NULL ? lv_label_get_text(label) : "button";
    printf("%s button clicked\n", text);
  }
}

static lv_obj_t *create_label(lv_obj_t *parent, const char *text, int width, int height, lv_align_t align, int offset_x, int offset_y, const lv_font_t *font, lv_color_t color)
{
  lv_obj_t *label = lv_label_create(parent);
  lv_label_set_text(label, text);
  lv_obj_set_size(label, width, height);
  lv_obj_align(label, align, offset_x, offset_y);
  lv_obj_set_style_text_font(label, font, 0);
  lv_obj_set_style_text_color(label, color, 0);
  lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
  return label;
}

static lv_obj_t *create_button(lv_obj_t *parent, const char *text, int width, int height, lv_align_t align, int offset_x, int offset_y)
{
  lv_obj_t *btn = lv_btn_create(parent);
  lv_obj_set_size(btn, width, height);
  lv_obj_align(btn, align, offset_x, offset_y);
  lv_obj_set_style_bg_color(btn, lv_color_hex(0x1F1F1F), 0);
  lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
  lv_obj_set_style_border_color(btn, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_border_width(btn, 2, 0);
  lv_obj_set_style_radius(btn, 12, 0);
  lv_obj_set_style_pad_all(btn, 6, 0);
  lv_obj_set_style_border_width(btn, 6, LV_PART_MAIN | LV_STATE_FOCUSED);
  lv_obj_set_style_border_color(btn, lv_color_hex(0xFFD400), LV_PART_MAIN | LV_STATE_FOCUSED);
  lv_obj_set_clickable(btn, true);

  lv_obj_t *label = lv_label_create(btn);
  lv_label_set_text(label, text);
  lv_obj_center(label);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_36, 0);
  lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);

  return btn;
}

lv_obj_t *create_home_screen(void)
{
  lv_obj_t *screen = lv_obj_create(NULL);
  lv_obj_t *title;
  lv_obj_t *status;
  lv_obj_t *btn1;
  lv_obj_t *btn2;
  lv_group_t *group;

  title = create_label(screen, "LVGL Button Demo", 240, 40, LV_ALIGN_TOP_MID, 0, 30, &lv_font_montserrat_48, lv_color_hex(0xA83232));
  (void)title;

  status = create_label(screen, "Press a button", 220, 30, LV_ALIGN_CENTER, 0, 40, &lv_font_montserrat_18, lv_color_hex(0xFFD400));
  (void)status;

  group = lv_group_get_default();
  if(group == NULL) {
    group = lv_group_create();
    lv_group_set_default(group);
  }

  btn1 = create_button(screen, "FullPress", 220, 80, LV_ALIGN_TOP_MID, 0, 90);
  lv_obj_add_event_cb(btn1, fullpress_btn_event_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(btn1, fullpress_btn_event_cb, LV_EVENT_SHORT_CLICKED, NULL);

  btn2 = create_button(screen, "Next", 220, 80, LV_ALIGN_BOTTOM_MID, 0, -30);
  lv_obj_add_event_cb(btn2, fullpress_btn_event_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(btn2, fullpress_btn_event_cb, LV_EVENT_SHORT_CLICKED, NULL);

  lv_group_add_obj(group, btn1);
  lv_group_add_obj(group, btn2);
  lv_group_focus_obj(btn1);

  return screen;
}

lv_obj_t *create_main_screen(void)
{
  return lv_obj_create(NULL);
}

lv_obj_t *create_settings_screen(void)
{
  return lv_obj_create(NULL);
}

lv_obj_t *create_detail_screen(void)
{
  return lv_obj_create(NULL);
}
