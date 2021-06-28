#ifndef __DATAAPP_JSONB_C_API_H__
#define __DATAAPP_JSONB_C_API_H__
#include "jsonb_c_type.h"
#define BUFMAX (2)
#define double(a) ((a)*(a))
#define sum(a,b) ((a)+(b))
#include "math.h"
extern void jsonb_opt_led_t(jsonb_opt_e opt, cJSON *json, void *element, size_t size);
typedef struct {
    uint32_t change;
    uint32_t redfreq;
    double redduty;
    uint32_t redlight;
    uint32_t yellowfreq;
    double yellowduty;
    uint32_t yellowlight;
} led_t;
extern void jsonb_opt_GwState_t(jsonb_opt_e opt, cJSON *json, void *element, size_t size);
typedef struct {
    double longg;
    double at;
    uint16_t vol;
    uint16_t vision;
    uint32_t devid;
} GwState_t;
extern void jsonb_opt_NodeState_t(jsonb_opt_e opt, cJSON *json, void *element, size_t size);
typedef struct {
    uint32_t devid;
    uint16_t vol;
    uint16_t svol;
    char uid[16];
    char lastupdate[16];
} NodeState_t;
extern void jsonb_opt_Config_t(jsonb_opt_e opt, cJSON *json, void *element, size_t size);
typedef struct {
    char cardetect[8];
    uint32_t taillenth;
    uint32_t alarmtime;
    led_t led;
} Config_t;
extern void jsonb_opt_RfState_t(jsonb_opt_e opt, cJSON *json, void *element, size_t size);
typedef struct {
    uint32_t vision;
    char time[16];
    led_t led;
} RfState_t;
extern void jsonb_opt_Mode_t(jsonb_opt_e opt, cJSON *json, void *element, size_t size);
typedef struct {
    uint8_t mode;
    char rf[16];
} Mode_t;
extern void jsonb_opt_Strategy_t(jsonb_opt_e opt, cJSON *json, void *element, size_t size);
typedef struct {
    char eph[16];
    char online[16];
    char offline[16];
} Strategy_t;
extern void jsonb_opt_Query_t(jsonb_opt_e opt, cJSON *json, void *element, size_t size);
typedef struct {
    char type[16];
} Query_t;
extern void jsonb_opt_Login_t(jsonb_opt_e opt, cJSON *json, void *element, size_t size);
typedef struct {
    char uid[32];
    char software[16];
    char hardware[16];
} Login_t;
extern void jsonb_opt_Rep_t(jsonb_opt_e opt, cJSON *json, void *element, size_t size);
typedef struct {
    char ack[8];
} Rep_t;
extern void jsonb_opt_SysControl_t(jsonb_opt_e opt, cJSON *json, void *element, size_t size);
typedef struct {
    char type[8];
} SysControl_t;
extern void jsonb_opt_GC_msg_t(jsonb_opt_e opt, cJSON *json, void *element, size_t size);
typedef struct {
    char cls[32];
    uint32_t sn;
    uint64_t ts;
    uint32_t id;
    char rpt[32];
    union {
    GwState_t GwState;
    NodeState_t NodeState;
    Config_t Config;
    RfState_t RfState;
    Mode_t Mode;
    Strategy_t Strategy;
    Query_t Query;
    Login_t Login;
    Rep_t Ack;
    SysControl_t Control;
    };
} GC_msg_t;
#endif /* __DATAAPP_JSONB_C_API_H__ */
