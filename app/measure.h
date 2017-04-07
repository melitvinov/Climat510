#ifndef _MEASURE_H_
#define _MEASURE_H_

// blobs in remote reporting
typedef struct __packed
{
    module_input_cfg_t  IndoorSensors[NZONES][cConfSSens];
    module_input_cfg_t  MeteoSensors[cConfSMetSens];
} calibration_t;

typedef struct
{
    int16_t         uInTeplSens[NZONES][cConfSSens];
    int16_t         uMeteoSens[cConfSMetSens];
} sens_t;

extern calibration_t caldata;
extern sens_t sensdata;

void reset_calibration(void);

void Measure(void);
void CheckInputConfig(void);

#endif
