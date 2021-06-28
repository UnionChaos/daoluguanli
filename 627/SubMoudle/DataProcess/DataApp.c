#include "jsonb_c_type.h"
#include "jsonb_c_base.h"
#include "DataApp.h"
void jsonb_opt_led_t(jsonb_opt_e opt, cJSON *json, void *e, size_t size)
{
    led_t *element= e;
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "change");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "change", json_child);
    }
    jsonb_opt_uint32_t(opt, json_child, &element->change, sizeof(uint32_t));
}
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "redfreq");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "redfreq", json_child);
    }
    jsonb_opt_uint32_t(opt, json_child, &element->redfreq, sizeof(uint32_t));
}
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "redduty");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "redduty", json_child);
    }
    jsonb_opt_double(opt, json_child, &element->redduty, sizeof(double));
}
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "redlight");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "redlight", json_child);
    }
    jsonb_opt_uint32_t(opt, json_child, &element->redlight, sizeof(uint32_t));
}
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "yellowfreq");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "yellowfreq", json_child);
    }
    jsonb_opt_uint32_t(opt, json_child, &element->yellowfreq, sizeof(uint32_t));
}
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "yellowduty");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "yellowduty", json_child);
    }
    jsonb_opt_double(opt, json_child, &element->yellowduty, sizeof(double));
}
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "yellowlight");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "yellowlight", json_child);
    }
    jsonb_opt_uint32_t(opt, json_child, &element->yellowlight, sizeof(uint32_t));
}
}
void jsonb_opt_GwState_t(jsonb_opt_e opt, cJSON *json, void *e, size_t size)
{
    GwState_t *element= e;
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "longg");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "longg", json_child);
    }
    jsonb_opt_double(opt, json_child, &element->longg, sizeof(double));
}
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "at");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "at", json_child);
    }
    jsonb_opt_double(opt, json_child, &element->at, sizeof(double));
}
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "vol");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "vol", json_child);
    }
    jsonb_opt_uint16_t(opt, json_child, &element->vol, sizeof(uint16_t));
}
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "vision");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "vision", json_child);
    }
    jsonb_opt_uint16_t(opt, json_child, &element->vision, sizeof(uint16_t));
}
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "devid");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "devid", json_child);
    }
    jsonb_opt_uint32_t(opt, json_child, &element->devid, sizeof(uint32_t));
}
}
void jsonb_opt_NodeState_t(jsonb_opt_e opt, cJSON *json, void *e, size_t size)
{
    NodeState_t *element= e;
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "devid");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "devid", json_child);
    }
    jsonb_opt_uint32_t(opt, json_child, &element->devid, sizeof(uint32_t));
}
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "vol");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "vol", json_child);
    }
    jsonb_opt_uint16_t(opt, json_child, &element->vol, sizeof(uint16_t));
}
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "svol");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "svol", json_child);
    }
    jsonb_opt_uint16_t(opt, json_child, &element->svol, sizeof(uint16_t));
}
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "uid");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "uid", json_child);
    }
    jsonb_opt_string(opt, json_child, element->uid, 16);
}
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "lastupdate");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "lastupdate", json_child);
    }
    jsonb_opt_string(opt, json_child, element->lastupdate, 16);
}
}
void jsonb_opt_Config_t(jsonb_opt_e opt, cJSON *json, void *e, size_t size)
{
    Config_t *element= e;
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "cardetect");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "cardetect", json_child);
    }
    jsonb_opt_string(opt, json_child, element->cardetect, 8);
}
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "taillenth");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "taillenth", json_child);
    }
    jsonb_opt_uint32_t(opt, json_child, &element->taillenth, sizeof(uint32_t));
}
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "alarmtime");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "alarmtime", json_child);
    }
    jsonb_opt_uint32_t(opt, json_child, &element->alarmtime, sizeof(uint32_t));
}
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "led");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "led", json_child);
    }
    jsonb_opt_led_t(opt, json_child, &element->led, sizeof(led_t));
}
}
void jsonb_opt_RfState_t(jsonb_opt_e opt, cJSON *json, void *e, size_t size)
{
    RfState_t *element= e;
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "vision");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "vision", json_child);
    }
    jsonb_opt_uint32_t(opt, json_child, &element->vision, sizeof(uint32_t));
}
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "time");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "time", json_child);
    }
    jsonb_opt_string(opt, json_child, element->time, 16);
}
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "led");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "led", json_child);
    }
    jsonb_opt_led_t(opt, json_child, &element->led, sizeof(led_t));
}
}
void jsonb_opt_Mode_t(jsonb_opt_e opt, cJSON *json, void *e, size_t size)
{
    Mode_t *element= e;
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "mode");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "mode", json_child);
    }
    jsonb_opt_uint8_t(opt, json_child, &element->mode, sizeof(uint8_t));
}
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "rf");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "rf", json_child);
    }
    jsonb_opt_string(opt, json_child, element->rf, 16);
}
}
void jsonb_opt_Strategy_t(jsonb_opt_e opt, cJSON *json, void *e, size_t size)
{
    Strategy_t *element= e;
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "eph");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "eph", json_child);
    }
    jsonb_opt_string(opt, json_child, element->eph, 16);
}
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "online");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "online", json_child);
    }
    jsonb_opt_string(opt, json_child, element->online, 16);
}
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "offline");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "offline", json_child);
    }
    jsonb_opt_string(opt, json_child, element->offline, 16);
}
}
void jsonb_opt_Query_t(jsonb_opt_e opt, cJSON *json, void *e, size_t size)
{
    Query_t *element= e;
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "type");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "type", json_child);
    }
    jsonb_opt_string(opt, json_child, element->type, 16);
}
}
void jsonb_opt_Login_t(jsonb_opt_e opt, cJSON *json, void *e, size_t size)
{
    Login_t *element= e;
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "uid");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "uid", json_child);
    }
    jsonb_opt_string(opt, json_child, element->uid, 32);
}
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "software");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "software", json_child);
    }
    jsonb_opt_string(opt, json_child, element->software, 16);
}
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "hardware");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "hardware", json_child);
    }
    jsonb_opt_string(opt, json_child, element->hardware, 16);
}
}
void jsonb_opt_Rep_t(jsonb_opt_e opt, cJSON *json, void *e, size_t size)
{
    Rep_t *element= e;
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "ack");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "ack", json_child);
    }
    jsonb_opt_string(opt, json_child, element->ack, 8);
}
}
void jsonb_opt_SysControl_t(jsonb_opt_e opt, cJSON *json, void *e, size_t size)
{
    SysControl_t *element= e;
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "type");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "type", json_child);
    }
    jsonb_opt_string(opt, json_child, element->type, 8);
}
}
void jsonb_opt_GC_msg_t(jsonb_opt_e opt, cJSON *json, void *e, size_t size)
{
    GC_msg_t *element= e;
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "cls");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "cls", json_child);
    }
    jsonb_opt_string(opt, json_child, element->cls, 32);
}
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "sn");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "sn", json_child);
    }
    jsonb_opt_uint32_t(opt, json_child, &element->sn, sizeof(uint32_t));
}
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "ts");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "ts", json_child);
    }
    jsonb_opt_uint64_t(opt, json_child, &element->ts, sizeof(uint64_t));
}
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "id");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "id", json_child);
    }
    jsonb_opt_uint32_t(opt, json_child, &element->id, sizeof(uint32_t));
}
{
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
        json_child = cJSON_GetObjectItem(json, "rpt");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "rpt", json_child);
    }
    jsonb_opt_string(opt, json_child, element->rpt, 32);
}
if (!strncmp("GwState", element->rpt, sizeof("GwState")))
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
            json_child = cJSON_GetObjectItem(json, "data");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "data", json_child);
    }
    jsonb_opt_GwState_t(opt, json_child, &element->GwState, sizeof(GwState_t));
}
if (!strncmp("NodeState", element->rpt, sizeof("NodeState")))
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
            json_child = cJSON_GetObjectItem(json, "data");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "data", json_child);
    }
    jsonb_opt_NodeState_t(opt, json_child, &element->NodeState, sizeof(NodeState_t));
}
if (!strncmp("Config", element->rpt, sizeof("Config")))
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
            json_child = cJSON_GetObjectItem(json, "data");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "data", json_child);
    }
    jsonb_opt_Config_t(opt, json_child, &element->Config, sizeof(Config_t));
}
if (!strncmp("RfState", element->rpt, sizeof("RfState")))
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
            json_child = cJSON_GetObjectItem(json, "data");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "data", json_child);
    }
    jsonb_opt_RfState_t(opt, json_child, &element->RfState, sizeof(RfState_t));
}
if (!strncmp("Mode", element->rpt, sizeof("Mode")))
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
            json_child = cJSON_GetObjectItem(json, "data");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "data", json_child);
    }
    jsonb_opt_Mode_t(opt, json_child, &element->Mode, sizeof(Mode_t));
}
if (!strncmp("Strategy", element->rpt, sizeof("Strategy")))
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
            json_child = cJSON_GetObjectItem(json, "data");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "data", json_child);
    }
    jsonb_opt_Strategy_t(opt, json_child, &element->Strategy, sizeof(Strategy_t));
}
if (!strncmp("Query", element->rpt, sizeof("Query")))
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
            json_child = cJSON_GetObjectItem(json, "data");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "data", json_child);
    }
    jsonb_opt_Query_t(opt, json_child, &element->Query, sizeof(Query_t));
}
if (!strncmp("Login", element->rpt, sizeof("Login")))
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
            json_child = cJSON_GetObjectItem(json, "data");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "data", json_child);
    }
    jsonb_opt_Login_t(opt, json_child, &element->Login, sizeof(Login_t));
}
if (!strncmp("Ack", element->rpt, sizeof("Ack")))
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
            json_child = cJSON_GetObjectItem(json, "data");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "data", json_child);
    }
    jsonb_opt_Rep_t(opt, json_child, &element->Ack, sizeof(Rep_t));
}
if (!strncmp("Control", element->rpt, sizeof("Control")))
{
    cJSON *json_child = NULL;
    if (opt == JSONB_OPT_J2S) {
        if (cJSON_IsNull(json)) return;
            json_child = cJSON_GetObjectItem(json, "data");
    } else if (opt == JSONB_OPT_S2J) {
        json_child = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "data", json_child);
    }
    jsonb_opt_SysControl_t(opt, json_child, &element->Control, sizeof(SysControl_t));
}
}
}
