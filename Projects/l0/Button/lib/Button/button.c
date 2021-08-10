/******************************************************************************
 * @file button
 * @brief driver example a simple button
 * @author Luos
 * @version 0.0.0
 ******************************************************************************/

#include "button.h"
#include "ll_button.h"
#include "profile_state.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

typedef struct driver
{
    void (*init)(void);
    uint8_t (*read)(profile_state_t *);
    uint8_t (*write)(profile_state_t *);
} driver_t;

driver_t button_drv = {
    .init  = ll_button_init,
    .read  = ll_button_read,
    .write = 0,
};

/*******************************************************************************
 * Variables
 ******************************************************************************/
profile_core_t button_handler;
profile_state_t button;
/*******************************************************************************
 * Function
 ******************************************************************************/
static inline void button_service_init(void);

/******************************************************************************
 * @brief init must be call in project init
 * @param None
 * @return None
 ******************************************************************************/
void Button_Init(void)
{
    // low level initialization
    button_drv.init();
    // service initialization
    button_service_init();
}

/******************************************************************************
 * @brief loop must be call in project loop
 * @param None
 * @return None
 ******************************************************************************/
void Button_Loop(void)
{
    button_drv.read(&button);
}

/******************************************************************************
 * @brief init must be call in project init
 * @param None
 * @return None
 ******************************************************************************/
void button_service_init(void)
{
    // service initialization
    revision_t revision = {.major = 1, .minor = 0, .build = 0};
    // Profile configuration
    button.access = READ_ONLY_ACCESS;
    // Service creation following state profile
    Luos_LinkStateProfile(&button_handler, &button, 0);
    Luos_LaunchProfile(&button_handler, "button", revision);
}