#include "home_screen.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char label[64];
  char font[64];
  char color[64];
  char alignment[64];
  lv_align_t align;
  int width;
  int height;
  int offset_x;
  int offset_y;
  char bg_color[64];
  char border_color[64];
  int border_width;
  int radius;
  int padding;
} style_t;

typedef struct {
  style_t style;
  style_t focused_style;
  bool has_focused_state;
  bool has_focus_key_state;
  bool focusable;
  bool initial_focus;
  char text[64];
} button_t;

typedef struct {
  style_t title;
  style_t status;
  button_t full_press;
  button_t next;
} screen_cfg_t;

static void fullpress_btn_event_cb(lv_event_t * e)
{
  lv_event_code_t code = lv_event_get_code(e);
  if(code == LV_EVENT_CLICKED) {
    lv_obj_t *btn = lv_event_get_target(e);
    lv_obj_t *label = lv_obj_get_child(btn, 0);
    const char *text = label != NULL ? lv_label_get_text(label) : NULL;
    printf("%s button clicked\n", text != NULL ? text : "button");
  }
}

static const char *find_matching_brace(const char *open_brace)
{
  int depth = 0;
  const char *p = open_brace;
  while(*p != '\0') {
    if(*p == '{') {
      depth++;
    }
    else if(*p == '}') {
      depth--;
      if(depth == 0) {
        return p;
      }
    }
    p++;
  }
  return NULL;
}

static bool find_object(const char *start, const char *end, const char *name, const char **obj_start, const char **obj_end)
{
  char pattern[64];
  const char *key_pos;
  const char *open_brace;

  snprintf(pattern, sizeof(pattern), "\"%s\"", name);
  key_pos = strstr(start, pattern);
  if(key_pos == NULL || key_pos > end) {
    return false;
  }

  open_brace = strchr(key_pos, '{');
  if(open_brace == NULL || open_brace > end) {
    return false;
  }

  *obj_start = open_brace;
  *obj_end = find_matching_brace(open_brace);
  return *obj_end != NULL;
}

static bool extract_string(const char *start, const char *end, const char *key, char *out, size_t out_size)
{
  char pattern[64];
  const char *key_pos;
  const char *value_pos;
  const char *quote_end;
  size_t len;

  snprintf(pattern, sizeof(pattern), "\"%s\"", key);
  key_pos = strstr(start, pattern);
  if(key_pos == NULL || key_pos > end) {
    return false;
  }

  value_pos = strchr(key_pos, ':');
  if(value_pos == NULL || value_pos > end) {
    return false;
  }
  value_pos++;
  while(value_pos < end && isspace((unsigned char)*value_pos)) {
    value_pos++;
  }
  if(value_pos >= end || *value_pos != '"') {
    return false;
  }

  quote_end = strchr(value_pos + 1, '"');
  if(quote_end == NULL || quote_end > end) {
    return false;
  }

  len = (size_t)(quote_end - (value_pos + 1));
  if(len >= out_size) {
    len = out_size - 1;
  }
  memcpy(out, value_pos + 1, len);
  out[len] = '\0';
  return true;
}

static bool extract_int(const char *start, const char *end, const char *key, int *out)
{
  char value[64];
  char *end_ptr;
  long parsed;

  if(!extract_string(start, end, key, value, sizeof(value))) {
    return false;
  }

  parsed = strtol(value, &end_ptr, 10);
  if(end_ptr == value || *end_ptr != '\0') {
    return false;
  }
  *out = (int)parsed;
  return true;
}

static bool extract_bool(const char *start, const char *end, const char *key, bool *out)
{
  char value[16];
  if(!extract_string(start, end, key, value, sizeof(value))) {
    return false;
  }
  if(strcmp(value, "true") == 0) {
    *out = true;
    return true;
  }
  if(strcmp(value, "false") == 0) {
    *out = false;
    return true;
  }
  return false;
}

