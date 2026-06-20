// Based on Kvaesitso Orbit Clock
// https://github.com/MM2-0/Kvaesitso/blob/main/app/ui/src/main/java/de/mm20/launcher2/ui/launcher/widgets/clock/clocks/OrbitClock.kt
// Font from the horizon watchface https://github.com/jrmobley/horizon-watchface/

#include <pebble.h>
#include <pebble-fctx/fctx.h>
#include <pebble-fctx/fpath.h>
#include <pebble-fctx/ffont.h>
#include <pebble-utf8/pebble-utf8.h>

#define SCREENSHOT 0

#define PHI_F 1.618033f
#define FIXED_TRIG_MAX_RATIO INT_TO_FIXED(TRIG_MAX_RATIO)

#define SETTINGS_KEY 1

struct Settings
{
    GColor ColorBackground;
    GColor ColorClock;
    GColor ColorDigits;
    bool EnableDate;
    bool EnableSeconds;
} __attribute__((__packed__)) g_settings;

static void default_settings()
{
    g_settings.ColorBackground = GColorOxfordBlue;
    g_settings.ColorClock = GColorWhite;
    g_settings.ColorDigits = GColorBlack;
    g_settings.EnableDate = true;
    g_settings.EnableSeconds = false;
}

static Window *g_window;
static Layer *g_clock;
static FFont *g_font;
struct tm g_local_time;

bool show_date(bool quick_view)
{
#ifdef PBL_ROUND
    return false;
#endif
    return (quick_view) ? false : g_settings.EnableDate;
}

static inline FPoint time_offset(int32_t angle, fixed_t r)
{
    return FPoint(INT_TO_FIXED(sin_lookup(angle)) * r / FIXED_TRIG_MAX_RATIO, INT_TO_FIXED(-cos_lookup(angle)) * r / FIXED_TRIG_MAX_RATIO);
}

void draw_dashed_circle(FContext *fctx, FPoint center, fixed_t r, fixed_t dash_width, fixed_t dash_length, int count)
{

    fixed_t half_width = dash_length / 2;
    fixed_t outer_edge = r;
    fixed_t inner_edge = outer_edge - dash_width;

    int32_t angle_increment = TRIG_MAX_ANGLE / count;

    for (int i = 0; i < count; i++)
    {
        int32_t dash_angle = i * angle_increment;

        fctx_begin_fill(fctx);
        fctx_set_fill_color(fctx, g_settings.ColorClock);
        fctx_set_offset(fctx, center);
        fctx_set_scale(fctx, FPointOne, FPointOne);
        fctx_set_rotation(fctx, dash_angle);

        fctx_move_to(fctx, FPoint(-half_width, -inner_edge));
        fctx_line_to(fctx, FPoint(half_width, -inner_edge));
        fctx_line_to(fctx, FPoint(half_width, -outer_edge));
        fctx_line_to(fctx, FPoint(-half_width, -outer_edge));

        fctx_close_path(fctx);
        fctx_end_fill(fctx);
    }
}

void draw_time_circle(FContext *fctx, FPoint center, fixed_t r)
{

    fctx_begin_fill(fctx);
    fctx_set_fill_color(fctx, g_settings.ColorClock);
    fctx_plot_circle(fctx, &center, r);
    fctx_end_fill(fctx);
}

