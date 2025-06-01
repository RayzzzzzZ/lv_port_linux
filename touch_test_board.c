/*********************
 *      INCLUDES
 *********************/
#include "touch_test_board.h"
//#include "gui_data_comm.h"
#include <stdio.h>
#include <stdlib.h>

/*********************
 *      DEFINES
 *********************/
#define CANVAS_WIDTH    480
#define CANVAS_HEIGHT   800
#define BUTTON_HEIGHT   60
#define BUTTON_WIDTH    120

/*********************
 *  STATIC PROTOTYPES
 *********************/
static void touch_event_cb(lv_event_t * e);
static void close_btn_cb(lv_event_t * e);
static void clear_btn_cb(lv_event_t * e);
static void draw_point(lv_coord_t x, lv_coord_t y);
static void draw_line(lv_coord_t x1, lv_coord_t y1, lv_coord_t x2, lv_coord_t y2);
static void clear_canvas(void);
static void update_coord_label(lv_coord_t x, lv_coord_t y, lv_indev_state_t state);

/*********************
 *  STATIC VARIABLES
 *********************/
static touch_test_board_t* p_touch_test_board = NULL;

/*********************
 *   GLOBAL FUNCTIONS
 *********************/

void touch_test_board_start(void)
{
    if (p_touch_test_board != NULL) {
        return; // 已经启动了
    }

    // 分配内存
    p_touch_test_board = (touch_test_board_t*)lv_malloc(sizeof(touch_test_board_t));
    LV_ASSERT(p_touch_test_board);
    lv_memzero(p_touch_test_board, sizeof(touch_test_board_t));

    // 创建背景容器，覆盖整个屏幕
    lv_obj_t *bg_obj = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(bg_obj);
    lv_obj_set_scrollbar_mode(bg_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(bg_obj, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC);
    lv_obj_set_style_bg_color(bg_obj, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(bg_obj, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_size(bg_obj, CANVAS_WIDTH, CANVAS_HEIGHT);
    lv_obj_set_pos(bg_obj, 0, 0);
    lv_obj_update_layout(bg_obj);
    p_touch_test_board->bg_cont = bg_obj;

    // 分配画布缓冲区
    uint32_t buf_size = CANVAS_WIDTH * CANVAS_HEIGHT * lv_color_format_get_size(LV_COLOR_FORMAT_ARGB8888);
    p_touch_test_board->canvas_buf = (void*)lv_malloc(buf_size);
    LV_ASSERT(p_touch_test_board->canvas_buf);

    // 创建画布
    p_touch_test_board->canvas = lv_canvas_create(bg_obj);
    lv_canvas_set_buffer(p_touch_test_board->canvas, p_touch_test_board->canvas_buf, 
                        CANVAS_WIDTH, CANVAS_HEIGHT, LV_COLOR_FORMAT_ARGB8888);
    lv_obj_set_size(p_touch_test_board->canvas, CANVAS_WIDTH, CANVAS_HEIGHT);
    lv_obj_set_pos(p_touch_test_board->canvas, 0, 0);
    
    // 设置画布背景为白色
    lv_canvas_fill_bg(p_touch_test_board->canvas, lv_color_white(), LV_OPA_COVER);

    // 添加触摸事件
    lv_obj_add_event_cb(bg_obj, touch_event_cb, LV_EVENT_PRESSING, NULL);
    lv_obj_add_event_cb(bg_obj, touch_event_cb, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(bg_obj, touch_event_cb, LV_EVENT_RELEASED, NULL);

    // 创建关闭按钮 (右上角)
    p_touch_test_board->close_btn = lv_btn_create(bg_obj);
    lv_obj_set_size(p_touch_test_board->close_btn, BUTTON_WIDTH, BUTTON_HEIGHT);
    lv_obj_set_pos(p_touch_test_board->close_btn, CANVAS_WIDTH - BUTTON_WIDTH - 10, 10);
    lv_obj_set_style_bg_color(p_touch_test_board->close_btn, lv_color_hex(0xFF0000), LV_PART_MAIN);
    lv_obj_add_event_cb(p_touch_test_board->close_btn, close_btn_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t* close_label = lv_label_create(p_touch_test_board->close_btn);
    lv_label_set_text(close_label, "CLOSE");
    lv_obj_set_style_text_color(close_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_center(close_label);

    // 创建清除按钮 (右上角，关闭按钮下方)
    p_touch_test_board->clear_btn = lv_btn_create(bg_obj);
    lv_obj_set_size(p_touch_test_board->clear_btn, BUTTON_WIDTH, BUTTON_HEIGHT);
    lv_obj_set_pos(p_touch_test_board->clear_btn, CANVAS_WIDTH - BUTTON_WIDTH - 10, 10 + BUTTON_HEIGHT + 10);
    lv_obj_set_style_bg_color(p_touch_test_board->clear_btn, lv_color_hex(0x0000FF), LV_PART_MAIN);
    lv_obj_add_event_cb(p_touch_test_board->clear_btn, clear_btn_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t* clear_label = lv_label_create(p_touch_test_board->clear_btn);
    lv_label_set_text(clear_label, "CLEAR");
    lv_obj_set_style_text_color(clear_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_center(clear_label);

    // 创建坐标显示标签 (左上角)
    p_touch_test_board->coord_label = lv_label_create(bg_obj);
    lv_obj_set_pos(p_touch_test_board->coord_label, 10, 10);
    lv_obj_set_style_text_color(p_touch_test_board->coord_label, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_color(p_touch_test_board->coord_label, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(p_touch_test_board->coord_label, LV_OPA_80, LV_PART_MAIN);
    lv_obj_set_style_pad_all(p_touch_test_board->coord_label, 5, LV_PART_MAIN);
    lv_label_set_text(p_touch_test_board->coord_label, "Touch Test Board\nTouch to draw");

    // 初始化状态
    p_touch_test_board->is_drawing = false;
    p_touch_test_board->point_count = 0;
    p_touch_test_board->last_x = 0;
    p_touch_test_board->last_y = 0;

    printf("Touch test board started - Size: %dx%d\n", CANVAS_WIDTH, CANVAS_HEIGHT);
}

void touch_test_board_stop(void)
{
    if (p_touch_test_board == NULL) {
        return;
    }

    // 释放画布缓冲区
    if (p_touch_test_board->canvas_buf) {
        lv_free(p_touch_test_board->canvas_buf);
    }

    // 删除UI对象
    if (p_touch_test_board->bg_cont) {
        lv_obj_delete(p_touch_test_board->bg_cont);
    }

    // 释放主结构体
    lv_free(p_touch_test_board);
    p_touch_test_board = NULL;

    printf("Touch test board stopped\n");
}

bool touch_test_board_is_running(void)
{
    return (p_touch_test_board != NULL);
}

void touch_test_board_trigger(void)
{
    extern uint8_t touch_test_flag;
    touch_test_flag = 1;
    printf("Touch test board triggered\n");
}

/*********************
 *   STATIC FUNCTIONS
 *********************/

static void touch_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    
    if (p_touch_test_board == NULL) return;

    // 获取输入设备
    lv_indev_t * indev = lv_indev_active();
    if (indev == NULL) return;

    // 获取触摸点坐标
    lv_point_t point;
    lv_indev_get_point(indev, &point);
    
    // 获取输入设备状态 (LVGL v9.3.0兼容方式)
    lv_indev_state_t state = lv_indev_get_state(indev);

    // 更新坐标显示
    update_coord_label(point.x, point.y, state);

    // 处理触摸事件
    if (code == LV_EVENT_PRESSED) {
        // 开始绘画
        p_touch_test_board->is_drawing = true;
        p_touch_test_board->last_x = point.x;
        p_touch_test_board->last_y = point.y;
        draw_point(point.x, point.y);
        
        // 记录触摸点
        if (p_touch_test_board->point_count < MAX_TOUCH_POINTS) {
            p_touch_test_board->points[p_touch_test_board->point_count].x = point.x;
            p_touch_test_board->points[p_touch_test_board->point_count].y = point.y;
            p_touch_test_board->points[p_touch_test_board->point_count].valid = true;
            p_touch_test_board->point_count++;
        }
    }
    else if (code == LV_EVENT_PRESSING && p_touch_test_board->is_drawing) {
        // 继续绘画
        draw_line(p_touch_test_board->last_x, p_touch_test_board->last_y, point.x, point.y);
        p_touch_test_board->last_x = point.x;
        p_touch_test_board->last_y = point.y;
        
        // 记录触摸点
        if (p_touch_test_board->point_count < MAX_TOUCH_POINTS) {
            p_touch_test_board->points[p_touch_test_board->point_count].x = point.x;
            p_touch_test_board->points[p_touch_test_board->point_count].y = point.y;
            p_touch_test_board->points[p_touch_test_board->point_count].valid = true;
            p_touch_test_board->point_count++;
        }
    }
    else if (code == LV_EVENT_RELEASED) {
        // 结束绘画
        p_touch_test_board->is_drawing = false;
    }
}

static void close_btn_cb(lv_event_t * e)
{
    touch_test_board_stop();
}

static void clear_btn_cb(lv_event_t * e)
{
    clear_canvas();
}

static void draw_point(lv_coord_t x, lv_coord_t y)
{
    if (p_touch_test_board == NULL || p_touch_test_board->canvas == NULL) return;

    // 使用LVGL v9.3.0的新绘制层API
    lv_layer_t layer;
    lv_canvas_init_layer(p_touch_test_board->canvas, &layer);

    // 在指定位置画一个红色的圆点
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = lv_color_hex(0xFF0000);  // 红色
    rect_dsc.bg_opa = LV_OPA_COVER;
    rect_dsc.radius = TOUCH_POINT_SIZE / 2;

    lv_area_t area = {
        .x1 = x - TOUCH_POINT_SIZE / 2,
        .y1 = y - TOUCH_POINT_SIZE / 2,
        .x2 = x + TOUCH_POINT_SIZE / 2 - 1,
        .y2 = y + TOUCH_POINT_SIZE / 2 - 1
    };

    lv_draw_rect(&layer, &rect_dsc, &area);
    lv_canvas_finish_layer(p_touch_test_board->canvas, &layer);
}

static void draw_line(lv_coord_t x1, lv_coord_t y1, lv_coord_t x2, lv_coord_t y2)
{
    if (p_touch_test_board == NULL || p_touch_test_board->canvas == NULL) return;

    // 使用LVGL v9.3.0的新绘制层API
    lv_layer_t layer;
    lv_canvas_init_layer(p_touch_test_board->canvas, &layer);

    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.color = lv_color_hex(0xFF0000);  // 红色
    line_dsc.width = TOUCH_POINT_SIZE;
    line_dsc.opa = LV_OPA_COVER;
    line_dsc.p1.x = x1;
    line_dsc.p1.y = y1;
    line_dsc.p2.x = x2;
    line_dsc.p2.y = y2;

    lv_draw_line(&layer, &line_dsc);
    lv_canvas_finish_layer(p_touch_test_board->canvas, &layer);
}

static void clear_canvas(void)
{
    if (p_touch_test_board == NULL || p_touch_test_board->canvas == NULL) return;

    // 清除画布，重新填充白色背景
    lv_canvas_fill_bg(p_touch_test_board->canvas, lv_color_white(), LV_OPA_COVER);
    
    // 重置触摸点记录
    p_touch_test_board->point_count = 0;
    lv_memzero(p_touch_test_board->points, sizeof(p_touch_test_board->points));
    
    printf("Canvas cleared\n");
}

static void update_coord_label(lv_coord_t x, lv_coord_t y, lv_indev_state_t state)
{
    if (p_touch_test_board == NULL || p_touch_test_board->coord_label == NULL) return;

    static char coord_text[128];
    const char* state_str = (state == LV_INDEV_STATE_PRESSED) ? "PRESSED" : "RELEASED";
    
    snprintf(coord_text, sizeof(coord_text), 
             "Touch Test Board\n"
             "Coord: (%d, %d)\n"
             "State: %s\n"
             "Points: %d",
             x, y, state_str, p_touch_test_board->point_count);
    
    lv_label_set_text(p_touch_test_board->coord_label, coord_text);
} 