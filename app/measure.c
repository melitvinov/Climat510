#include "syntax.h"

#include "405_ConfigEnRuSR.h"

#include "control_gd.h"
//#include "module.h"
#include "fbd.h"
#include "measure.h"

calibration_t caldata;

#warning "these data are only for intermediate results and calibration. may be refactored away later"
sens_t sensdata;

static uint GetInputConfig(uint zone_idx, uint sensor_idx)
{
    return gd()->MechConfig[zone_idx].RNum[INPUTS_REGS_OFFSET + sensor_idx];
}


static uint GetIndoorSensorConfig(uint zone_idx, uint sensor_idx)
{
    return gd()->MechConfig[zone_idx].RNum[SENSORS_REGS_OFFSET + sensor_idx];
}

static void CheckDigitMidl(eSensing *ftemp,int16_t* Mes, int16_t* ValueS, uint8_t* tPause, uint16_t tFilter)
{
    if (((*Mes<*ValueS+tFilter)&&(*Mes>*ValueS-tFilter))||(*tPause>30)||(!*ValueS))
        *tPause=0;
    else
    {
        *Mes=*ValueS;
        if (*tPause<120) (*tPause)++;
    }

/*	if ((YesBit(ftemp->RCS,cbNotGoodSens)))
        ClrBit(ftemp->RCS,cbNotGoodSens);
    else
    {
        if ((*Mes>*ValueS+50)||(*Mes<*ValueS-50))
        {
            ftemp->RCS |= cbNotGoodSens;
            *Mes=*ValueS;
        }
    }*/
}


