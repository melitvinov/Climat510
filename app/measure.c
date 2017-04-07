#include "syntax.h"

#include "405_ConfigEnRuSR.h"

#include "control_gd.h"
#include "module.h"
#include "fbd.h"
#include "measure.h"

calibration_t caldata;
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


void CheckSensLevsNew(char fnTepl,uint8_t fnSens,char full,char met,int16_t Mes)
{
    const int16_t         *uS;
    const eNameASens  *nameS;
    eSensing    *valueS;
    int16_t         *llS;
    int16_t         *lS;
    int16_t         *levelS;
    uint8_t         *tPause;
    uS=&sensdata.uInTeplSens[fnTepl][fnSens];
    nameS=&NameSensConfig[fnSens];

    valueS=&gd_rw()->Hot.Zones[fnTepl].InTeplSens[fnSens];
    llS=&gd_rw()->TControl.Zones[fnTepl].LastLastInTeplSensing[fnSens];
    lS=&gd_rw()->TControl.Zones[fnTepl].LastInTeplSensing[fnSens];
    levelS=gd_rw()->Level.InTeplSens[fnTepl][fnSens];
    tPause=&gd_rw()->TControl.Zones[fnTepl].TimeInTepl[fnSens];
    if (met)
    {
        uS=&sensdata.uMeteoSens[fnSens];
        nameS=&NameSensConfig[fnSens+cConfSSens];
        valueS=&gd_rw()->Hot.MeteoSensing[fnSens];
        //llS=&GD.TControl.LastLastMeteoSensing[fnSens];
        //lS=&GD.TControl.LastMeteoSensing[fnSens];
        levelS=gd_rw()->Level.MeteoSens[fnSens];
        tPause=&gd_rw()->TControl.TimeMeteoSensing[fnSens];
    }
    if (full)
    {
        if (((*uS)<nameS->uMin)||((*uS)>nameS->uMax))
            valueS->RCS |= cbMinMaxUSens;
    }
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
            if (met) break;

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
    if (!met)
        CheckDigitMidl(valueS,&Mes,&valueS->Value,tPause,nameS->DigitMidl);
    if (nameS->TypeSens == cTypeFram)
    {
        if (! (gd()->TControl.Zones[fnTepl].MechBusy[fnSens-cSmWinNSens+cHSmWinN].RCS & cMSBusyMech))
            gd_rw()->TControl.Zones[fnTepl].MechBusy[fnSens-cSmWinNSens+cHSmWinN].RCS |= cMSFreshSens;

    }
    if (nameS->TypeSens == cTypeScreen)
    {
        if (! (gd()->TControl.Zones[fnTepl].MechBusy[cHSmScrTH].RCS & cMSBusyMech))
            gd_rw()->TControl.Zones[fnTepl].MechBusy[cHSmScrTH].RCS |= cMSFreshSens;

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

void  CalibrNew(char nSArea,char nTepl, char nSens,int16_t Mes)
{
    eSensing    *fSens;
    const eNameASens  *fNameSens;
    int16_t     *fuSens;
    module_input_cfg_t  *fCalSens;
    char        met=0;
    if (nSArea)
    {
        fSens=&gd_rw()->Hot.Zones[nTepl].InTeplSens[nSens];
        fuSens=&sensdata.uInTeplSens[nTepl][nSens];
        fCalSens=&caldata.IndoorSensors[nTepl][nSens];
        fNameSens=&NameSensConfig[nSens];
        met=0;
    }
    else
    {
        fSens=&gd_rw()->Hot.MeteoSensing[nSens];
        fuSens=&sensdata.uMeteoSens[nSens];
        fCalSens=&caldata.MeteoSensors[nSens];
        fNameSens=&NameSensConfig[nSens+cConfSSens];
        met=1;
    }
    fSens->RCS=(fSens->RCS&(cbNotGoodSens+cbDownAlarmSens+cbUpAlarmSens));
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
            //Mes=(int)((long int)Mes*(long int)1000/(long int)GD.Cal.Port);
            fuSens[0]=Mes;
//			if(Mes>5000)
//				Mes=0;
            long long_x = ((long)fCalSens->v0-(long)fCalSens->v0)
                          *((long)Mes-(long)fCalSens->u0);
            Mes=(int16_t)(long_x/((long)fCalSens->u0-(long)fCalSens->u0));
            Mes=Mes+fCalSens->v0;
            CheckSensLevsNew(nTepl,nSens,1,met,Mes);
            return;
        }
    }
}

