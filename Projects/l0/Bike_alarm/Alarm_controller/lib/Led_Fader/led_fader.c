/******************************************************************************
 * @file Led
 * @brief driver example a simple Led
 * @author Luos
 * @version 0.0.0
 ******************************************************************************/
#include "led_fader.h"
#include <math.h>
#include "string.h"
#include "led_drv.h"
#include "fader.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
//fader service new type
typedef enum
{
    FADER_TYPE = LUOS_LAST_TYPE

} fader_type_t;

//led driver struct
typedef struct driver_color
{
    void (*init)(void);
    uint8_t (*read)(color_t *);
    uint8_t (*write)(color_t *);
    uint8_t (*configure)(void *);

} driver_color_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

time_luos_t time;

//Initialization of led driver functions
driver_color_t led_driver = {
    .init      = LedDrv_Init,
    .read      = 0,
    .write     = LedDrv_Write,
    .configure = 0};

/*******************************************************************************
 * Function
 ******************************************************************************/
static void Led_MsgHandler(service_t *service, msg_t *msg);
static void Fader_MsgHandler(service_t *service, msg_t *msg);
/******************************************************************************
 * @brief package init must be call in project init
 * @param None
 * @return None
 ******************************************************************************/
void LedFader_Init(void)
{
    revision_t revision = {.unmap = REV};

    led_driver.init();

    Luos_CreateService(Led_MsgHandler, COLOR_TYPE, "rgb_led", revision);
    Luos_CreateService(Fader_MsgHandler, FADER_TYPE, "fader", revision);
    //Initialize Fader App
    Fader_Init();
}
/******************************************************************************
 * @brief package loop must be call in project loop
 * @param None
 * @return None
 ******************************************************************************/
void LedFader_Loop(void)
{
    Fader_Loop();
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

/******************************************************************************
 * @brief Msg manager callback when a msg receive for the fader service
 * @param Service destination
 * @param Msg receive
 * @return None
 ******************************************************************************/
void Fader_MsgHandler(service_t *service, msg_t *msg)
{
    color_t rgb;
    time_luos_t time;

    if (msg->header.cmd == COLOR)
    {
        IlluminanceOD_ColorFromMsg((color_t *)&rgb, msg);
        Fader_WriteColor(&rgb);
        return;
    }
    else if (msg->header.cmd == TIME)
    {
        // save transition time
        TimeOD_TimeFromMsg((time_luos_t *)&time, msg);
        Fader_WriteTime(time);
        return;
    }
}