void CheckSensLevsNew(char zone_idx, uint8_t sensor_idx, bool is_meteo_sensor,int16_t Mes, int16_t raw_sample)
{
    const eNameASens  *nameS;
    eSensing    *valueS;
    int16_t         *llS;
    int16_t         *lS;
    uint8_t         *tPause;
    nameS=&NameSensConfig[sensor_idx];

    valueS=&gd_rw()->Hot.Zones[zone_idx].IndoorSensors[sensor_idx];
    llS=&gd_rw()->TControl.Zones[zone_idx].LastLastInTeplSensing[sensor_idx];
    lS=&gd_rw()->TControl.Zones[zone_idx].LastInTeplSensing[sensor_idx];
    tPause=&gd_rw()->TControl.Zones[zone_idx].TimeInTepl[sensor_idx];

    if (is_meteo_sensor)
    {
        nameS=&NameSensConfig[sensor_idx+cConfSSens];
        valueS=&gd_rw()->Hot.MeteoSensing[sensor_idx];
        //llS=&GD.TControl.LastLastMeteoSensing[fnSens];
        //lS=&GD.TControl.LastMeteoSensing[fnSens];
        tPause=&gd_rw()->TControl.TimeMeteoSensing[sensor_idx];
    }

    // GUESS: check raw measurement ('voltage')
    if (raw_sample < nameS->uMin || raw_sample > nameS->uMax)
        valueS->RCS |= cbMinMaxUSens;

    // GUESS: clamp cooked measurement min and max
    if (Mes < nameS->Min)
    {
        if ((nameS->TypeSens == cTypeSun)||(nameS->TypeSens == cTypeRain)||(nameS->TypeSens == cTypeFram)||(nameS->TypeSens == cTypeScreen))
            Mes=nameS->Min;
        else
        {
            valueS->RCS |= cbMinMaxVSens;
            Mes=0;
        }
    }
    if (Mes > nameS->Max)
    {
        if ((nameS->TypeSens == cTypeRain)||(nameS->TypeSens == cTypeRH)||(nameS->TypeSens == cTypeFram)||(nameS->TypeSens == cTypeScreen))
            Mes=nameS->Max;
        else
        {
            valueS->RCS |= cbMinMaxVSens;
            Mes=0;
        }
    }

    switch (nameS->TypeMidl)
    {
    case cNoMidlSens:
        break;
    case c2MidlSens:
        (*llS)=0;
    case c3MidlSens:
        {
            if (is_meteo_sensor) break;

            int16_t int_x = (*llS);
            int16_t int_y = (*lS);
            (*llS)=int_y;
            (*lS)=Mes;
            int16_t int_z = 0;
            if (Mes) int_z++;
            if (int_x) int_z++;
            if (int_y) int_z++;
            if (int_z) Mes=(Mes+int_x+int_y)/int_z;

        }
        break;
    case cExpMidlSens:
        if (!(*lS)) (*lS)=Mes;
        Mes=(int)((((long int)(*lS))*(1000-cKExpMidl)+((long int)Mes)*cKExpMidl)/1000);
        (*lS)=Mes;
        break;
    }
    if (!is_meteo_sensor)
        CheckDigitMidl(valueS,&Mes,&valueS->Value,tPause,nameS->DigitMidl);
    if (nameS->TypeSens == cTypeFram)
    {
        if (! (gd()->TControl.Zones[zone_idx].MechBusy[sensor_idx-cSmWinNSens+cHSmWinN].RCS & cMSBusyMech))
            gd_rw()->TControl.Zones[zone_idx].MechBusy[sensor_idx-cSmWinNSens+cHSmWinN].RCS |= cMSFreshSens;

    }
    if (nameS->TypeSens == cTypeScreen)
    {
        if (! (gd()->TControl.Zones[zone_idx].MechBusy[cHSmScrTH].RCS & cMSBusyMech))
            gd_rw()->TControl.Zones[zone_idx].MechBusy[cHSmScrTH].RCS |= cMSFreshSens;

    }
    valueS->Value=Mes;
    /*ClrBit(valueS->RCS,(cbDownAlarmSens+cbUpAlarmSens));
    if ((levelS[cSmDownCtrlLev])&&(Mes <= levelS[cSmDownCtrlLev]))
        valueS->RCS |= cbDownCtrlSens;
    if ((levelS[cSmUpCtrlLev])&&(Mes >= levelS[cSmUpCtrlLev]))
        valueS->RCS |= cbUpCtrlSens;
    if ((levelS[cSmDownAlarmLev])&&(Mes <= levelS[cSmDownAlarmLev]))
    {
        valueS->RCS |= cbDownAlarmSens;
        return;
    }
    if ((levelS[cSmUpAlarmLev])&&(Mes >= levelS[cSmUpAlarmLev]))
    {
        valueS->RCS |= cbUpAlarmSens;
        return;
    }*/
}

static void CalibrNew(bool is_indoor_sensor, uint zone_idx, char sensor_idx, int16_t sample)
{
    eSensing    *fSens;
    const eNameASens  *fNameSens;
    int16_t     *fuSens;
    const board_input_cfg_t  *fCalSens;

    if (is_indoor_sensor)
    {
        fSens = &gd_rw()->Hot.Zones[zone_idx].IndoorSensors[sensor_idx];
        fuSens = &sensdata.uIndoorSensors[zone_idx][sensor_idx];
        fCalSens = &caldata.IndoorSensors[zone_idx][sensor_idx];
        fNameSens=&NameSensConfig[sensor_idx];
    }
    else
    {
        fSens=&gd_rw()->Hot.MeteoSensing[sensor_idx];
        fuSens=&sensdata.uMeteoSens[sensor_idx];
        fCalSens=&caldata.MeteoSensors[sensor_idx];
        fNameSens=&NameSensConfig[sensor_idx+cConfSSens];
    }

    // leave only these fields
    fSens->RCS &= cbNotGoodSens | cbDownAlarmSens | cbUpAlarmSens;

    switch (fNameSens->TypeSens)
    {
    case cTypeFram:
    case cTypeScreen:
    case cTypeSun:
    case cTypeRain:
    case cTypeAnal:
    case cTypeRH:
    case cTypeMeteo:
        {
            *fuSens = sample;

            int16_t cooked_sample = sample;

            #warning "so fckn calibration is applied right here, not in sensor"
//			if(Mes>5000)
//				Mes=0;
            long long_x = ((long)fCalSens->v0 -(long)fCalSens->v0) * ((long)sample-(long)fCalSens->u0);
            cooked_sample = (int16_t)(long_x/((long)fCalSens->u0-(long)fCalSens->u0));
            cooked_sample += fCalSens->v0;
            CheckSensLevsNew(zone_idx,sensor_idx, !is_indoor_sensor, cooked_sample, sample);
            return;
        }
    }
}


