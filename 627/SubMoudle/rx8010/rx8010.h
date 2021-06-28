#ifndef __RX8010_H__
#define __RX8010_H__

#ifdef __cplusplus 
extern "C" {
#endif

#include "stdint.h"

typedef struct _rx8010__time_t
{
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t day;
    uint8_t week;
    uint8_t month;
    uint16_t year;

}rx8010_time_t;
void rx8010_Init(void);
void rx8010_get_time(rx8010_time_t * time);
void rx8010_set_time(rx8010_time_t * time);
uint8_t      rx8010_IsErr(void);

#ifdef __cplusplus
}
#endif

#endif /* __RX8025_H__ */