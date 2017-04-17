#include "syntax.h"

// XXX: isolation

#include "405_memory.h"
#include "405_ConfigEnRuSR.h"
#include "405_EngRus.h"

#include "control.h"
#include "control_siod.h"

//#include "module.h"
#include "fbd.h"
#include "measure.h"

// for modul_sum constant
#include "lcd.h"
#include "wtf.h"

#include "unsorted.h"

#include "405_menu.h"

#define	MaxY_menu		9

#define SUM_NAME_TUNE sizeof(NameConst)

#define SumTeplZones	(gd()->Control.ConfSTepl)


// XXX: this stuff is from climdef.h
uchar StartMenuY;
uchar NowCurRow;
uchar NewCurRow;
int Savebuf;
//char	ToLowTime, ToHiTime;
uchar   StartY_menu2;

char GrafView;

extern char lcdbuf[];
extern uchar Form;
extern uchar   Ad_Buf;
extern uchar   AdinB;
extern int8_t      SaveChar;
extern uint16_t Y_menu2;
extern uint16_t Y_menu;
extern uint16_t x_menu;
extern bool     BlkW;
extern bool EndInput;

extern int MinimVal;
extern int MaximVal;
extern int16_t SaveInt;

extern uchar SumYMenu;

extern uint16_t ctx__fnSIOfaza[8];
extern uint16_t ctx__fnSIOpumpOut[8];
extern uint16_t ctx__fnSIOvelvOut[8];
extern uint16_t ctx__fnSIOpause[8];

static const uchar DayOfWeek[][10]=
{
    "Mon#понед",
    "Tue#вторн",
    "Wed#среда",
    "Thu#четв",
    "Fri#пятн",
    "Sat#субб",
    "Sun#воскр"
};

static int16_t SaveInt2;
static uchar nBlEEP;


void GMenu(void) {
    uchar   numRow;
    //NowDigCal = 0;
    if (GrafView)
        return;

    clear_d();
    Form = 0;
    Ad_Buf = 0;
    AdinB = 0;
    if (Y_menu > MaxY_menu) Y_menu = MaxY_menu;
//***************************************************
//---- Если в крайнем столбце -----
    if (!x_menu)
    {
        if (Y_menu > (StartMenuY+5)) StartMenuY = Y_menu-5;
        if (Y_menu < StartMenuY) StartMenuY = Y_menu;
//---- Вывести SUM_LINE_DISP-2 строк главного меню  -----
        for (numRow = 0;numRow < (SUM_LINE_DISP-2);numRow++)
        {
            Ad_Buf = numRow*DisplCols;
            YMenu(numRow+StartMenuY);
        }
        NewCurRow = Y_menu - StartMenuY + 1;
        // looks like this is 'the menu cursor'
        lcdbuf[NewCurRow*DisplCols-19]=130;
        if (wtf0.Second & 1)
            lcdbuf[NewCurRow*DisplCols-18]=130;
        else
            lcdbuf[NewCurRow*DisplCols-20]=130;

        // xxx: render menu caption ?
        SendBlock(&lcdbuf[Str2d], Str2d,23);   //send tu str

//---- Вывести инверсный фон -----
        if (NewCurRow !=NowCurRow)
        {
//				SetRow(0xFF,NewCurRow);
//				if(NowCurRow) SetRow(0x00,NowCurRow);
            NowCurRow = NewCurRow;
        }
        return;
    }
    else  NewCurRow = 0;
//---- Стереть фон -----
    if (NewCurRow != NowCurRow)
    {
//			if(NowCurRow) SetRow(0x00,NowCurRow);
        NowCurRow = NewCurRow;
//			if (x_menu) SendSim(Tire,1);
    }
//-----------------------
// ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  == =
    if (wtf0.Menu&&(gd()->TControl.NowCod != gd()->Control.Cod))
        if (SaveChar!=127)
        {
            w_txt(Mes65); //Access code~
//				w_txt(Mes1);
            w_int(&gd()->TControl.NowCod,SSS, 0);
            SaveChar = 126;
            return;
        }
    YMenu(Y_menu);
}

void pmReset(void) {
//char nBlk;
    Y_menu2%=5;
    Ad_Buf = Str2;
    w_txt(Mes66);   // Задания //Init Tasks~
    w_int(&BlockEEP[1].Erase,oS, 0);
    if (Y_menu2 == 0)
    {
        BlkW = 1; nBlEEP = 1;
    }
    Ad_Buf++;
    w_txt(Mes221);   // <Input 1 for init>
    Ad_Buf = Str3;
    w_txt(Mes67); // Калибровки  //Init Calibrations~
    w_int(&BlockEEP[5].Erase,oS, 0);
    if (Y_menu2 == 1)
    {
        BlkW = 1; nBlEEP = 5;
    }
    Ad_Buf = Str4;
    w_txt(Mes68);   // Параметры  //Init Parameters~
    w_int(&BlockEEP[0].Erase,oS, 0);
    if (Y_menu2 == 2)
    {
        BlkW = 1; nBlEEP = 0;
    }
    Ad_Buf = Str5;
    if (Y_menu2<=3)
    {
        if (Y_menu2 == 3) nBlEEP = 4;
        w_txt(Mes69);   // Конфигурация  //Init Configuration~
        w_int(&BlockEEP[4].Erase,oS, 0);
    }
    else
    {
        if (Y_menu2 == 4) nBlEEP = 3;
        w_txt(Mes222);   // Init Strategy~
        w_int(&BlockEEP[3].Erase,oS, 0);
    }
    if (EndInput && (BlockEEP[nBlEEP].Erase  ==  1))
    {
        EndInput = 0;
        TestMem(nBlEEP+10);
//			RamToEEP(nBlEEP);
    }
}


