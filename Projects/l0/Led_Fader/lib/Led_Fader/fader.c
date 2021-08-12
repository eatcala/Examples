/******************************************************************************
 * @file Fader
 * @brief app that makes a Led fade after a blink
 * @author Luos
 * @version 0.0.0
 ******************************************************************************/

#include "fader.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

volatile color_t last_rgb;
color_t target_rgb;
time_luos_t time;
uint32_t init_time     = 0;
static int elapsed_ms  = 0;
volatile float coef[3] = {0.0};
/*******************************************************************************
 * Function
 ******************************************************************************/

/******************************************************************************
 * @brief Init fader app
 * @param None
 * @return None
 ******************************************************************************/
void Fader_Init()
{
    //init time
    time = TimeOD_TimeFrom_ms(0.0);
}

/******************************************************************************
 * @brief app loop must be called in package loop
 * @param None
 * @return None
 ******************************************************************************/
void Fader_Loop()
{
    msg_t msg;
    color_t rgb;
    //calculate how much time is the led open
    elapsed_ms = Luos_GetSystick() - init_time;
    //find the led id
    int id            = RoutingTB_IDFromType(COLOR_TYPE);
    msg.header.target = id;
    if (time != 0)
    {
        if ((float)elapsed_ms > TimeOD_TimeTo_ms(time))
        {
            // we finished our transition
            // transform the color to a message and send it
            IlluminanceOD_ColorToMsg((color_t *)&target_rgb, &msg);
            Luos_SendMsg(0, &msg);
            // keep the previous value
            memcpy((color_t *)&last_rgb, (color_t *)&target_rgb, sizeof(color_t));
        }
        else
        {
            //calculate the new led color and send it
            rgb.r = (int)(coef[0] * (float)elapsed_ms + (float)last_rgb.r);
            rgb.g = (int)(coef[1] * (float)elapsed_ms + (float)last_rgb.g);
            rgb.b = (int)(coef[2] * (float)elapsed_ms + (float)last_rgb.b);
            IlluminanceOD_ColorToMsg((color_t *)&rgb, &msg);
            Luos_SendMsg(0, &msg);
        }
    }
    else
    {
        //send the full color
        IlluminanceOD_ColorToMsg((color_t *)&target_rgb, &msg);
        Luos_SendMsg(0, &msg);
    }
}
/******************************************************************************
 * @brief store the desired color for the led
 * @param rgb value
 * @return None
 ******************************************************************************/
void Fader_WriteColor(color_t *rgb)
{
    memcpy((color_t *)&last_rgb, (color_t *)&target_rgb, sizeof(color_t));
    //IlluminanceOD_ColorFromMsg((color_t *)&target_rgb, msg);
    target_rgb.r = rgb->r;
    target_rgb.g = rgb->g;
    target_rgb.b = rgb->b;
    if (TimeOD_TimeTo_ms(time) > 0.0)
    {
        elapsed_ms = 0;
        coef[0]    = (float)(target_rgb.r - last_rgb.r) / TimeOD_TimeTo_ms(time);
        coef[1]    = (float)(target_rgb.g - last_rgb.g) / TimeOD_TimeTo_ms(time);
        coef[2]    = (float)(target_rgb.b - last_rgb.b) / TimeOD_TimeTo_ms(time);
    }
    //keep the time we are going to light the led
    init_time = Luos_GetSystick();
}
/******************************************************************************
 * @brief gets the time received
 * @param input_time
 * @return None
 ******************************************************************************/
void Fader_WriteTime(time_luos_t input_time)
{
    time = input_time;
}