void on_clock_update(Layer *layer, GContext *ctx)
{

#if SCREENSHOT
    g_local_time.tm_hour = 13;
    g_local_time.tm_min = 50;
#endif
    // g_local_time.tm_hour = 12;
    // g_local_time.tm_min = 0;

    GRect bounds = layer_get_unobstructed_bounds(layer);
    GRect full_bounds = layer_get_bounds(layer);
    bool quick_view = (full_bounds.size.h > bounds.size.h) ? true : false;

    FPoint center = FPointI(bounds.size.w / 2, bounds.size.h / 2);
    fixed_t safe_width = (bounds.size.w >= bounds.size.h) ? INT_TO_FIXED(bounds.size.w) : INT_TO_FIXED(bounds.size.h);
    fixed_t clock_width = safe_width * PBL_IF_ROUND_ELSE(0.95f, 0.85f); // scale

    int date_font_size = bounds.size.h / 8;
    fixed_t date_height = INT_TO_FIXED(date_font_size);
    if (show_date(quick_view))
    {
        center.y = center.y - date_height / 2;
    }
    FPoint date_offset = FPoint(center.x, clock_width + date_height / 2);

    fixed_t second_radius = clock_width * 0.07f;
    fixed_t minute_radius = clock_width * 0.19f;
    fixed_t hour_radius = clock_width * 0.38f;

    fixed_t second_circle = clock_width * 0.0175f;
    fixed_t minute_circle = clock_width * 0.07f;
    fixed_t hour_circle = clock_width * 0.09f;

    int16_t minute_height = FIXED_TO_INT((minute_circle * 2) - (minute_circle / 2));
    int16_t hour_height = FIXED_TO_INT((hour_circle * 2) - (hour_circle / 2));

    int second_angle = TRIG_MAX_ANGLE * g_local_time.tm_sec / 60;
    FPoint seconds_offset = fpoint_add(center, time_offset(second_angle, second_radius));
    int minute_angle = TRIG_MAX_ANGLE * g_local_time.tm_min / 60;
    FPoint minute_offset = fpoint_add(center, time_offset(minute_angle, minute_radius));
    int hour_angle = (TRIG_MAX_ANGLE * (((g_local_time.tm_hour % 12) * 6) + (g_local_time.tm_min / 10))) / (12 * 6);
    FPoint hour_offset = fpoint_add(center, time_offset(hour_angle, hour_radius));

    FContext fctx;
    fctx_init_context(&fctx, ctx);
    fctx_set_color_bias(&fctx, 0);

    int second_dashes = FIXED_TO_INT(clock_width / 10);
    if (g_settings.EnableSeconds)
    {
        draw_dashed_circle(&fctx, center, second_radius, INT_TO_FIXED(2), INT_TO_FIXED(2), second_dashes);
    }
    draw_dashed_circle(&fctx, center, minute_radius, INT_TO_FIXED(2), INT_TO_FIXED(3), second_dashes * PHI_F);
    draw_dashed_circle(&fctx, center, hour_radius, INT_TO_FIXED(2), INT_TO_FIXED(5), second_dashes * PHI_F * PHI_F);

    if (g_settings.EnableSeconds)
    {
        draw_time_circle(&fctx, seconds_offset, second_circle);
    }
    draw_time_circle(&fctx, minute_offset, minute_circle);
    draw_time_circle(&fctx, hour_offset, hour_circle);

    /* MINUTE TEXT */
    fctx_begin_fill(&fctx);
    fctx_set_fill_color(&fctx, g_settings.ColorDigits);
    fctx_set_text_em_height(&fctx, g_font, minute_height);
    static char minute_buffer[3];
    strftime(minute_buffer, sizeof(minute_buffer), "%M", &g_local_time);
    fctx_set_rotation(&fctx, 0);
    fctx_set_offset(&fctx, minute_offset);
    fctx_draw_string(&fctx, minute_buffer, g_font, GTextAlignmentCenter, FTextAnchorMiddle);
    fctx_end_fill(&fctx);

    /* HOUR TEXT */
    fctx_begin_fill(&fctx);
    fctx_set_fill_color(&fctx, g_settings.ColorDigits);
    fctx_set_text_em_height(&fctx, g_font, hour_height);
    static char hour_buffer[3];
    if (clock_is_24h_style())
    {
        strftime(hour_buffer, sizeof(hour_buffer), "%H", &g_local_time);
    }
    else
    {
        strftime(hour_buffer, sizeof(hour_buffer), "%I", &g_local_time);
    }
    fctx_set_rotation(&fctx, 0);
    fctx_set_offset(&fctx, hour_offset);
    fctx_draw_string(&fctx, hour_buffer, g_font, GTextAlignmentCenter, FTextAnchorMiddle);
    fctx_end_fill(&fctx);

    /* DATE TEXT */
    if (show_date(quick_view))
    {
        fctx_begin_fill(&fctx);
        fctx_set_fill_color(&fctx, g_settings.ColorClock);
        fctx_set_text_em_height(&fctx, g_font, date_font_size);

        static char date_buffer[24];
        strftime(date_buffer, sizeof(date_buffer), "%a %b %e", &g_local_time);
        utf8_str_to_upper(date_buffer);

        fctx_set_rotation(&fctx, 0);
        fctx_set_offset(&fctx, date_offset);
        fctx_draw_string(&fctx, date_buffer, g_font, GTextAlignmentCenter, FTextAnchorMiddle);
        fctx_end_fill(&fctx);
    }

    fctx_deinit_context(&fctx);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
    g_local_time = *tick_time;
    layer_mark_dirty(g_clock);
}