// ==  ==  ==  ==  == = СОСТОЯНИЕ ПОЛИВА В ПОСЛЕДНИЙ КАДР  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  == =
//char Proces,Proces2;
void pmInfoProg405(void){
    BlkW = 1;
    int16_t zone_idx=(wtf0.Second / 6) % gd()->Control.ConfSTepl;
    int16_t idx=(wtf0.Second/2)%3;
    w_txt(Mes7); //Zone
    int gh_num = zone_idx+1;
    w_int(&gh_num,SS, 0);
    Ad_Buf++;
//	IntX = gd_get()->Hot.MidlSR;
//	if (IntX<0) IntX=-IntX;
//	w_int(&IntX,SSSS, 0);

    #warning "removed fandata"
    //w_int(&gd()->FanBlock[0][0].FanData[0].actual_speed,SSSS, 0);

    if (!idx)
    {
        w_txt(Mes40);
        w_int(&gd()->Hot.Zones[zone_idx].AllTask.DoTHeat,SSpS0, 0);
        Ad_Buf++;
        w_txt(Mes41);
        w_int(&gd()->Hot.Zones[zone_idx].AllTask.DoTVent,SSpS0, 0);
        Ad_Buf++;
        w_txt(Mes42);
        w_int(&gd()->Hot.Zones[zone_idx].IndoorSensors[cSmTSens1].Value,SSpS0, 0);
        w_int(&gd()->Hot.Zones[zone_idx].IndoorSensors[cSmTSens2].Value,SSpS0, 0);
        w_int(&gd()->Hot.Zones[zone_idx].IndoorSensors[cSmTSens3].Value,SSpS0, 0);
        w_int(&gd()->Hot.Zones[zone_idx].IndoorSensors[cSmTSens4].Value,SSpS0, 0);
        return;
    }
    if (idx == 1)
    {
        w_txt(Mes43);
        w_int(&gd()->Hot.Zones[zone_idx].AllTask.DoRHAir,SSpS0, 0);
        Ad_Buf++;
        w_txt(Mes44);
        w_int(&gd()->Hot.Zones[zone_idx].IndoorSensors[cSmRHSens].Value,SSpS0, 0);
        return;
    }
    w_txt(Mes45);
    w_int(&gd()->Hot.Zones[zone_idx].AllTask.DoCO2,SSSS, 0);
    Ad_Buf++;
    w_txt(Mes46);
    w_int(&gd()->Hot.Zones[zone_idx].IndoorSensors[cSmCOSens].Value,SSSS, 0);


}


void pmDate(void) {
    Ad_Buf = Str2;
    w_txt(Proect);
    w_int(&gd()->Control.rModification,SS, 0);
    lcdbuf[Ad_Buf++]='-';
    w_int(&gd()->Control.rSInTeplSens,SS, 0);
//	w_txt("-M");
//    w_int(&gd_get()->Config[cfSumTap],oS, 0);
//	w_txt("-V");
//    w_int(&gd_get()->Config[cfSumIrrigVal],SS, 0);
    Ad_Buf = Str3;
    w_txt(Mes70); //Time ~

    w_int(&gd()->Hot.Time,SSdSS, 0);

    lcdbuf[Ad_Buf++]=':';
    lcdbuf[Ad_Buf++]=wtf0.Second/10+'0';
    lcdbuf[Ad_Buf++]=wtf0.Second%10+'0';
    if (!Y_menu2) BlkW = 1;
    Ad_Buf = Str4;
    w_txt(Mes71); //Date~
    w_int(&gd()->Hot.Date,DsMsY, 0);
    Ad_Buf++;
    int day = NowDayOfWeek;
    int day_of_week;
    for (day_of_week = 0;day_of_week < 7; day_of_week++)
    {
        if (day&64) break;
        day<<=1;
    }
    if (day_of_week <7) w_txt(&DayOfWeek[day_of_week][0]);    //&
    if (EndInput)
    {
        #warning "looks like this baby just set time"
        EndInput = 0;
        if (gd()->Hot.Time>=24*60)
            gd_rw()->Hot.Time = 1;
//		CalcDayOfWeek();
        SetRTC();
    }

}