static bool extract_focus_flags(const char *appearance_start, const char *appearance_end, bool *focused, bool *focus_key)
{
  char buffer[256];
  const char *array_start;
  const char *array_end;
  const char *cursor;
  bool found_focused = false;
  bool found_focus_key = false;

  if(!find_object(appearance_start, appearance_end, "focusStates", &array_start, &array_end)) {
    return false;
  }

  snprintf(buffer, sizeof(buffer), "%.*s", (int)(array_end - array_start), array_start);
  array_start = strchr(buffer, '[');
  array_end = strchr(array_start != NULL ? array_start : buffer, ']');
  if(array_start == NULL || array_end == NULL) {
    return false;
  }

  cursor = array_start + 1;
  while(cursor < array_end) {
    const char *item_end = strchr(cursor, ',');
    char item[32];
    size_t len;
    if(item_end == NULL || item_end > array_end) {
      item_end = array_end;
    }
    len = (size_t)(item_end - cursor);
    if(len >= sizeof(item)) {
      len = sizeof(item) - 1;
    }
    memcpy(item, cursor, len);
    item[len] = '\0';
    if(strstr(item, "LV_STATE_FOCUSED") != NULL) {
      found_focused = true;
    }
    if(strstr(item, "LV_STATE_FOCUS_KEY") != NULL) {
      found_focus_key = true;
    }
    cursor = item_end + 1;
  }

  *focused = found_focused;
  *focus_key = found_focus_key;
  return true;
}

static lv_align_t align_from_string(const char *value)
{
  if(strcmp(value, "LV_ALIGN_TOP_LEFT") == 0) return LV_ALIGN_TOP_LEFT;
  if(strcmp(value, "LV_ALIGN_TOP_MID") == 0) return LV_ALIGN_TOP_MID;
  if(strcmp(value, "LV_ALIGN_TOP_RIGHT") == 0) return LV_ALIGN_TOP_RIGHT;
  if(strcmp(value, "LV_ALIGN_LEFT_MID") == 0) return LV_ALIGN_LEFT_MID;
  if(strcmp(value, "LV_ALIGN_RIGHT_MID") == 0) return LV_ALIGN_RIGHT_MID;
  if(strcmp(value, "LV_ALIGN_BOTTOM_LEFT") == 0) return LV_ALIGN_BOTTOM_LEFT;
  if(strcmp(value, "LV_ALIGN_BOTTOM_MID") == 0) return LV_ALIGN_BOTTOM_MID;
  if(strcmp(value, "LV_ALIGN_BOTTOM_RIGHT") == 0) return LV_ALIGN_BOTTOM_RIGHT;
  return LV_ALIGN_CENTER;
}

static lv_text_align_t text_align_from_string(const char *value)
{
  if(strcmp(value, "right") == 0) return LV_TEXT_ALIGN_RIGHT;
  if(strcmp(value, "center") == 0) return LV_TEXT_ALIGN_CENTER;
  return LV_TEXT_ALIGN_LEFT;
}

static const lv_font_t *font_from_name(const char *name)
{
  if(strcmp(name, "lv_font_montserrat_12") == 0) return &lv_font_montserrat_12;
  if(strcmp(name, "lv_font_montserrat_18") == 0) return &lv_font_montserrat_18;
  if(strcmp(name, "lv_font_montserrat_24") == 0) return &lv_font_montserrat_24;
  if(strcmp(name, "lv_font_montserrat_30") == 0) return &lv_font_montserrat_30;
  if(strcmp(name, "lv_font_montserrat_36") == 0) return &lv_font_montserrat_36;
  if(strcmp(name, "lv_font_montserrat_48") == 0) return &lv_font_montserrat_48;
  return &lv_font_montserrat_18;
}

static lv_color_t color_from_hex(const char *hex)
{
  uint32_t value = 0;
  char buffer[16];
  const char *p = hex;
  size_t len;

  if(p != NULL && *p == '#') {
    p++;
  }

  len = strlen(p);
  if(len >= sizeof(buffer)) {
    len = sizeof(buffer) - 1;
  }
  memcpy(buffer, p, len);
  buffer[len] = '\0';
  sscanf(buffer, "%x", &value);
  return lv_color_hex(value);
}

static void init_style(style_t *style)
{
  memset(style, 0, sizeof(*style));
  style->width = -1;
  style->height = -1;
  style->border_width = -1;
  style->radius = -1;
  style->padding = -1;
  style->align = LV_ALIGN_CENTER;
}

