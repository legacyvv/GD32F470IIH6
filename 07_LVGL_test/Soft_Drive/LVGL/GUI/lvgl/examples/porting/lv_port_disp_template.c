/**
 * @file lv_port_disp_templ.c
 *
 */

 /*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp_template.h"
#include "../../lvgl.h"
#include "lcd.h"

/*********************
 *      DEFINES
 *********************/
#define LVGL_BUF_DIV 		8
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_disp_drv_t g_disp_drv;                         /*Descriptor of a display driver*/

/* Example for 1) */
static lv_disp_draw_buf_t draw_buf_dsc_1;

/*A buffer for LVGL_BUF_DIV rows*/
static lv_color_t buf_1[CANVAS_H * CANVAS_V / LVGL_BUF_DIV] __attribute((aligned(256)));

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/
	lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, CANVAS_H * CANVAS_V / LVGL_BUF_DIV);   /*Initialize the display buffer*/

    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/
	lv_disp_drv_init(&g_disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    g_disp_drv.hor_res = CANVAS_H;
    g_disp_drv.ver_res = CANVAS_V;

    /*Used to copy the buffer's content to the display*/
    g_disp_drv.flush_cb = disp_flush;

    /*Set a display buffer*/
    g_disp_drv.draw_buf = &draw_buf_dsc_1;

    /*Finally register the driver*/
    lv_disp_drv_register(&g_disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
    /*You code here*/
	clear_pixel(0x0000);
	display_init();
	ipa_config(0x00000000);
}

/*Flush the content of the internal buffer the specific area on the display
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_disp_flush_ready()' has to be called when finished.*/
void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
	ipa_trig(area->x1, area->y1, area->x2, area->y2, (uint32_t)&buf_1, (uint32_t)&lcd_tli_buffer);
}


void IPA_IRQHandler(void)
{
	if(ipa_interrupt_flag_get(IPA_INT_FLAG_FTF))
	{
		ipa_interrupt_flag_clear(IPA_INT_FLAG_FTF);
		/*IMPORTANT!!!
		*Inform the graphics library that you are ready with the flushing*/
		lv_disp_flush_ready(&g_disp_drv);
	}
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