void pmStrategy(void) {
    w_txt(Mes72); //Irrigation archive
    if (x_menu>cSWaterKontur) x_menu = 0;
    if (!x_menu) return;
    Y_menu2%=3;
    Ad_Buf = Str2;
    w_txt(Mes76);
    w_int(&x_menu,SS, 0);
    Ad_Buf = Str3;
    w_txt(Mes73);
    w_int(&gd()->Strategy[0][x_menu-1].TempPower,SS, 0);
    if (!Y_menu2) BlkW = 1;
    Ad_Buf = Str4;
    w_txt(Mes74);
    w_int(&gd()->Strategy[0][x_menu-1].RHPower,SS, 0);
    if (Y_menu2 == 1) BlkW = 1;
    Ad_Buf = Str5;
    w_txt(Mes75);
    w_int(&gd()->Strategy[0][x_menu-1].OptimalPower,SS, 0);
    return;
}

void pmParam() {
    const char *OutStr;
    w_txt(Mes85); //Parameters
    if (!x_menu) return;

    int byte_z = x_menu-1;
    int byte_x = 0;
    int byte_y = 0;
    int byte_w = 0;

    Ad_Buf = Str2;
    if (!byte_z)
    {
        pmDate(); return;
    }
    byte_z--;
    if (byte_z<SumTeplZones)
    {
        if (Y_menu2 > SUM_NAME_CONF) Y_menu2 = 0;
        if (Y_menu2 >= SUM_NAME_CONF) Y_menu2 = SUM_NAME_CONF-1;
        w_txt(Mes86); //-Configuration-
        Ad_Buf++;
        w_txt(Mes7);
        Ad_Buf++;
        byte_x = byte_z+1;
        w_int(&byte_x,SS, 0);
        Ad_Buf = Str3;
        if (Y_menu2 < StartY_menu2) StartY_menu2 = Y_menu2;
        if (Y_menu2 > (StartY_menu2+2)) StartY_menu2 = Y_menu2-2;
        for (byte_y = StartY_menu2;byte_y < (StartY_menu2+3);byte_y++)
        {
            byte_x = byte_y % SUM_NAME_CONF;
            OutStr = NameOutputConfig[byte_x].Name;
            byte_w = byte_x;
            if (byte_x >= INPUTS_REGS_OFFSET)
                OutStr = NameInputConfig[byte_x-INPUTS_REGS_OFFSET].Name;
            if (byte_x >= SENSORS_REGS_OFFSET)
                OutStr = NameSensConfig[byte_x-SENSORS_REGS_OFFSET].Name;
            w_txt(OutStr);
            Ad_Buf=(Ad_Buf / DisplCols)*DisplCols+20;
            lcdbuf[Ad_Buf++]=':';
            w_int(&gd()->MechConfig[byte_z].RNum[byte_w],SpSSpSS, 0);
            if (Y_menu2  ==  byte_y) BlkW = 1;
            //w_txt(Mes87); //-rela
            Ad_Buf=((Ad_Buf / DisplCols)+1)*DisplCols;
        }
        if (!gd()->TControl.NowCod) Form = 0;
        return;
    }
    byte_z-=SumTeplZones;
    if (byte_z<SumTeplZones)
    {
        if (Y_menu2 > cConfSSystem) Y_menu2 = 0;
        if (Y_menu2 >= cConfSSystem) Y_menu2 = cConfSSystem-1;
        w_txt(Mes86); //-Configuration-
        Ad_Buf++;
        w_txt(Mes7);
        Ad_Buf++;
        byte_x = byte_z+1;
        w_int(&byte_x,SS, 0);
        Ad_Buf = Str3;
        if (Y_menu2 < StartY_menu2) StartY_menu2 = Y_menu2;
        if (Y_menu2 > (StartY_menu2+2)) StartY_menu2 = Y_menu2-2;
        for (byte_y = StartY_menu2;byte_y < (StartY_menu2+3);byte_y++)
        {
            byte_x = byte_y % SUM_NAME_CONF;
            OutStr = NameSystemConfig[byte_x].Name;
            byte_w = byte_x;
            w_txt(OutStr);
            Ad_Buf=(Ad_Buf / DisplCols)*DisplCols+20;
            lcdbuf[Ad_Buf++]=':';
            w_int(&gd()->MechConfig[byte_z].Systems[byte_w],SSSi, 0);
            if (Y_menu2  ==  byte_y) BlkW = 1;
            //w_txt(Mes87); //-rela
            Ad_Buf=((Ad_Buf / DisplCols)+1)*DisplCols;
        }
        if (!gd()->TControl.NowCod)
            Form = 0;
        return;
    }
    byte_z-=SumTeplZones;
    if (byte_z<SumTeplZones)
    {
        if (Y_menu2 > SUM_NAME_PARS) Y_menu2 = 0;
        if (Y_menu2 >=SUM_NAME_PARS) Y_menu2 = SUM_NAME_PARS-1;
        w_txt(Mes90); //-Correction-
        Ad_Buf++;
        w_txt(Mes7);
        Ad_Buf++;
        byte_x = byte_z+1;
        w_int(&byte_x,SS, 0);
        Ad_Buf = Str3;
        if (Y_menu2 < StartY_menu2) StartY_menu2 = Y_menu2;
        if (Y_menu2 > (StartY_menu2+2)) StartY_menu2 = Y_menu2-2;
        for (byte_y = StartY_menu2;byte_y < (StartY_menu2+3);byte_y++)
        {
            byte_x = byte_y % SUM_NAME_PARS;
            w_txt(NameParUpr[byte_x].Name);
            Ad_Buf=(Ad_Buf / DisplCols)*DisplCols+20;
            lcdbuf[Ad_Buf++]='=';
            w_int(&gd()->Control.Zones[byte_z].c_MaxTPipe[byte_y],NameParUpr[byte_x].Ed, 0);
            if (Y_menu2  ==  byte_y) BlkW = 1;
            Ad_Buf=((Ad_Buf / DisplCols)+1)*DisplCols;

        }
        return;
    }
    byte_z-=SumTeplZones;
    if (!byte_z)                                                     // Уставки
    {
        #warning "sizeof !? wtf ? should be countof. disabled"
        #if 0
        if (Y_menu2 > sizeof(NameConst))//SUM_NAME_TUNE)
            Y_menu2 = 0;
        if (Y_menu2 >= sizeof(NameConst))//SUM_NAME_TUNE)
            Y_menu2 = sizeof(NameConst)-1;//SUM_NAME_TUNE-1;
        w_txt(Mes91);
        Ad_Buf = Str3;
        if (Y_menu2 < StartY_menu2) StartY_menu2 = Y_menu2;
        if (Y_menu2 > (StartY_menu2+2)) StartY_menu2 = Y_menu2-2;
        for (ByteY = StartY_menu2;ByteY < (StartY_menu2+3);ByteY++)
        {
            ByteX = ByteY % SUM_NAME_TUNE;
            ByteW = ByteX+1;
            w_txt(Mes92);
            w_int(&ByteW,SS, 0);
            Ad_Buf=((Ad_Buf / DisplCols))*DisplCols+20;
            lcdbuf[Ad_Buf++]='=';
            w_int(&gd_get()->TuneClimate.s_TStart[ByteY],NameConst[ByteX].Frm, 0);
            if (Y_menu2  ==  ByteY)
                BlkW = 1;
            Ad_Buf=((Ad_Buf / DisplCols)+1)*DisplCols;

        }
        return;
        #endif
    }
    byte_z--;  //if (!gd_get()->Config[cfReturn1Val] && !gd_get()->Config[cfRegulRetEC])
    if (!byte_z)
    {
        Y_menu2%=8;
        w_txt(Mes126); //Controller num~
        w_int(&gd()->Control.NFCtr,SS, 0);
        if (!Y_menu2) BlkW = 1;
        if (!gd()->Hot.News) w_txt(" on line");
        Ad_Buf = Str3;
        w_txt(Mes127); //Language~
        w_int(&gd()->Control.Language,oS, 0);
        // w_int(&PowerOfLineRS,SSSS, 0);
        if (Y_menu2 == 1) BlkW = 1;

        Ad_Buf = Str4;
        w_txt(Mes65); //Access code~
        w_int(&gd()->Control.Cod,SSS, 0);
        SaveChar = 127;
        if (!wtf0.Menu) SaveChar = 0;
        if (Y_menu2 == 2) BlkW = 1;
        Ad_Buf = Str5;
        if (Y_menu2<=3)
        {
            w_txt(Mes5);
            w_int(&gd()->Control.ConfSTepl,oS, 0);
        }
        else
        {
            w_txt("IP:");
            w_int(&gd()->Control.IPAddr[0],SSS, 0);
            if (Y_menu2 == 4) BlkW = 1;
            w_txt(".");
            w_int(&gd()->Control.IPAddr[1],SSS, 0);
            if (Y_menu2 == 5) BlkW = 1;
            w_txt(".");
            w_int(&gd()->Control.IPAddr[2],SSS, 0);
            if (Y_menu2 == 6) BlkW = 1;
            w_txt(".");
            w_int(&gd()->Control.IPAddr[3],SSS, 0);

            //w_txt(Mes206);
            //w_int(&gd_get()->Control.Screener,SSS, 0);
        }
        return;
    }
    if (byte_z > 1)   x_menu = 0;
    pmReset();
}

