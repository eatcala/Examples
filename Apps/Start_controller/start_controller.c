/******************************************************************************
 * @file start controller
 * @brief application example a start controller
 * @author Luos
 * @version 0.0.0
 ******************************************************************************/
#include "start_controller.h"
#include "main.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define LIGHT_INTENSITY 255

#define UPDATE_PERIOD_MS 10

typedef enum
{
    ALARM_CONTROLLER_APP = LUOS_LAST_TYPE,
    START_CONTROLLER_APP
} alarm_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/
container_t *app;
volatile control_t control_app;
uint8_t lock           = 1;
uint8_t last_btn_state = 0;
uint8_t state_switch   = 0;
uint8_t init           = 0;

/*******************************************************************************
 * Function
 ******************************************************************************/
static void StartController_MsgHandler(container_t *container, msg_t *msg);

/******************************************************************************
 * @brief init must be call in project init
 * @param None
 * @return None
 ******************************************************************************/
void StartController_Init(void)
{
    revision_t revision = {.major = 0, .minor = 1, .build = 1};
    // By default this app running
    control_app.flux = PLAY;
    // Create App
    app = Luos_CreateContainer(StartController_MsgHandler, START_CONTROLLER_APP, "start_control", revision);
}
/******************************************************************************
 * @brief loop must be call in project loop
 * @param None
 * @return None
 ******************************************************************************/
void StartController_Loop(void)
{
    static short previous_id       = -1;
    static uint32_t switch_date    = 0;
    static uint8_t animation_state = 0;
    // ********** hot plug management ************
    // Check if we have done the first init or if container Id have changed
    if (previous_id != RoutingTB_IDFromContainer(app))
    {
        if (RoutingTB_IDFromContainer(app) == 0)
        {
            // We don't have any ID, meaning no detection occure or detection is occuring.
            if (previous_id == -1)
            {
                // This is the really first init, we have to make it.
                // Be sure the network is powered up 1000 ms before starting a detection
                if (HAL_GetTick() > 1000)
                {
                    // No detection occure, do it
                    RoutingTB_DetectContainers(app);
                }
            }
            else
            {
                // someone is making a detection, let it finish.
                // reset the init state to be ready to setup container at the end of detection
                previous_id = 0;
            }
        }
        else
        {
            // Make containers configurations
            int id = RoutingTB_IDFromAlias("lock");
            if (id > 0)
            {
                msg_t msg;
                msg.header.target      = id;
                msg.header.target_mode = IDACK;
                // Setup auto update each UPDATE_PERIOD_MS on button
                // This value is resetted on all container at each detection
                // It's important to setting it each time.
                time_luos_t time = TimeOD_TimeFrom_ms(UPDATE_PERIOD_MS);
                TimeOD_TimeToMsg(&time, &msg);
                msg.header.cmd = UPDATE_PUB;
                Luos_SendMsg(app, &msg);
            }
            previous_id = RoutingTB_IDFromContainer(app);
        }
        return;
    }
    // ********** non blocking button management ************
    if (state_switch & (control_app.flux == PLAY) & (animation_state == 0))
    {
        msg_t msg;
        msg.header.target_mode = IDACK;
        // Share the lock state with the alarm_control app
        int id = RoutingTB_IDFromAlias("alarm_control");
        if (id > 0)
        {
            // we have an alarm_controller App control it
            control_t alarm_control;
            if (lock)
            {
                // Bike is locked, alarm need to run.
                alarm_control.flux = PLAY;
            }
            else
            {
                // Bike is unlocked alarm should be sutted down.
                alarm_control.flux = STOP;
            }
            // send message
            ControlOD_ControlToMsg(&alarm_control, &msg);
            Luos_SendMsg(app, &msg);
        }
        // The button state switch, change the led consequently
        state_switch = 0;
        id           = RoutingTB_IDFromType(COLOR_MOD);
        if (id > 0)
        {
            // we have an alarm, we can set its color
            color_t color;
            color.r = 0;
            color.g = 0;
            color.b = 0;
            if (!lock)
            {
                color.g = LIGHT_INTENSITY;
            }
            else
            {
                color.b = LIGHT_INTENSITY;
            }
            msg.header.target = id;
            IlluminanceOD_ColorToMsg(&color, &msg);
            Luos_SendMsg(app, &msg);
        }
        id = RoutingTB_IDFromAlias("horn");
        if (id > 0)
        {
            // we get a horn
            msg.header.target = id;
            msg.header.size   = sizeof(uint8_t);
            msg.header.cmd    = IO_STATE;
            // turn the horn on/off
            msg.data[0] = 1;
            Luos_SendMsg(app, &msg);
        }
        // try to reach a buzzer and drive it to make a happy sound
        if (!lock)
        {
            id = RoutingTB_IDFromAlias("buzzer_mod");
            if (id > 0)
            {
                msg.header.target = id;
                msg.header.cmd    = IO_STATE;
                msg.header.size   = 1;
                msg.data[0]       = 1;
                Luos_SendMsg(app, &msg);
            }
        }
        // Save switch date
        switch_date = HAL_GetTick();
        animation_state++;
    }
    // This part is a start stop animation using available containers
    if (((HAL_GetTick() - switch_date) > 100) & (animation_state == 1))
    {
        // 100ms after button turn of light and horn
        msg_t msg;
        msg.header.target_mode = IDACK;
        int id                 = RoutingTB_IDFromAlias("horn");
        if (id > 0)
        {
            // we get a horn
            msg.header.target = id;
            msg.header.size   = sizeof(uint8_t);
            msg.header.cmd    = IO_STATE;
            // turn the horn on/off
            msg.data[0] = 0;
            Luos_SendMsg(app, &msg);
        }
        animation_state++;
    }
    if (((HAL_GetTick() - switch_date) > 600) & (animation_state == 2))
    {
        // 600ms after switch turn light depending on the curent lock state
        msg_t msg;
        msg.header.target_mode = IDACK;
        int id                 = RoutingTB_IDFromType(COLOR_MOD);
        if (id > 0)
        {
            // we have an alarm, we can set its color
            color_t color;
            if (lock)
            {
                color.r = 0;
                color.g = 0;
                color.b = 0;
            }
            else
            {
                color.r = LIGHT_INTENSITY;
                color.g = LIGHT_INTENSITY;
                color.b = LIGHT_INTENSITY;
            }
            msg.header.target = id;
            IlluminanceOD_ColorToMsg(&color, &msg);
            Luos_SendMsg(app, &msg);
        }
        animation_state = 0;
    }
}
/******************************************************************************
 * @brief Msg Handler call back when a msg receive for this container
 * @param Container destination
 * @param Msg receive
 * @return None
 ******************************************************************************/
static void StartController_MsgHandler(container_t *container, msg_t *msg)
{
    if (msg->header.cmd == IO_STATE)
    {
        if (control_app.flux == PLAY)
        {
            if (RoutingTB_TypeFromID(msg->header.source) == STATE_MOD)
            {
                // this is the button reply we have filter it to manage monostability
                if ((!last_btn_state) & (last_btn_state != msg->data[0]))
                {
                    lock = (!lock);
                    state_switch++;
                }
            }
            else
            {
                // this is an already filtered information
                if ((lock != msg->data[0]))
                {
                    lock = msg->data[0];
                    state_switch++;
                }
            }
            last_btn_state = msg->data[0];
        }
        return;
    }
    if (msg->header.cmd == CONTROL)
    {
        control_app.unmap = msg->data[0];
        return;
    }
}