static void parse_style_object(const char *start, const char *end, style_t *style, bool reset)
{
  char value[64];
  if(reset) {
    init_style(style);
  }

  if(extract_string(start, end, "label", value, sizeof(value))) {
    snprintf(style->label, sizeof(style->label), "%s", value);
  }
  if(extract_string(start, end, "font", value, sizeof(value))) {
    snprintf(style->font, sizeof(style->font), "%s", value);
  }
  if(extract_string(start, end, "color", value, sizeof(value))) {
    snprintf(style->color, sizeof(style->color), "%s", value);
  }
  if(extract_string(start, end, "alignment", value, sizeof(value))) {
    snprintf(style->alignment, sizeof(style->alignment), "%s", value);
  }
  if(extract_int(start, end, "width", &style->width)) {
    ;
  }
  if(extract_int(start, end, "height", &style->height)) {
    ;
  }
  if(extract_int(start, end, "offsetX", &style->offset_x)) {
    ;
  }
  if(extract_int(start, end, "offsetY", &style->offset_y)) {
    ;
  }
  if(extract_string(start, end, "backgroundColor", value, sizeof(value))) {
    snprintf(style->bg_color, sizeof(style->bg_color), "%s", value);
  }
  if(extract_string(start, end, "borderColor", value, sizeof(value))) {
    snprintf(style->border_color, sizeof(style->border_color), "%s", value);
  }
  if(extract_int(start, end, "borderWidth", &style->border_width)) {
    ;
  }
  if(extract_int(start, end, "radius", &style->radius)) {
    ;
  }
  if(extract_int(start, end, "padding", &style->padding)) {
    ;
  }
  if(extract_string(start, end, "align", value, sizeof(value))) {
    style->align = align_from_string(value);
  }
}

static void parse_button_object(const char *start, const char *end, button_t *button)
{
  const char *appearance_start;
  const char *appearance_end;
  const char *text_start;
  const char *text_end;
  const char *base_start;
  const char *base_end;
  const char *focused_start;
  const char *focused_end;
  const char *position_start;
  const char *position_end;
  const char *size_start;
  const char *size_end;
  const char *focus_start;
  const char *focus_end;

  init_style(&button->style);
  init_style(&button->focused_style);
  button->focusable = true;
  button->initial_focus = false;
  button->has_focused_state = false;
  button->has_focus_key_state = false;

  if(find_object(start, end, "appearance", &appearance_start, &appearance_end)) {
    if(find_object(appearance_start, appearance_end, "text", &text_start, &text_end)) {
      parse_style_object(text_start, text_end, &button->style, true);
      if(extract_string(text_start, text_end, "label", button->text, sizeof(button->text))) {
        ;
      }
    }
    if(find_object(appearance_start, appearance_end, "baseStyle", &base_start, &base_end)) {
      parse_style_object(base_start, base_end, &button->style, false);
    }
    if(find_object(appearance_start, appearance_end, "focusedStyle", &focused_start, &focused_end)) {
      parse_style_object(focused_start, focused_end, &button->focused_style, true);
    }
    if(find_object(appearance_start, appearance_end, "position", &position_start, &position_end)) {
      extract_string(position_start, position_end, "align", button->style.alignment, sizeof(button->style.alignment));
      extract_int(position_start, position_end, "offsetX", &button->style.offset_x);
      extract_int(position_start, position_end, "offsetY", &button->style.offset_y);
      button->style.align = align_from_string(button->style.alignment);
    }
    if(find_object(appearance_start, appearance_end, "size", &size_start, &size_end)) {
      extract_int(size_start, size_end, "width", &button->style.width);
      extract_int(size_start, size_end, "height", &button->style.height);
    }
    extract_focus_flags(appearance_start, appearance_end, &button->has_focused_state, &button->has_focus_key_state);
  }

  if(find_object(start, end, "focus", &focus_start, &focus_end)) {
    extract_bool(focus_start, focus_end, "focusable", &button->focusable);
    extract_bool(focus_start, focus_end, "initialFocus", &button->initial_focus);
  }
}

static bool load_home_screen_config(screen_cfg_t *cfg)
{
  const char *paths[] = { "src/screens/home_screen/home_screen.json", "home_screen.json", "./src/screens/home_screen/home_screen.json", NULL };
  FILE *fp = NULL;
  char *json_text = NULL;
  long size;
  size_t read_size;
  const char *obj_start;
  const char *obj_end;
  int i;

  memset(cfg, 0, sizeof(*cfg));
  init_style(&cfg->title);
  init_style(&cfg->status);
  init_style(&cfg->full_press.style);
  init_style(&cfg->next.style);

  for(i = 0; paths[i] != NULL; i++) {
    fp = fopen(paths[i], "rb");
    if(fp != NULL) {
      break;
    }
  }
  if(fp == NULL) {
    return false;
  }

  fseek(fp, 0, SEEK_END);
  size = ftell(fp);
  rewind(fp);
  if(size <= 0) {
    fclose(fp);
    return false;
  }

  json_text = (char *)malloc((size_t)size + 1);
  if(json_text == NULL) {
    fclose(fp);
    return false;
  }
  read_size = fread(json_text, 1, (size_t)size, fp);
  fclose(fp);
  json_text[read_size] = '\0';

  if(find_object(json_text, json_text + read_size, "titleLabel", &obj_start, &obj_end)) {
    parse_style_object(obj_start, obj_end, &cfg->title, true);
    extract_string(obj_start, obj_end, "label", cfg->title.label, sizeof(cfg->title.label));
  }
  if(find_object(json_text, json_text + read_size, "statusLabel", &obj_start, &obj_end)) {
    parse_style_object(obj_start, obj_end, &cfg->status, true);
    extract_string(obj_start, obj_end, "label", cfg->status.label, sizeof(cfg->status.label));
  }
  if(find_object(json_text, json_text + read_size, "fullPressButton", &obj_start, &obj_end)) {
    parse_button_object(obj_start, obj_end, &cfg->full_press);
  }
  if(find_object(json_text, json_text + read_size, "nextButton", &obj_start, &obj_end)) {
    parse_button_object(obj_start, obj_end, &cfg->next);
  }

  free(json_text);
  return true;
}