void AutoMan(char RCS,int fint)
{
    if (RCS & fint)
        w_txt(Mes139);
    else
        w_txt(Mes138);
}
// ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ручное управление  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==
void pmHand(void) {
    w_txt(Mes128); //Ручное управление //Manual control~
    if (!x_menu)  return;
    Ad_Buf = Str2;

    int zone_idx = x_menu - 1;
    int contour_idx = 0;

    if (zone_idx<SumTeplZones)                     /* Клапан полива N */
    {
        while (Y_menu2< cSRegCtrl*2)
        {
            contour_idx = Y_menu2/2;
            if (gd()->MechConfig[zone_idx].RNum[contour_idx]) break;
            Y_menu2++;
        }
        if (Y_menu2 >= cSRegCtrl*2)
        {
            Y_menu2 = 0;
            w_txt(Mes89);
            return;
        }

        const eMechanic *hand_ctr = &gd()->Hot.Zones[zone_idx].HandCtrl[contour_idx];
        const eTControlZone *gh_ctrl = &gd()->TControl.Zones[zone_idx];

        Ad_Buf = Str2;
        w_txt(Mes7);
        Ad_Buf++;

        int gh_num = zone_idx+1;
        w_int(&gh_num,SS, 0);
        Ad_Buf++;
        w_txt(NameOutputConfig[contour_idx].Name); /* Клап бойлера*/ //Boiler val
        Ad_Buf = Str3;
        w_txt(Mes134); /* Ход клап */ //Boiler val time~~

        w_int(&hand_ctr->RCS, bS, 0x01);
        AutoMan(hand_ctr->RCS, 0x01);
        if (!(Y_menu2%2)) BlkW = 1;
        Ad_Buf = Str4;
        w_txt(Mes133); /* Ход клап */ //Boiler val time~~
        w_int(&hand_ctr->Position,SSS, 0);
        lcdbuf[Ad_Buf++]='%';
        BlkW = 1;
        lcdbuf[Ad_Buf++]='(';

        w_int(&gh_ctrl->MechBusy[contour_idx].TimeRealMech,SSSi, 0);
        w_txt("s) #с) ");
        if (gh_ctrl->MechBusy[contour_idx].Sens)
        {
            lcdbuf[Ad_Buf++]='(';
            w_int(&gh_ctrl->MechBusy[contour_idx].Sens->Value, SSSpS, 0);
            lcdbuf[Ad_Buf++]=')';
        }
        Ad_Buf = Str5;
        w_txt(Mes136); Ad_Buf++;

        w_int(&gd()->MechConfig[zone_idx].RNum[contour_idx],SpSSpSS, 0);

        return;
    }

    zone_idx-=SumTeplZones;

    if (zone_idx<SumTeplZones)
    {
        while (Y_menu2< cSDiskrCtrl*2)
        {
            contour_idx = cSRegCtrl+Y_menu2/2;
            if (gd()->MechConfig[zone_idx].RNum[contour_idx]) break;
            Y_menu2++;
        }
        if (Y_menu2 >= cSDiskrCtrl*2)
        {
            Y_menu2 = 0;
            w_txt(Mes89);
            return;
        }

        Ad_Buf = Str2;
        w_txt(Mes7);
        Ad_Buf++;
        int gh_num = zone_idx+1;
        w_int(&gh_num, SS, 0);
        Ad_Buf++;
        w_txt(NameOutputConfig[contour_idx].Name); /* Клап бойлера*/ //Boiler val
        Ad_Buf = Str3;
        w_txt(Mes134); /* Ход клап */ //Boiler val time~~

        const eMechanic *hand_ctr = &gd()->Hot.Zones[zone_idx].HandCtrl[contour_idx];

        w_int(&hand_ctr->RCS, bS, 0x01);
        AutoMan(hand_ctr->RCS,0x01);

        if (!(Y_menu2%2))
            BlkW = 1;

        if (contour_idx  ==  cHSmSIOPump)
        {
            lcdbuf[Ad_Buf++]='(';
            w_int(&siod_view()->fnSIOfaza[zone_idx],SS, 0);
            lcdbuf[Ad_Buf++]=',';
            w_int(&siod_view()->fnSIOvelvOut[zone_idx],SSSS, 0);
            lcdbuf[Ad_Buf++]=',';
            w_int(&siod_view()->fnSIOpumpOut[zone_idx],SSSS, 0);
            lcdbuf[Ad_Buf++]=',';
            w_int(&siod_view()->fnSIOpause[zone_idx],SSSS, 0);
            lcdbuf[Ad_Buf++]=')';
        }

        Ad_Buf = Str4;
        w_txt(Mes133); /* Ход клап */ //Boiler val time~~
        w_int(&hand_ctr->Position, bS, 0x01);
        BlkW = 1;
        Ad_Buf = Str5;
        w_txt(Mes140);// Ad_Buf++;
        w_int(&gd()->MechConfig[zone_idx].RNum[contour_idx],SpSSpSS, 0);


        return;
    }
    x_menu = 0;
}


/* ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  Настройка датчиков  ==  ==  ==  ==  ==  ==  ==  ==
        Если включен режим МЕНЮ, то выводим запомненный потенциал
        и измеренное значение раствора
        При режиме ВВОД выводится измеренный потенциал
        и значение калибровочного раствора
        При окончании ввода(EndInput = 1) запоминаем
        новые значения потенциала и калибровочного раствора
        при условии если крутизна характеристики не изменилась более
        20%(или 30% 13.04.96)
*/
void pmCalibr(void) {
    w_txt(Mes148); //Sensor calibration
    if (!x_menu) return;

/*    if(x_menu>=(SumTeplZones+2))
    {
        x_menu = SumTeplZones+2;
        CalPort = 1;
        Ad_Buf = Str2;
        Y_menu2%=3;
        w_txt(Mes147);
        Ad_Buf = Str3;
        w_txt(Mes149);
        w_int(&nPortSave,SS, 0);
        if ((nPortSave<1)||(nPortSave>4))  nPortSave = 1;
        if (!Y_menu2) BlkW = 1;
        w_txt(Mes157);
        w_int(&SaveChar,oS, 0);
        if ((!SaveChar)||(SaveChar>8))  SaveChar = 1;
        if (Y_menu2<2) BlkW = 1;
        else
            if (EndInput)
            {
                EndInput = 0;
                gd_get()->Cal.Port=(int)((long)Mes*(long)1000/(long)SaveInt);
                SizeEEP = 2;
                AdrRAM=&gd_get()->Cal.Port;
                SetInSaveRam();
            }
        Ad_Buf = Str4;
        w_txt(Mes158);
        if(!Menu)
            SaveInt=(uint16_t)((long)Mes*(long)1000/(long)(gd_get()->Cal.Port));
        w_int(&SaveInt,SSSS, 0);
        w_txt(Mes150);
        BlkW = 1;
        w_txt(Mes152);
        w_int(&gd_get()->Cal.Port,SpSSS, 0);
        Ad_Buf = Str5;
        w_txt(Mes151);
        w_int(&Mes,SSSS, 0);




        return;
    }
    */

    int byte_x = 0;
    int byte_y = 0;
    int byte_z = 0;

    int int_x = 0;
    int int_y = 0;
    int byte_w = 0;

    Ad_Buf = Str2;
    if (x_menu>=(SumTeplZones+1))
    {
        x_menu = SumTeplZones+1;
        w_txt(Mes159);
        Y_menu2%=(cConfSMetSens*5);
        byte_x = Y_menu2/5;        //номер датчика в общих с 0
        int_y = byte_x+NZONES*cConfSSens;           //номер в массиве калибровок
        int_x = gd()->Hot.MeteoSensing[byte_x].Value;
        gd_rw()->TControl.TimeMeteoSensing[byte_x]=120;

        byte_z = byte_x+1;          //номер датчика  с 1
        byte_x+=cConfSSens;  //номер датчика после тепличных в общих именах с 0
    }
    else
    {
        w_txt(Mes7);
        byte_w = x_menu-1;     //номер теплицы с 0
        w_int(&x_menu,oS, 0);
        Y_menu2%=(cConfSSens*5);
        byte_x = Y_menu2/5;        //номер датчика в тепличных именах с 0
        byte_z = byte_x+1;          //номер датчика в тепличных с 1
        int_x = gd()->Hot.Zones[byte_w].IndoorSensors[byte_x].Value;
        gd_rw()->TControl.Zones[byte_w].TimeInTepl[byte_x]=120;
//		if (!ByteX)
        int_y = byte_x+byte_w*cConfSSens;    //номер в массиве калибровок
    }
//    if (eCS->nPort>10) {DigitSens();return;}
    Ad_Buf++;


    sensor_config_t *eCS=&caldata.IndoorSensors[0][int_y];
    w_txt(NameSensConfig[byte_x].Name);
    lcdbuf[Ad_Buf++]='=';
    w_int(&int_x,NameSensConfig[byte_x].Frm, 0);
//    PrintEd(NamesOfSens[ByteX].Ed);
    byte_y = Y_menu2%5;        //номер меню в датчике
    Ad_Buf = Str3;
    w_txt(Mes149);
    w_int(&eCS->type,SS, 0);
    //if(!eCS->Input) {Ad_Buf++; w_txt("<off>");return;}
    if (!(byte_y))
    {
        BlkW = 1;MaximVal = 32;
    }
    //if((eCS->Input > 10)) {DigitSens();return;}

    w_txt(Mes157);
    w_int(&eCS->output,SS, 0);
    if ((byte_y) == 1)
    {
        BlkW = 1;MinimVal = 1;MaximVal = 32;
    }
    w_txt(Mes156);
    w_int(&eCS->corr,SSS, 0);
    if ((byte_y) == 2)
    {
        BlkW = 1;MinimVal = 0;MaximVal = 255;
    }
    Ad_Buf++;
    w_txt(Mes158);

    #warning "just output fbd input here"

    w_int(&sensdata.uIndoorSensors[0][int_y],SSSS, 0);
    w_txt(Mes150);

    if ((EndInput)&&(byte_y>=3))
    {   //конец ввода и ввели эталон
        byte_z=(byte_y-3);            //номер эталона
        if (!byte_z)
        {
            eCS->v1+=(SaveInt-eCS->v0);
            eCS->u1+=(sensdata.uIndoorSensors[0][int_y] - eCS->u0);
            eCS->v0 = SaveInt;
            eCS->u0 = sensdata.uIndoorSensors[0][int_y];
        }
        else
        {
            eCS->v1 = SaveInt2;
            eCS->u1 = sensdata.uIndoorSensors[0][int_y];
        }
        SetInSaveRam(eCS, 12);
        EndInput = 0;
    }
    Ad_Buf = Savebuf;

    if (!wtf0.Menu)
    {
        SaveInt = eCS->v0;
        SaveInt2 = eCS->v1;
    }
    Ad_Buf = Str4;
    w_txt(Mes154);
    lcdbuf[Ad_Buf++]='(';
    w_int(&eCS->u0,SSSS, 0);
    w_txt(Mes150);
    lcdbuf[Ad_Buf++]=')';
    w_int(&SaveInt,NameSensConfig[byte_x].Frm, 0);
    if (byte_y == 3) BlkW = 1;
    Ad_Buf = Str5;
    w_txt(Mes155);
    lcdbuf[Ad_Buf++]='(';
    w_int(&eCS->u1,SSSS, 0);
    w_txt(Mes150);
    lcdbuf[Ad_Buf++]=')';
    w_int(&SaveInt2,NameSensConfig[byte_x].Frm, 0);
    return;
}

// ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  Параметры мехнаизмов  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==
void pmParMechanic(void) {
    w_txt(Mes211); //Condition
    x_menu%=(SumTeplZones+1);
    if (!x_menu) return;
    Y_menu2%=cSRegCtrl*4;//cSMech;

    int byte_x =  x_menu-1;
    Ad_Buf = Str2;
    w_txt(Mes7);
    w_int(&x_menu,oS, 0);
    int byte_y = Y_menu2/4;
    Ad_Buf++;
    w_txt(NameOutputConfig[byte_y].Name);
    Ad_Buf = Str3;
    w_txt(Mes135);
    w_int(&gd()->ConstMechanic[byte_x].ConstMixVal[byte_y].v_TimeMixVal,SSSi, 0);
    w_txt(Mes96);
    if (!(Y_menu2%4)) BlkW = 1;
    Ad_Buf = Str4;
    w_txt(Mes97);
    w_int(&gd()->ConstMechanic[byte_x].ConstMixVal[byte_y].v_PFactor,SpSSS, 0);
    if ((Y_menu2%4) == 1) BlkW = 1;
    w_txt(Mes98);
    w_int(&gd()->ConstMechanic[byte_x].ConstMixVal[byte_y].v_IFactor,SpSSS, 0);
    if ((Y_menu2%4) == 2) BlkW = 1;
    Ad_Buf = Str5;
    w_txt(Mes99);
    w_int(&gd()->ConstMechanic[byte_x].ConstMixVal[byte_y].v_Type,SSS, 0);
    BlkW = 1;
    return;
}

// ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  Состояние  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==  ==
void pmNow(void) {
    uint8_t Cond;
    uint8_t Err;
    uint8_t Failure;
    uint8_t MaxIn;
    uint16_t CpM;


    w_txt(Mes38); //Condition
    x_menu%=(SumTeplZones*3+2);

    if (!x_menu) return;
    if (x_menu == SumTeplZones*3+1)
    {
        Form = 0;

        board_t *b = fbd_next_board(NULL);

        for (uint i = 0; b && i < Y_menu2; i++)
            b = fbd_next_board(b);

        if (! b)
        {
            Y_menu2 = 0;
            return;
        }

        CpM = fbd_get_addr(b);
        const board_stat_t *stat = fbd_get_stat(b);

        Err = stat->err_cnt;
        Failure = stat->reset_cnt;
        Cond = stat->permanent_errs;
        #warning "not implemented"
        MaxIn = -1;

        Ad_Buf = Str2;
        w_txt("Modul ");
        w_int(&Y_menu2,SS, 0);
        w_txt(" num:");
        w_int(&CpM,SSSi, 0);
        Ad_Buf = Str3;
        w_txt("Cond:");
        w_int(&Cond,SSS, 0);
        w_txt(" Err:");
        w_int(&Err,SSS, 0);
        Ad_Buf = Str4;
        w_txt("Max Input:");
        w_int(&MaxIn,SSS, 0);
        Ad_Buf++;
        w_txt("Fails:");
        w_int(&Failure,SSS, 0);


        return;
    }
    if (x_menu>SumTeplZones)
    {
//      int byte_y=(x_menu-SumTeplZones-1)/2+1;
//      int byte_z=(x_menu-SumTeplZones-1)%2+1;
//      Y_menu2%=N_MAX_MODULES;//cSMech;
//      Form = 0;
//      Ad_Buf = Str2;
//      w_txt("Fans zone ");
//      w_int(&byte_y,oS, 0);
//      lcdbuf[Ad_Buf++]='-';
//      w_int(&byte_z,oS, 0);
//      Ad_Buf = Str3;
//      for (int byte_x = 0;byte_x<MAX_FAN_COUNT;byte_x++)
//      {
//          if (byte_x == 32)
//              Ad_Buf = Str4;
//          if (gd()->FanBlock[x_menu-SumTeplZones-1][0].FanData[byte_x].actual)
//          {
//
//              lcdbuf[Ad_Buf++]='0';
//          }
//          else
//              lcdbuf[Ad_Buf++]='1';
//
//      }
/*        Ad_Buf = Str4;
        for (ByteX = 0;ByteX<MAX_FAN_COUNT;ByteX++)
        {
            if (ByteX == 32)
                Ad_Buf = Str5;
            if (gd_get()->FanBlock[x_menu-SumTeplZones-1][1].FanData[ByteX].Actual)
            {

                lcdbuf[Ad_Buf++]='0';
            }
            else
            lcdbuf[Ad_Buf++]='1';

        }
//    	w_txt("05/02");*/
        return;
    }
    int byte_y = x_menu-1;
    Y_menu2 = 1;//cSMech;

    Ad_Buf = Str2;
    w_txt(Mes7);
    w_int(&x_menu,oS, 0);
    Ad_Buf++;
    w_txt(Mes39);
    Ad_Buf = Str3;
    w_txt(" T  | RH |CO2 |Tp1 |Tp2 |Tp3 |Tp4 |Tp5");
    Ad_Buf = Str4;
    w_int(&gd()->Hot.Zones[byte_y].IndoorSensors[cSmTSens1].Value,SSpS0, 0);
    w_int(&gd()->Hot.Zones[byte_y].IndoorSensors[cSmTSens2].Value,SSpS0, 0);
    w_int(&gd()->Hot.Zones[byte_y].IndoorSensors[cSmTSens3].Value,SSpS0, 0);
    w_int(&gd()->Hot.Zones[byte_y].IndoorSensors[cSmTSens4].Value,SSpS0, 0);
    lcdbuf[Ad_Buf++]='|';
    w_int(&gd()->Hot.Zones[byte_y].IndoorSensors[cSmRHSens].Value,SSpS0, 0);
    lcdbuf[Ad_Buf++]='|';
    w_int(&gd()->Hot.Zones[byte_y].IndoorSensors[cSmCOSens].Value,SSSS, 0);
    for (int byte_x = 0;byte_x<cSWaterKontur-1;byte_x++)
    {
        lcdbuf[Ad_Buf++]='|';
        int int_x = gd()->Hot.Zones[byte_y].IndoorSensors[cSmWaterSens+byte_x].Value/10;
        w_int(&int_x,SSS, 0);
        Ad_Buf++;
    }
    Ad_Buf = Str5;
    w_int(&gd()->Hot.Zones[byte_y].AllTask.DoTHeat,SSpS0, 0);
    lcdbuf[Ad_Buf++]='|';
    w_int(&gd()->Hot.Zones[byte_y].AllTask.DoRHAir,SSpS0, 0);
    lcdbuf[Ad_Buf++]='|';
    w_int(&gd()->Hot.Zones[byte_y].AllTask.DoCO2,SSSS, 0);
    for (int byte_x = 0;byte_x<cSWaterKontur-1;byte_x++)
    {
        lcdbuf[Ad_Buf++]='|';
        int int_x = gd()->Hot.Zones[byte_y].Kontur[byte_x].Do/10;
        w_int(&int_x,SSS, 0);
        Ad_Buf++;
    }
    Form = 0;
    return;
}