void Measure(void)
{
    char tTepl,nSens;
    uint16_t    tSensVal;
    int nModule;
    int8_t ErrModule;
    for (tTepl=0;tTepl<NZONES;tTepl++)
    {
        for (nSens=0;nSens<cConfSSens;nSens++)
        {
            uint mapping = GetIndoorSensorConfig(tTepl,nSens);

            tSensVal=GetInIPC(mapping / 100, mapping % 100 - 1,&ErrModule);
            if (ErrModule<0)
            {
                gd_rw()->Hot.Zones[tTepl].InTeplSens[nSens].RCS=cbNoWorkSens;
                gd_rw()->Hot.Zones[tTepl].InTeplSens[nSens].Value=0;
                sensdata.uInTeplSens[tTepl][nSens]=0;
                continue;
            }
            if (ErrModule>=iMODULE_MAX_ERR)
                tSensVal=0;
            CalibrNew(1,tTepl,nSens,tSensVal);
        }
    }
    for (nSens=0;nSens<cConfSMetSens;nSens++)
    {
        uint mapping = GetMeteoSensorConfig(nSens);

        tSensVal=GetInIPC(mapping / 100, mapping % 100 - 1, &ErrModule);
        if (ErrModule<0)
        {
            gd_rw()->Hot.MeteoSensing[nSens].RCS=cbNoWorkSens;
            sensdata.uMeteoSens[nSens]=0;
            continue;
        }
        if (ErrModule>=iMODULE_MAX_ERR)
            tSensVal=0;
        CalibrNew(0,0,nSens,tSensVal);
    }
}

void CheckInputConfig()
{
    static const module_input_cfg_t dummy_config;

    for (uint zone_idx=0; zone_idx < NZONES; zone_idx++)
    {
        for (uint sensor_idx =0; sensor_idx < cConfSInputs; sensor_idx++)
        {
            uint mapping = GetInputConfig(zone_idx,sensor_idx);
            UpdateInputConfig(mapping / 100, mapping % 100 - 1, &dummy_config);
        }
    }
    for (uint zone_idx=0;zone_idx<NZONES;zone_idx++)
    {
        for (uint sensor_idx = 0; sensor_idx < cConfSSens; sensor_idx++)
        {
            uint mapping = GetIndoorSensorConfig(zone_idx, sensor_idx);
            UpdateInputConfig(mapping / 100, mapping % 100 - 1, &caldata.IndoorSensors[zone_idx][sensor_idx]);
        }
    }
    for (uint sensor_idx = 0; sensor_idx < cConfSMetSens; sensor_idx++)
    {
        uint mapping = GetMeteoSensorConfig(sensor_idx);
        UpdateInputConfig(mapping / 100, mapping % 100 - 1, &caldata.MeteoSensors[sensor_idx]);
    }

}

void reset_calibration(void)
{
    module_input_cfg_t *eCS;

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
    memclr(&sensdata.uInTeplSens,sizeof(sensdata.uInTeplSens));
}

void LoadDiscreteInputs(void)
{
    for (int zone_idx=0; zone_idx<NZONES; zone_idx++)
    {
        eZone *zone = &gd_rw()->Hot.Zones[zone_idx];
        for (int input_idx=0; input_idx < cConfSInputs; input_idx++)
        {
            #warning "discrete_inputs stuck and never clears ? nice !"

            uint mapping = GetInputConfig(zone_idx, input_idx);

            if (GetDiskrIPC(mapping / 100, mapping % 100 - 1))
                zone->discrete_inputs[0] |= 1<<input_idx;
        }
    }
}