void Measure(void)
{
    for (uint zone_idx=0;zone_idx<NZONES;zone_idx++)
    {
        for (uint sensor_idx=0; sensor_idx<cConfSSens; sensor_idx++)
        {
            uint mapping = GetIndoorSensorConfig(zone_idx, sensor_idx);

            board_t *b = fbd_find_board_by_addr(mapping / 100);

            if (! b)
            {
                gd_rw()->Hot.Zones[zone_idx].IndoorSensors[sensor_idx].RCS = cbNoWorkSens;
                gd_rw()->Hot.Zones[zone_idx].IndoorSensors[sensor_idx].Value = 0;
                sensdata.uIndoorSensors[zone_idx][sensor_idx]=0;
                continue;
            }

            u16 val = 0;
            if (! fbd_get_stat(b)->permanent_errs)
                val = fbd_read_input(b, mapping % 100 - 1);

            CalibrNew(1,zone_idx,sensor_idx,val);
        }
    }
    for (uint sensor_idx=0; sensor_idx < cConfSMetSens; sensor_idx++)
    {
        uint mapping = GetMeteoSensorConfig(sensor_idx);

        board_t *b = fbd_find_board_by_addr(mapping / 100);

        if (! b)
        {
            gd_rw()->Hot.MeteoSensing[sensor_idx].RCS=cbNoWorkSens;
            sensdata.uMeteoSens[sensor_idx]=0;
            continue;
        }

        u16 val = 0;
        if (! fbd_get_stat(b)->permanent_errs)
            val = fbd_read_input(b, mapping % 100 - 1);

        CalibrNew(0,0,sensor_idx,val);
    }
}

void UpdateInputConfigs(void)
{
    unmount_unused_boards();
    mount_used_boards();

    static const board_input_cfg_t dummy_config;

    for (uint zone_idx=0; zone_idx < NZONES; zone_idx++)
    {
        for (uint sensor_idx =0; sensor_idx < cConfSInputs; sensor_idx++)
        {
            uint mapping = GetInputConfig(zone_idx, sensor_idx);
            board_t *b = fbd_find_board_by_addr(mapping / 100);
            if (b)
                fbd_register_input_config(b, mapping % 100 - 1, &dummy_config);
        }
    }
    for (uint zone_idx=0;zone_idx<NZONES;zone_idx++)
    {
        for (uint sensor_idx = 0; sensor_idx < cConfSSens; sensor_idx++)
        {
            uint mapping = GetIndoorSensorConfig(zone_idx, sensor_idx);

            board_t *b = fbd_find_board_by_addr(mapping / 100);
            if (b)
                fbd_register_input_config(b, mapping % 100 - 1, &caldata.IndoorSensors[zone_idx][sensor_idx]);
        }
    }
    for (uint sensor_idx = 0; sensor_idx < cConfSMetSens; sensor_idx++)
    {
        uint mapping = GetMeteoSensorConfig(sensor_idx);

        board_t *b = fbd_find_board_by_addr(mapping / 100);
        if (b)
            fbd_register_input_config(b, mapping % 100 - 1, &caldata.MeteoSensors[sensor_idx]);
    }
}


// XXX: just for now: mount the modules once
#warning "ordering is wrong ?"

