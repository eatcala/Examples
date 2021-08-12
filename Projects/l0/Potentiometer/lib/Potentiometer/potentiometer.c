/******************************************************************************
 * @file potentiometer
 * @brief driver example a simple potentiometer
 * @author Luos
 * @version 0.0.0
 ******************************************************************************/
#include "main.h"
#include "potentiometer.h"
#include "analog.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
typedef struct driver_angle
{
    void (*init)(void);
    uint8_t (*read)(angular_position_t *);
    uint8_t (*write)(angular_position_t *);
    uint8_t (*configure)(void *);
} driver_angle_t;
/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile angular_position_t angle = 0.0;

driver_angle_t potentiometer_drv = {
    .init      = PotentiometerDrv_Init,
    .write     = 0,
    .read      = PotentiometerDrv_Read,
    .configure = 0,
};

/*******************************************************************************
 * Function
 ******************************************************************************/
static void Potentiometer_MsgHandler(service_t *service, msg_t *msg);

/******************************************************************************
 * @brief init must be call in project init
 * @param None
 * @return None
 ******************************************************************************/
void Potentiometer_Init(void)
{
    revision_t revision = {.unmap = REV};
    //driver init
    potentiometer_drv.init();
    // ******************* service creation *******************
    Luos_CreateService(Potentiometer_MsgHandler, ANGLE_TYPE, "potentiometer", revision);
}
/******************************************************************************
 * @brief loop must be call in project loop
 * @param None
 * @return None
 ******************************************************************************/
void Potentiometer_Loop(void)
{
    potentiometer_drv.read(&angle);
}
/******************************************************************************
 * @brief Msg Handler call back when a msg receive for this service
 * @param Service destination
 * @param Msg receive
 * @return None
 ******************************************************************************/
static void Potentiometer_MsgHandler(service_t *service, msg_t *msg)
{
    if (msg->header.cmd == GET_CMD)
    {
        msg_t pub_msg;
        // fill the message infos
        pub_msg.header.target_mode = ID;
        pub_msg.header.target      = msg->header.source;
        AngularOD_PositionToMsg((angular_position_t *)&angle, &pub_msg);
        Luos_SendMsg(service, &pub_msg);
        return;
    }
}
