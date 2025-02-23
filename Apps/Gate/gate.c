/******************************************************************************
 * @file gate
 * @brief Service gate
 * @author Luos
 ******************************************************************************/
#include <stdio.h>
#include <stdbool.h>
#include "gate_config.h"
#include "gate.h"
#include "data_manager.h"
#include "convert.h"
#include "pipe_link.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
container_t *gate;
char detection_ask      = 0;
time_luos_t update_time = GATE_REFRESH_TIME_S;
/*******************************************************************************
 * Function
 ******************************************************************************/
/******************************************************************************
 * @brief init must be call in project init
 * @param None
 * @return None
 ******************************************************************************/
void Gate_Init(void)
{
    revision_t revision = {.major = 1, .minor = 0, .build = 0};
    gate                = Luos_CreateContainer(0, GATE_MOD, "gate", revision);
}

/******************************************************************************
 * @brief loop must be call in project loop
 * @param None
 * @return None
 ******************************************************************************/
void Gate_Loop(void)
{
#ifndef GATE_POLLING
    static uint8_t first_conversion = 1;
#endif
    static short pipe_id              = 0;
    static short previous_id          = -1;
    static volatile bool gate_running = false;
    static uint32_t last_time         = 0;

    // Check the detection status.
    if (RoutingTB_IDFromContainer(gate) == 0)
    {
        // We don't have any ID, meaning no detection occure or detection is occuring.
        if (previous_id == -1)
        {
            // This is the start period, we have to make a detection.
            // Be sure the network is powered up 20 ms before starting a detection
#ifndef NODETECTION
            if (Luos_GetSystick() > 20)
            {
                // No detection occure, do it
                RoutingTB_DetectContainers(gate);
#ifndef GATE_POLLING
                first_conversion = 1;
                update_time      = TimeOD_TimeFrom_s(GATE_REFRESH_TIME_S);
#endif
            }
#endif
        }
        else
        {
            // someone is making a detection, let it finish.
            // reset the previous_id state to be ready to setup container at the end of detection
            previous_id = 0;
        }
        pipe_id = 0;
    }
    else
    {
        // Network have been detected, We are good to go
        if (pipe_id == 0)
        {
            // We dont have spotted any pipe yet. Try to find one
            pipe_id = PipeLink_Find(gate);
        }
        if (gate_running && !detection_ask)
        {
            // Manage input and output data
            if ((Luos_GetSystick() - last_time >= TimeOD_TimeTo_ms(update_time)) && (Luos_GetSystick() > last_time))
            {
                last_time = Luos_GetSystick();
                DataManager_Run(gate);
#ifndef GATE_POLLING
                if (first_conversion == 1)
                {
                    // This is the first time we perform a convertion
                    // Evaluate the time needed to convert all the data of this configuration and update refresh rate
                    uint32_t execution_time = ((Luos_GetSystick() - last_time) * 2) + 1;
                    update_time             = TimeOD_TimeFrom_ms(execution_time);
                    // Update refresh rate for all services of the network
                    DataManager_collect(gate);
                    first_conversion = 0;
                }
#endif
            }
        }
        else
        {
            DataManager_RunPipeOnly(gate);
        }
        if (detection_ask)
        {
            // Run detection
            RoutingTB_DetectContainers(gate);
            pipe_id = PipeLink_Find(gate);
            // Create data from container
            Convert_RoutingTableData(gate);
#ifndef GATE_POLLING
            // Set update frequency
            update_time = TimeOD_TimeFrom_s(GATE_REFRESH_TIME_S);
            DataManager_collect(gate);
            last_time        = Luos_GetSystick() + (uint32_t)(TimeOD_TimeTo_ms(GATE_REFRESH_TIME_S) / 2);
            first_conversion = 1;
#endif
            gate_running  = true;
            detection_ask = 0;
        }
    }
}
