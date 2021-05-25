#include <stdio.h>
#include <stdbool.h>
#include "json_mnger.h"
#include "cmd.h"
#include "convert.h"
#include "gate.h"

static unsigned int delayms = 1;

//******************* sensor update ****************************
// This function will gather data from sensors and create a json string for you
void collect_data(service_t *service)
{
    msg_t json_msg;
    json_msg.header.target_mode = ID;
    json_msg.header.cmd         = ASK_PUB_CMD;
    json_msg.header.size        = 0;
    // ask services to publish datas
    for (uint8_t i = 1; i <= RoutingTB_GetLastService(); i++)
    {
        // Check if this service is a sensor
        if ((RoutingTB_ServiceIsSensor(RoutingTB_TypeFromID(i))) || (RoutingTB_TypeFromID(i) >= LUOS_LAST_TYPE))
        {
            // This service is a sensor so create a msg and send it
            json_msg.header.target = i;
            Luos_SendMsg(service, &json_msg);
        }
    }
}

// This function will create a json string for services datas
void format_data(service_t *service, char *json)
{
    msg_t *json_msg = 0;
    uint8_t json_ok = false;
    if ((Luos_NbrAvailableMsg() > 0))
    {
        // Init the json string
        sprintf(json, "{\"services\":{");
        // loop into services.
        uint16_t i = 1;
        // get the oldest message
        while (Luos_ReadMsg(service, &json_msg) == SUCCEED)
        {
            // check if this is an assert
            if (json_msg->header.cmd == ASSERT)
            {
                char error_json[256] = "\0";
                luos_assert_t assertion;
                memcpy(assertion.unmap, json_msg->data, json_msg->header.size);
                assertion.unmap[json_msg->header.size] = '\0';
                sprintf(error_json, "{\"assert\":{\"node_id\":%d,\"file\":\"%s\",\"line\":%d}}\n", json_msg->header.source, assertion.file, (unsigned int)assertion.line);
                json_send(error_json);
                continue;
            }
            // get the source of this message
            i = json_msg->header.source;
            // Create service description
            char *alias;
            alias = RoutingTB_AliasFromId(i);
            if (alias != 0)
            {
                json_ok = true;
                sprintf(json, "%s\"%s\":{", json, alias);
                // now add json data from service
                msg_to_json(json_msg, &json[strlen(json)]);
                // Check if we receive other messages from this service
                while (Luos_ReadFromService(service, i, &json_msg) == SUCCEED)
                {
                    // we receive some, add it to the Json
                    msg_to_json(json_msg, &json[strlen(json)]);
                }
                if (json[strlen(json) - 1] != '{')
                {
                    // remove the last "," char
                    json[strlen(json) - 1] = '\0';
                }
                // End the service section
                sprintf(json, "%s},", json);
            }
        }
        if (json_ok)
        {
            // remove the last "," char
            json[strlen(json) - 1] = '\0';
            // End the Json message
            sprintf(json, "%s}}\n", json);
        }
        else
        {
            //create a void string
            *json = '\0';
        }
    }
    else
    {
        //create a void string
        *json = '\0';
    }
}

unsigned int get_delay(void)
{
    return delayms;
}

void set_delay(unsigned int new_delayms)
{
    delayms = new_delayms;
}
