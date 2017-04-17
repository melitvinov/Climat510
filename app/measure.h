#ifndef _MEASURE_H_
#define _MEASURE_H_

typedef struct __packed
{
    u8 type;
    u8 input;
    u8 output;
    u8 corr;
    u16 u0;
    u16 v0;
    u16 u1;
    u16 v1;
} sensor_config_t;

// blobs in remote reporting
typedef struct __packed
{
    sensor_config_t  IndoorSensors[NZONES][cConfSSens];
    sensor_config_t  MeteoSensors[cConfSMetSens];
} calibration_t;

typedef struct
{
    int16_t         uIndoorSensors[NZONES][cConfSSens];
    int16_t         uMeteoSens[cConfSMetSens];
} sens_t;

extern calibration_t caldata;
extern sens_t sensdata;

void reset_calibration(void);

void Measure(void);
void UpdateInputConfigs(void);

#endif
