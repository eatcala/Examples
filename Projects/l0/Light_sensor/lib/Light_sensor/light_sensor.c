/******************************************************************************
 * @file light sensor
 * @brief driver example a simple light sensor
 * @author Luos
 * @version 0.0.0
 ******************************************************************************/
#include "main.h"
#include "light_sensor.h"
#include "string.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

typedef struct driver_light
{
    void (*init)(void);
    uint8_t (*read)(illuminance_t *);
    uint8_t (*write)(illuminance_t *);
    uint8_t (*configure)(void *);
} driver_light_t;
/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile illuminance_t lux = 0.0;

driver_light_t light_sensor_drv = {
    .init      = LightSensorDrv_Init,
    .read      = LightSensorDrv_Read,
    .write     = 0,
    .configure = 0,
};
/*******************************************************************************
 * Function
 ******************************************************************************/
static void LightSensor_MsgHandler(service_t *service, msg_t *msg);

/******************************************************************************
 * @brief init must be call in project init
 * @param None
 * @return None
 ******************************************************************************/
void LightSensor_Init(void)
{
    revision_t revision = {.unmap = REV};
    // **************** communication driver init *************
    light_sensor_drv.init();
    // ******************* service creation *******************
    Luos_CreateService(LightSensor_MsgHandler, LIGHT_TYPE, "light_sensor", revision);
}
/******************************************************************************
 * @brief loop must be call in project loop
 * @param None
 * @return None
 ******************************************************************************/
void LightSensor_Loop(void)
{
    light_sensor_drv.read(&lux);
}
/******************************************************************************
 * @brief Msg Handler call back when a msg receive for this service
 * @param Service destination
 * @param Msg receive
 * @return None
 ******************************************************************************/
static void LightSensor_MsgHandler(service_t *service, msg_t *msg)
{
    if (msg->header.cmd == GET_CMD)
    {
        msg_t pub_msg;
        // fill the message infos
        pub_msg.header.target_mode = ID;
        pub_msg.header.target      = msg->header.source;
        IlluminanceOD_IlluminanceToMsg((illuminance_t *)&lux, &pub_msg);
        Luos_SendMsg(service, &pub_msg);
        return;
    }
}