static bool is_board_used(uint addr)
{
    for (uint zone_idx = 0; zone_idx < NZONES; zone_idx++)
    {
        for (uint mech_idx = 0; mech_idx < countof(gd()->MechConfig[0].RNum); mech_idx++)
        {
            uint mapping = gd()->MechConfig[zone_idx].RNum[mech_idx];
            if (mapping / 100 == addr && (mapping % 100 != 0))
                return 1;
        }
    }

    return 0;
}

void unmount_unused_boards(void)
{
    for (board_t *b = fbd_next_board(NULL); b; b = fbd_next_board(b))
    {
        if (! is_board_used(fbd_get_addr(b)))
            fbd_unmount(b);
    }
}

void mount_used_boards(void)
{
    for (uint zone_idx = 0; zone_idx < NZONES; zone_idx++)
    {
        for (uint mech_idx = 0; mech_idx < countof(gd()->MechConfig[0].RNum); mech_idx++)
        {
            uint mapping = gd()->MechConfig[zone_idx].RNum[mech_idx];

            if (mapping / 100 != 0 && mapping % 100 != 0)
                fbd_mount(mapping / 100);
        }
    }
}


void reset_calibration(void)
{
    board_input_cfg_t *eCS;

    for (int i=0; i<cConfSMetSens;i++)
    {
        eCS=&caldata.MeteoSensors[i];
        eCS->v0=NameSensConfig[i+cConfSSens].vCal[0];
        eCS->v1=NameSensConfig[i+cConfSSens].vCal[1];
        eCS->u0=NameSensConfig[i+cConfSSens].uCal[0];
        eCS->u1=NameSensConfig[i+cConfSSens].uCal[1];
        eCS->type=NameSensConfig[i+cConfSSens].TypeInBoard;
        eCS->output=NameSensConfig[i+cConfSSens].Output;
        //eCS->Input=OutPortsAndInputs[ByteX][0];
        //eCS->nInput=OutPortsAndInputs[ByteX][1];
    }

    for (int zone_idx=0;zone_idx<NZONES;zone_idx++)
    {
        for (int byte_y=0;byte_y<cConfSSens;byte_y++)
        {
            eCS=&caldata.IndoorSensors[zone_idx][byte_y];
            eCS->v0=NameSensConfig[byte_y].vCal[0];
            eCS->v1=NameSensConfig[byte_y].vCal[1];
            eCS->u0=NameSensConfig[byte_y].uCal[0];
            eCS->u1=NameSensConfig[byte_y].uCal[1];
            eCS->output=NameSensConfig[byte_y].Output;
            eCS->type=NameSensConfig[byte_y].TypeInBoard;
            //eCS->nInput=InPortsAndInputs[ByteX][ByteY][1];
        }
    }

    #warning "meteo is not cleaned"
    memclr(&sensdata.uIndoorSensors,sizeof(sensdata.uIndoorSensors));
}

#warning "maybe we could sanitize RNum while receiving block (at frontend)"

void LoadDiscreteInputs(void)
{
    for (int zone_idx=0; zone_idx<NZONES; zone_idx++)
    {
        eZone *zone = &gd_rw()->Hot.Zones[zone_idx];
        for (int input_idx = 0; input_idx < cConfSInputs; input_idx++)
        {
            #warning "discrete_inputs stuck and never clears ? nice !"

            uint mapping = GetInputConfig(zone_idx, input_idx);
            board_t *b = fbd_find_board_by_addr(mapping / 100);

            if (! b)
                continue;

            if (mapping % 100 == 0)
                continue;

            #warning "its the (almost_ original logic, copied verbatim here :-)"
            if (fbd_get_stat(b)->permanent_errs)
                continue;

            u16 val = fbd_read_input(b, mapping % 100 - 1);

            if (val > 2500)
                zone->discrete_inputs[0] |= 1<<input_idx;
        }
    }
}


void ClrAllOutIPCDigit(void)
{
    for (board_t *b = fbd_next_board(NULL); b; b = fbd_next_board(b))
    {
        fbd_write_discrete_outputs(b, 0, ~0);
    }
}