static void register_tick_timer()
{
    if (g_settings.EnableSeconds)
    {
        tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
    }
    else
    {
        tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
    }
}

static void save_settings()
{
    persist_write_data(SETTINGS_KEY, &g_settings, sizeof(g_settings));
    window_set_background_color(g_window, g_settings.ColorBackground);
    register_tick_timer();
    layer_mark_dirty(g_clock);
}

static void load_settings()
{
    default_settings();
    persist_read_data(SETTINGS_KEY, &g_settings, sizeof(g_settings));
}

static void inbox_received_handler(DictionaryIterator *iter, void *context)
{
    Tuple *color_background = dict_find(iter, MESSAGE_KEY_COLOR_BACKGROUND);
    if (color_background)
    {
        g_settings.ColorBackground = GColorFromHEX(color_background->value->int32);
    }

    Tuple *color_clock = dict_find(iter, MESSAGE_KEY_COLOR_CLOCK);
    if (color_clock)
    {
        g_settings.ColorClock = GColorFromHEX(color_clock->value->int32);
    }

    Tuple *color_digits = dict_find(iter, MESSAGE_KEY_COLOR_DIGITS);
    if (color_digits)
    {
        g_settings.ColorDigits = GColorFromHEX(color_digits->value->int32);
    }

    Tuple *enable_date = dict_find(iter, MESSAGE_KEY_ENABLE_DATE);
    if (enable_date)
    {
        if (enable_date->value->int32 == 0)
        {
            g_settings.EnableDate = false;
        }
        else
        {
            g_settings.EnableDate = true;
        }
    }

    Tuple *enable_seconds = dict_find(iter, MESSAGE_KEY_ENABLE_SECONDS);
    if (enable_seconds)
    {
        if (enable_seconds->value->int32 == 0)
        {
            g_settings.EnableSeconds = false;
        }
        else
        {
            g_settings.EnableSeconds = true;
        }
    }

    save_settings();
}

static void init()
{
    load_settings();

    app_message_register_inbox_received(inbox_received_handler);
    app_message_open(128, 128);

    g_font = ffont_create_from_resource(RESOURCE_ID_FONT_DIN_CONDENSED);
    ffont_debug_log(g_font, APP_LOG_LEVEL_DEBUG);

    g_window = window_create();
    window_set_background_color(g_window, g_settings.ColorBackground);
    window_stack_push(g_window, true);
    Layer *window_layer = window_get_root_layer(g_window);
    GRect window_frame = layer_get_frame(window_layer);

    g_clock = layer_create(window_frame);
    layer_set_update_proc(g_clock, &on_clock_update);
    layer_add_child(window_layer, g_clock);

    time_t now = time(NULL);
    g_local_time = *localtime(&now);

    register_tick_timer();
}

static void deinit()
{
    tick_timer_service_unsubscribe();
    window_destroy(g_window);
    layer_destroy(g_clock);
    ffont_destroy(g_font);
}

int main(void)
{
    init();
    app_event_loop();
    deinit();
}
