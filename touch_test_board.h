/*********************
 *      INCLUDES
 *********************/
#ifndef __TOUCH_TEST_BOARD_H__
#define __TOUCH_TEST_BOARD_H__

#include "lvgl/lvgl.h"

/*********************
 *      DEFINES
 *********************/
#define TOUCH_POINT_SIZE    4   // 画笔点的大小
#define MAX_TOUCH_POINTS    2000 // 最大记录的触摸点数量

/*********************
 *      TYPEDEFS
 *********************/
typedef struct {
    lv_coord_t x;
    lv_coord_t y;
    bool valid;
} touch_point_t;

typedef struct {
    lv_obj_t* bg_cont;          // 背景容器
    lv_obj_t* canvas;           // 画布
    lv_obj_t* close_btn;        // 关闭按钮
    lv_obj_t* clear_btn;        // 清除按钮
    lv_obj_t* coord_label;      // 坐标显示标签
    
    void* canvas_buf;           // 画布缓冲区
    touch_point_t points[MAX_TOUCH_POINTS]; // 触摸点记录
    uint16_t point_count;       // 当前记录的点数
    
    bool is_drawing;            // 是否正在绘画
    lv_coord_t last_x;          // 上一个触摸点X坐标
    lv_coord_t last_y;          // 上一个触摸点Y坐标
} touch_test_board_t;

/*********************
 * GLOBAL PROTOTYPES
 *********************/

/**
 * 启动触摸测试画板
 */
void touch_test_board_start(void);

/**
 * 停止触摸测试画板
 */
void touch_test_board_stop(void);

/**
 * 获取当前是否正在运行画板
 * @return true: 正在运行, false: 未运行
 */
bool touch_test_board_is_running(void);

/**
 * 设置触摸测试标志位
 */
void touch_test_board_trigger(void);

// 外部变量声明
extern uint8_t touch_test_flag;

#endif /* __TOUCH_TEST_BOARD_H__ */ 