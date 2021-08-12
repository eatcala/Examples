/******************************************************************************
 * @file Led
 * @brief driver example a simple Led
 * @author Luos
 * @version 0.0.0
 ******************************************************************************/
#include "led.h"
#include <math.h>
#include "string.h"
#include "led_drv.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
//led driver struct
typedef struct driver_color
{
    void (*init)(void);
    uint8_t (*read)(color_t *);
    uint8_t (*write)(color_t *);
    uint8_t (*configure)(void *);

}driver_color_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

//Initialization of led driver functions
driver_color_t led_driver = {
    .init  = LedDrv_Init,
    .read  = 0,
    .write = LedDrv_Write,
    .configure = 0};

/*******************************************************************************
 * Function
 ******************************************************************************/
static void Led_MsgHandler(service_t *service, msg_t *msg);

/******************************************************************************
 * @brief package init must be call in project init
 * @param None
 * @return None
 ******************************************************************************/
void Led_Init(void)
{
    revision_t revision = {.unmap = REV};

    led_driver.init();

    Luos_CreateService(Led_MsgHandler, COLOR_TYPE, "rgb_led", revision);
}
/******************************************************************************
 * @brief package loop must be call in project loop
 * @param None
 * @return None
 ******************************************************************************/
void Led_Loop(void)
{
}
/******************************************************************************
 * @brief Msg manager callback when a msg receive for the led service
 * @param Service destination
 * @param Msg receive
 * @return None
 ******************************************************************************/
static void Led_MsgHandler(service_t *service, msg_t *msg)
{
    color_t rgb;
    if (msg->header.cmd == COLOR)
    {
        // change led target color
        IlluminanceOD_ColorFromMsg((color_t *)&rgb, msg);
        led_driver.write(&rgb);
        return;
    }
}