void    pmProgClimate(void)
{
    w_txt(Mes34);
    //w_int(airHeatTimeWork[0],SSS, 0);

    if (x_menu > cSTimer) x_menu = 0;
    if (!x_menu)
    {
        return;
    }
    Ad_Buf = Str2;
    int byte_z = x_menu-1;
    if (Y_menu2 > SUM_NAME_TIMER) Y_menu2 = 0;
    if (Y_menu2 >=SUM_NAME_TIMER) Y_menu2 = SUM_NAME_TIMER-1;
    w_txt(Mes35); //-Correction-
    w_int(&x_menu,SS, 0);
    Ad_Buf = Str3;
    if (Y_menu2 < StartY_menu2) StartY_menu2 = Y_menu2;
    if (Y_menu2 > (StartY_menu2+2)) StartY_menu2 = Y_menu2-2;
    for (int byte_y = StartY_menu2;byte_y < (StartY_menu2+3);byte_y++)
    {
        int byte_x = byte_y % SUM_NAME_TIMER;
        w_txt(NameTimer[byte_x].Name);
        Ad_Buf=(Ad_Buf / DisplCols)*DisplCols+20;
        lcdbuf[Ad_Buf++]='=';
        w_int(&gd()->Timers[byte_z].Zone[NameTimer[byte_x].Index],NameTimer[byte_x].Frm, 0);
        if (Y_menu2  ==  byte_y) BlkW = 1;
        Ad_Buf=((Ad_Buf / DisplCols)+1)*DisplCols;
    }
    return;
}

void pmVersion(void)
{
    if ((Y_menu2%4) == 2) BlkW = 1;
    w_txt(Mes223);
    if (x_menu > cSTimer) x_menu = 0;
    if (!x_menu)
    {
        return;
    }
    Ad_Buf = Str2;

    if (Y_menu2 > SUM_NAME_TIMER) Y_menu2 = 0;
    if (Y_menu2 >=SUM_NAME_TIMER) Y_menu2 = SUM_NAME_TIMER-1;

    w_txt(FWVersion);
    Ad_Buf = Str3;
    return;
}

void YMenu(char vPozY) {
    char pozY;
    pozY = vPozY;
    if (!pozY)
    {
        pmProgClimate();  return;
    }
    if (!(--pozY))
    {
        pmParam();    return;
    }
    if (!(--pozY))
    {
        pmStrategy();     return;
    }
    if (!(--pozY))
    {
        pmHand();     return;
    }
    if (!(--pozY))
    {
        pmCalibr();   return;
    }
    if (!(--pozY))
    {
        pmNow();      return;
    }
    if (!(--pozY))
    {
        pmParMechanic();  return;
    }
    // NEW
    if (!(--pozY))
    {
        pmVersion();  return;
    }
    //----
    SumYMenu = vPozY-pozY;
    if (Y_menu > SumYMenu) Y_menu = SumYMenu;
}

void reset_menu(void)
{
    Y_menu=0;
    x_menu=0;
}