static void apply_style(lv_obj_t *obj, const style_t *style)
{
  if(style->width > 0 && style->height > 0) {
    lv_obj_set_size(obj, style->width, style->height);
  }
  lv_obj_align(obj, style->align, style->offset_x, style->offset_y);
  if(style->font[0] != '\0') {
    lv_obj_set_style_text_font(obj, font_from_name(style->font), 0);
  }
  if(style->color[0] != '\0') {
    lv_obj_set_style_text_color(obj, color_from_hex(style->color), 0);
  }
  if(style->alignment[0] != '\0') {
    lv_obj_set_style_text_align(obj, text_align_from_string(style->alignment), 0);
  }
  if(style->bg_color[0] != '\0') {
    lv_obj_set_style_bg_color(obj, color_from_hex(style->bg_color), 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
  }
  if(style->border_color[0] != '\0') {
    lv_obj_set_style_border_color(obj, color_from_hex(style->border_color), 0);
  }
  if(style->border_width >= 0) {
    lv_obj_set_style_border_width(obj, style->border_width, 0);
  }
  if(style->radius >= 0) {
    lv_obj_set_style_radius(obj, style->radius, 0);
  }
  if(style->padding >= 0) {
    lv_obj_set_style_pad_all(obj, style->padding, 0);
  }
}

static void apply_button_style(lv_obj_t *btn, const button_t *button)
{
  lv_obj_remove_style(btn, NULL, LV_STATE_FOCUS_KEY);
  apply_style(btn, &button->style);
  if(button->has_focused_state) {
    lv_obj_set_style_border_width(btn, button->focused_style.border_width, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_border_color(btn, color_from_hex(button->focused_style.border_color), LV_PART_MAIN | LV_STATE_FOCUSED);
  }
  if(button->has_focus_key_state) {
    lv_obj_set_style_border_width(btn, button->focused_style.border_width, LV_PART_MAIN | LV_STATE_FOCUS_KEY);
    lv_obj_set_style_border_color(btn, color_from_hex(button->focused_style.border_color), LV_PART_MAIN | LV_STATE_FOCUS_KEY);
  }
  lv_obj_set_clickable(btn, true);
}

static lv_obj_t *create_label(lv_obj_t *parent, const style_t *style, const char *text)
{
  lv_obj_t *label = lv_label_create(parent);
  lv_label_set_text(label, text);
  apply_style(label, style);
  return label;
}

static lv_obj_t *create_button(lv_obj_t *parent, const button_t *button)
{
  lv_obj_t *btn = lv_btn_create(parent);
  lv_obj_t *label = lv_label_create(btn);
  lv_label_set_text(label, button->text);
  lv_obj_center(label);
  apply_button_style(btn, button);
  return btn;
}

lv_obj_t *create_home_screen(void)
{
  screen_cfg_t cfg;
  lv_obj_t *screen = lv_obj_create(NULL);
  lv_obj_t *title;
  lv_obj_t *status;
  lv_obj_t *btn1;
  lv_obj_t *btn2;
  lv_group_t *group;

  if(!load_home_screen_config(&cfg)) {
    snprintf(cfg.title.label, sizeof(cfg.title.label), "LVGL Button Demo");
    snprintf(cfg.title.font, sizeof(cfg.title.font), "lv_font_montserrat_48");
    snprintf(cfg.title.color, sizeof(cfg.title.color), "#A83232");
    snprintf(cfg.title.alignment, sizeof(cfg.title.alignment), "center");
    snprintf(cfg.title.bg_color, sizeof(cfg.title.bg_color), "#FFFFFF");
    snprintf(cfg.title.border_color, sizeof(cfg.title.border_color), "#000000");
    cfg.title.width = 240;
    cfg.title.height = 40;
    cfg.title.align = LV_ALIGN_TOP_MID;
    cfg.title.offset_x = 0;
    cfg.title.offset_y = 30;
    cfg.title.border_width = 2;
    cfg.title.radius = 12;
    cfg.title.padding = 6;

    snprintf(cfg.status.label, sizeof(cfg.status.label), "Press a button");
    snprintf(cfg.status.font, sizeof(cfg.status.font), "lv_font_montserrat_18");
    snprintf(cfg.status.color, sizeof(cfg.status.color), "#FFD400");
    snprintf(cfg.status.alignment, sizeof(cfg.status.alignment), "center");
    cfg.status.width = 220;
    cfg.status.height = 30;
    cfg.status.align = LV_ALIGN_CENTER;
    cfg.status.offset_x = 0;
    cfg.status.offset_y = 40;

    snprintf(cfg.full_press.text, sizeof(cfg.full_press.text), "FullPress");
    snprintf(cfg.full_press.style.font, sizeof(cfg.full_press.style.font), "lv_font_montserrat_36");
    snprintf(cfg.full_press.style.color, sizeof(cfg.full_press.style.color), "#FFFFFF");
    snprintf(cfg.full_press.style.bg_color, sizeof(cfg.full_press.style.bg_color), "#1F1F1F");
    snprintf(cfg.full_press.style.border_color, sizeof(cfg.full_press.style.border_color), "#FFFFFF");
    cfg.full_press.style.width = 220;
    cfg.full_press.style.height = 80;
    cfg.full_press.style.align = LV_ALIGN_TOP_MID;
    cfg.full_press.style.offset_x = 0;
    cfg.full_press.style.offset_y = 90;
    cfg.full_press.style.border_width = 2;
    cfg.full_press.style.radius = 12;
    cfg.full_press.style.padding = 6;
    cfg.full_press.focusable = true;
    cfg.full_press.initial_focus = true;
    snprintf(cfg.full_press.focused_style.border_color, sizeof(cfg.full_press.focused_style.border_color), "#FFD400");
    cfg.full_press.focused_style.border_width = 6;
    cfg.full_press.has_focused_state = true;

    snprintf(cfg.next.text, sizeof(cfg.next.text), "Next");
    snprintf(cfg.next.style.font, sizeof(cfg.next.style.font), "lv_font_montserrat_36");
    snprintf(cfg.next.style.color, sizeof(cfg.next.style.color), "#FFFFFF");
    snprintf(cfg.next.style.bg_color, sizeof(cfg.next.style.bg_color), "#1F1F1F");
    snprintf(cfg.next.style.border_color, sizeof(cfg.next.style.border_color), "#FFFFFF");
    cfg.next.style.width = 220;
    cfg.next.style.height = 80;
    cfg.next.style.align = LV_ALIGN_BOTTOM_MID;
    cfg.next.style.offset_x = 0;
    cfg.next.style.offset_y = -30;
    cfg.next.style.border_width = 2;
    cfg.next.style.radius = 12;
    cfg.next.style.padding = 6;
    cfg.next.focusable = true;
    cfg.next.initial_focus = false;
    snprintf(cfg.next.focused_style.border_color, sizeof(cfg.next.focused_style.border_color), "#FFD400");
    cfg.next.focused_style.border_width = 6;
    cfg.next.has_focused_state = true;
    cfg.next.has_focus_key_state = true;
  }

  title = create_label(screen, &cfg.title, cfg.title.label);
  status = create_label(screen, &cfg.status, cfg.status.label);

  group = lv_group_get_default();
  if(group == NULL) {
    group = lv_group_create();
    lv_group_set_default(group);
  }

  btn1 = create_button(screen, &cfg.full_press);
  lv_obj_add_event_cb(btn1, fullpress_btn_event_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(btn1, fullpress_btn_event_cb, LV_EVENT_SHORT_CLICKED, NULL);

  btn2 = create_button(screen, &cfg.next);
  lv_obj_add_event_cb(btn2, fullpress_btn_event_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(btn2, fullpress_btn_event_cb, LV_EVENT_SHORT_CLICKED, NULL);

  if(cfg.full_press.focusable) lv_group_add_obj(group, btn1);
  if(cfg.next.focusable) lv_group_add_obj(group, btn2);
  if(cfg.full_press.initial_focus) lv_group_focus_obj(btn1);
  else if(cfg.next.initial_focus) lv_group_focus_obj(btn2);

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
