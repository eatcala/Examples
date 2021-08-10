/******************************************************************************
 * @file gpio_dev
 * @brief driver example a simple gpio_dev
 * @author Luos
 * @version 0.0.0
 ******************************************************************************/
#include "gpio_dev.h"
#include "ll_gpio.h"

#include "profile_state.h"
#include "profile_voltage.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
typedef struct driver
{
    void (*init)(void);
    uint8_t (*read)(void *);
    uint8_t (*write)(void *);
} driver_t;

driver_t gpio_drv = {
    .init  = ll_digital_init,
    .read  = ll_digital_read,
    .write = ll_digital_write,
};

driver_t analog_drv = {
    .init  = ll_analog_init,
    .read  = ll_analog_read,
    .write = 0,
};

/*******************************************************************************
 * Variables
 ******************************************************************************/
profile_core_t gpio_profile[GPIO_NB];
profile_state_t gpio[GPIO_NB];

profile_core_t analog_profile[ANALOG_NB];
profile_voltage_t analog[ANALOG_NB];

/*******************************************************************************
 * Function
 ******************************************************************************/
static inline void analog_service_init(void);
static inline void digital_service_init(void);
static void GpioDev_callback(service_t *service, msg_t *msg);

/******************************************************************************
 * @brief init must be call in project init
 * @param None
 * @return None
 ******************************************************************************/
void GpioDev_Init(void)
{
    // hardware initialization
    gpio_drv.init();
    analog_drv.init();

    // services creation
    analog_service_init();
    digital_service_init();
}

/******************************************************************************
 * @brief loop must be call in project loop
 * @param None
 * @return None
 ******************************************************************************/
void GpioDev_Loop(void)
{
    // read values from GPIO
    gpio_drv.read(gpio);
    // read values from ADC
    analog_drv.read(analog);
}

/******************************************************************************
 * @brief routine called by luos
 * @param None
 * @return None
 ******************************************************************************/
static void GpioDev_callback(service_t *service, msg_t *msg)
{
    if (msg->header.cmd == IO_STATE)
    {
        // write values on digital port
        gpio_drv.write(gpio);
    }
}

/******************************************************************************
 * @brief Analog services initialization routine
 * @param None
 * @return None
 ******************************************************************************/
void analog_service_init(void)
{
    revision_t revision = {.major = 1, .minor = 0, .build = 0};
    // Link user variables to core profile.
    for (uint8_t i = 0; i < ANALOG_NB; i++)
    {
        Luos_LinkVoltageProfile(&analog_profile[i], &analog[i], 0);
    }
    // Profile configuration
    analog[P1].access = READ_ONLY_ACCESS;
    analog[P7].access = READ_ONLY_ACCESS;
    analog[P8].access = READ_ONLY_ACCESS;
    analog[P9].access = READ_ONLY_ACCESS;
    // Service creation following profile
    Luos_LaunchProfile(&analog_profile[P1], "analog_read_P1", revision);
    Luos_LaunchProfile(&analog_profile[P7], "analog_read_P7", revision);
    Luos_LaunchProfile(&analog_profile[P8], "analog_read_P8", revision);
    Luos_LaunchProfile(&analog_profile[P9], "analog_read_P9", revision);
}

/******************************************************************************
 * @brief Digital services initialization routine
 * @param None
 * @return None
 ******************************************************************************/
void digital_service_init(void)
{
    revision_t revision = {.major = 1, .minor = 0, .build = 0};
    // Link user variables to core profile.
    for (uint8_t i = 0; i < GPIO_NB; i++)
    {
        if (i <= P4)
        {
            // output pins uses callback to write data
            Luos_LinkStateProfile(&gpio_profile[i], &gpio[i], GpioDev_callback);
        }
        else
        {
            // input pins doesn't use callbacks
            Luos_LinkStateProfile(&gpio_profile[i], &gpio[i], 0);
        }
    }
    // Input profile configuration
    gpio[P5].access = READ_ONLY_ACCESS;
    gpio[P6].access = READ_ONLY_ACCESS;
    // Service creation following profile
    Luos_LaunchProfile(&gpio_profile[P5], "digit_read_P5", revision);
    Luos_LaunchProfile(&gpio_profile[P6], "digit_read_P6", revision);

    // Output profile configuration
    gpio[P2].access = WRITE_ONLY_ACCESS;
    gpio[P3].access = WRITE_ONLY_ACCESS;
    gpio[P4].access = WRITE_ONLY_ACCESS;
    // Service creation following profile, for this one we one to use a target evnet using callback
    Luos_LaunchProfile(&gpio_profile[P2], "digit_write_P2", revision);
    Luos_LaunchProfile(&gpio_profile[P3], "digit_write_P3", revision);
    Luos_LaunchProfile(&gpio_profile[P4], "digit_write_P4", revision);
}