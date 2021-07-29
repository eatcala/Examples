/******************************************************************************
 * @file Power switch
 * @brief driver example a simple Power switch
 * @author Luos
 * @version 0.0.0
 ******************************************************************************/
#include "main.h"
#include "power_switch.h"
#include "profile_state.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
profile_core_t power_switch_profile;
profile_state_t power_switch;
/*******************************************************************************
 * Function
 ******************************************************************************/

/******************************************************************************
 * @brief init must be call in project init
 * @param None
 * @return None
 ******************************************************************************/
void PowerSwitch_Init(void)
{
    revision_t revision = {.major = 1, .minor = 0, .build = 0};
    // Profile configuration
    power_switch.access = WRITE_ONLY_ACCESS;

    Luos_LinkStateProfile(&power_switch_profile, &power_switch, 0);
    Luos_LaunchProfile(&power_switch_profile, "power_switch", revision);
}
/******************************************************************************
 * @brief loop must be call in project loop
 * @param None
 * @return None
 ******************************************************************************/
void PowerSwitch_Loop(void)
{
    HAL_GPIO_WritePin(GPIOA, SWITCH_Pin, power_switch.state);
}
