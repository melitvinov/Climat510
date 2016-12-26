//================= ПРОЕКТ 403 =====================
/*-----------------------------------------------
        Вывод даты и времени
------------------------------------------------*/
#ifdef SetTest
#include "405_Test.c"
#endif

#define	MaxY_menu		9

#define SUM_NAME_TUNE sizeof(NameConst)

uchar	StartMenuY;
uchar	NowCurRow,NewCurRow;
int		SaveSample,Savebuf;
//char	ToLowTime, ToHiTime;
uchar	StartY_menu2;
eCalSensor xdata *eCS;
void YMenu(char pozY);

uchar code DayOfWeek[][10]=
{"Mon#\276o\275e\343","Tue#\263\277o\310\275","Wed#c\310e\343a","Thu#\300e\277\263","Fri#\276\307\277\275","Sat#c\311\262\262","Sun#\263oc\272\310"};


//-------------------- рассчет дня недели --------------------------


void GMenu(void) {
uchar	numRow;
        //NowDigCal=0;
		if(GrafView) return;
		clear_d();
        Form=0;
        Ad_Buf=0;
		AdinB=0;
		CalPort=0;
        ClrDog;
		if(Y_menu > MaxY_menu) Y_menu=MaxY_menu;
//***************************************************
//---- Если в крайнем столбце -----
		if(!x_menu) {
			if (Y_menu > (StartMenuY+5)) StartMenuY=Y_menu-5;
			if (Y_menu < StartMenuY) StartMenuY = Y_menu;
//---- Вывести SUM_LINE_DISP-2 строк главного меню  -----
			for (numRow=0;numRow < (SUM_LINE_DISP-2);numRow++) {
				Ad_Buf=numRow*DisplCols;
				YMenu(numRow+StartMenuY);
				}
			NewCurRow=Y_menu - StartMenuY + 1;
			buf[NewCurRow*DisplCols-19]='\312';
//			buf[NewCurRow*DisplCols-18]='\312';
			if(Second & 1) buf[NewCurRow*DisplCols-18]='\312';	//'\076';
				else buf[NewCurRow*DisplCols-20]='\312';
			SendBlock(&buf[Str2d],TxtHomeAddr+Str2d,23);   //send tu str
//---- Вывести инверсный фон -----
			if (NewCurRow !=NowCurRow) {
//				SetRow(0xFF,NewCurRow);
//				if(NowCurRow) SetRow(0x00,NowCurRow);
				NowCurRow=NewCurRow;
				}
			return;
			} else  NewCurRow = 0;
//---- Стереть фон -----
    	if(NewCurRow != NowCurRow) {
//			if(NowCurRow) SetRow(0x00,NowCurRow);
			NowCurRow=NewCurRow;
//			if (x_menu) SendSim(Tire,1);
			}
//-----------------------
//=========================================================
        if(Menu&&(GD.TControl.NowCod!=GD.Control.Cod))
              if(SaveChar!=127)
                {
				w_txt(Mes65); //Access code~
//				w_txt(Mes1);
                w_int(&GD.TControl.NowCod,SSS);
				SaveChar=126;
                return;
                }
		YMenu(Y_menu);
		}

void pmReset(void) {
//char nBlk;
    Y_menu2%=5;
    Ad_Buf=Str2;
    w_txt(Mes66);   // Задания //Init Tasks~
    w_int(&BlockEEP[1].Erase,oS);
	if(Y_menu2==0) {BlkW=1; nBlEEP=1;}
    Ad_Buf++;
    w_txt(Mes221);   // <Input 1 for init>
    Ad_Buf=Str3;
    w_txt(Mes67); // Калибровки  //Init Calibrations~
    w_int(&BlockEEP[5].Erase,oS);
	if(Y_menu2==1) {BlkW=1; nBlEEP=5;}
    Ad_Buf=Str4;
    w_txt(Mes68);   // Параметры  //Init Parameters~
    w_int(&BlockEEP[0].Erase,oS);
	if(Y_menu2==2) {BlkW=1; nBlEEP=0;}
    Ad_Buf=Str5;
	if(Y_menu2<=3) {
		if(Y_menu2==3) nBlEEP=4;
    	w_txt(Mes69);   // Конфигурация  //Init Configuration~
    	w_int(&BlockEEP[4].Erase,oS);
		}
	else {
		if(Y_menu2==4) nBlEEP=3;
    	w_txt(Mes222);   // Init Strategy~
    	w_int(&BlockEEP[3].Erase,oS);
		}
	if(EndInput && (BlockEEP[nBlEEP].Erase == 1)) {
			EndInput=0;
			TestMem(nBlEEP+10);
//			RamToEEP(nBlEEP);
			}
}


//=========== СОСТОЯНИЕ ПОЛИВА В ПОСЛЕДНИЙ КАДР =======================
//char Proces,Proces2;
void pmInfoProg405(void){
	BlkW=1;
	ByteY=(Second/6)%GD.Control.ConfSTepl;
	ByteZ=(Second/2)%3;
	w_txt(Mes7); //Zone
	ByteX=ByteY+1;
    w_int(&ByteX,SS);
	Ad_Buf++;
//	IntX=GD.Hot.MidlSR;
//	if (IntX<0) IntX=-IntX;
//	w_int(&IntX,SSSS);
	w_int(&GD.FanBlock[0][0].FanData[0].ActualSpeed,SSSS);

	if (!ByteZ)
	{
		w_txt(Mes40);
		w_int(&GD.Hot.Tepl[ByteY].AllTask.DoTHeat,SSpS0);
		Ad_Buf++;
		w_txt(Mes41);
		w_int(&GD.Hot.Tepl[ByteY].AllTask.DoTVent,SSpS0);
		Ad_Buf++;
		w_txt(Mes42);
		w_int(&GD.Hot.Tepl[ByteY].InTeplSens[cSmTSens1].Value,SSpS0);
		w_int(&GD.Hot.Tepl[ByteY].InTeplSens[cSmTSens2].Value,SSpS0);
		w_int(&GD.Hot.Tepl[ByteY].InTeplSens[cSmTSens3].Value,SSpS0);
		w_int(&GD.Hot.Tepl[ByteY].InTeplSens[cSmTSens4].Value,SSpS0);
		return;
	}
	if (ByteZ==1)
	{
		w_txt(Mes43);
		w_int(&GD.Hot.Tepl[ByteY].AllTask.DoRHAir,SSpS0);
		Ad_Buf++;
		w_txt(Mes44);
		w_int(&GD.Hot.Tepl[ByteY].InTeplSens[cSmRHSens].Value,SSpS0);
		return;
	}
	w_txt(Mes45);
	w_int(&GD.Hot.Tepl[ByteY].AllTask.DoCO2,SSSS);
 	Ad_Buf++;
	w_txt(Mes46);
	w_int(&GD.Hot.Tepl[ByteY].InTeplSens[cSmCOSens].Value,SSSS);


}


void pmDate(void) {
   Ad_Buf=Str2;
    w_txt(Proect);
    w_int(&GD.Control.rModification,SS);
	buf[Ad_Buf++]='-';
    w_int(&GD.Control.rSInTeplSens,SS);
//	w_txt("-M");
//    w_int(&GD.Config[cfSumTap],oS);
//	w_txt("-V");
//    w_int(&GD.Config[cfSumIrrigVal],SS);
    Ad_Buf=Str3;
    w_txt(Mes70); //Time ~
    w_int(&CtrTime,SSdSS);
    buf[Ad_Buf++]=':';
    buf[Ad_Buf++]=Second/10+'0';
    buf[Ad_Buf++]=Second%10+'0';
    if(!Y_menu2) BlkW=1;
    Ad_Buf=Str4;
    ClrDog;
    w_txt(Mes71); //Date~
    w_int(&CtrData,DsMsY);
    Ad_Buf++;
    ByteY=NowDayOfWeek;
	for(ByteX=0;ByteX < 7; ByteX++) {
			if(ByteY&64) break;
			ByteY<<=1;
			}
	if (ByteX <7) w_txt(&DayOfWeek[ByteX][0]);	  //&
    if (EndInput) {
        EndInput=0;
        if (CtrTime>=24*60) CtrTime=1;
//		CalcDayOfWeek();
        SetRTC();
        }

}

void pmStrategy(void) {
                w_txt(Mes72); //Irrigation archive
				if (x_menu>cSWaterKontur) x_menu=0;
                if(!x_menu)	return;
				Y_menu2%=3;
				Ad_Buf=Str2;
				w_txt(Mes76);
				w_int(&x_menu,SS);
				Ad_Buf=Str3;
				w_txt(Mes73);
				w_int(&GD.Strategy[0][x_menu-1].TempPower,SS);
				if (!Y_menu2) BlkW=1;
				Ad_Buf=Str4;
				w_txt(Mes74);
				w_int(&GD.Strategy[0][x_menu-1].RHPower,SS);
				if (Y_menu2==1) BlkW=1;
				Ad_Buf=Str5;
				w_txt(Mes75);
				w_int(&GD.Strategy[0][x_menu-1].OptimalPower,SS);
				return;
}

void pmParam() {
                char* OutStr;
				w_txt(Mes85); //Parameters
                if(!x_menu) return;
                ByteZ=x_menu-1;
                Ad_Buf=Str2;
                if(!ByteZ){pmDate(); return;}
				ByteZ--;
                if(ByteZ<SumTeplZones){
                        if (Y_menu2 > SUM_NAME_CONF) Y_menu2=0;
                        if (Y_menu2 >= SUM_NAME_CONF) Y_menu2=SUM_NAME_CONF-1;
                        w_txt(Mes86); //-Configuration-
						Ad_Buf++;
						w_txt(Mes7);
						Ad_Buf++;
						ByteX=ByteZ+1;
						w_int(&ByteX,SS);
                        Ad_Buf=Str3;
						if(Y_menu2 < StartY_menu2) StartY_menu2 = Y_menu2;
						if(Y_menu2 > (StartY_menu2+2)) StartY_menu2 = Y_menu2-2;
                        for (ByteY=StartY_menu2;ByteY < (StartY_menu2+3);ByteY++) {
							ByteX=ByteY % SUM_NAME_CONF;
							OutStr=NameOutputConfig[ByteX].Name;
							ByteW=ByteX;
							if (ByteX>=SUM_NAME_INPUTS)
								OutStr=NameInputConfig[ByteX-SUM_NAME_INPUTS].Name;
							if (ByteX>=SUM_NAME_INSENS)
								OutStr=NameSensConfig[ByteX-SUM_NAME_INSENS].Name;
                        	w_txt(OutStr);
							Ad_Buf=(Ad_Buf / DisplCols)*DisplCols+20;
                        	buf[Ad_Buf++]=':';
	                        w_int(&GD.MechConfig[ByteZ].RNum[ByteW],SpSSpSS);
							if(Y_menu2 == ByteY) BlkW=1;
 							//w_txt(Mes87); //-rela\321
							Ad_Buf=((Ad_Buf / DisplCols)+1)*DisplCols;
							}
						if (!GD.TControl.NowCod) Form=0;
                        return;}
				ByteZ-=SumTeplZones;
                if(ByteZ<SumTeplZones){
                        if (Y_menu2 > cConfSSystem) Y_menu2=0;
                        if (Y_menu2 >= cConfSSystem) Y_menu2=cConfSSystem-1;
                        w_txt(Mes86); //-Configuration-
						Ad_Buf++;
						w_txt(Mes7);
						Ad_Buf++;
						ByteX=ByteZ+1;
						w_int(&ByteX,SS);
                        Ad_Buf=Str3;
						if(Y_menu2 < StartY_menu2) StartY_menu2 = Y_menu2;
						if(Y_menu2 > (StartY_menu2+2)) StartY_menu2 = Y_menu2-2;
                        for (ByteY=StartY_menu2;ByteY < (StartY_menu2+3);ByteY++) {
							ByteX=ByteY % SUM_NAME_CONF;
							OutStr=NameSystemConfig[ByteX].Name;
							ByteW=ByteX;
                        	w_txt(OutStr);
							Ad_Buf=(Ad_Buf / DisplCols)*DisplCols+20;
                        	buf[Ad_Buf++]=':';
	                        w_int(&GD.MechConfig[ByteZ].Systems[ByteW],SSSi);
							if(Y_menu2 == ByteY) BlkW=1;
 							//w_txt(Mes87); //-rela\321
							Ad_Buf=((Ad_Buf / DisplCols)+1)*DisplCols;
							}
						if (!GD.TControl.NowCod) Form=0;
                        return;}
				ByteZ-=SumTeplZones;
				if(ByteZ<SumTeplZones){
		                if(Y_menu2 > SUM_NAME_PARS) Y_menu2=0;
		                if(Y_menu2 >=SUM_NAME_PARS) Y_menu2=SUM_NAME_PARS-1;
                        w_txt(Mes90); //-Correction-
						Ad_Buf++;
						w_txt(Mes7);
						Ad_Buf++;
						ByteX=ByteZ+1;
						w_int(&ByteX,SS);
                        Ad_Buf=Str3;
						if(Y_menu2 < StartY_menu2) StartY_menu2 = Y_menu2;
						if(Y_menu2 > (StartY_menu2+2)) StartY_menu2 = Y_menu2-2;
                        for (ByteY=StartY_menu2;ByteY < (StartY_menu2+3);ByteY++) {
							ByteX=ByteY % SUM_NAME_PARS;
                			w_txt(NameParUpr[ByteX].Name);
							Ad_Buf=(Ad_Buf / DisplCols)*DisplCols+20;
							buf[Ad_Buf++]='=';
	                        w_int(&GD.Control.Tepl[ByteZ].c_MaxTPipe[ByteY],NameParUpr[ByteX].Ed);
							if(Y_menu2 == ByteY) BlkW=1;
							Ad_Buf=((Ad_Buf / DisplCols)+1)*DisplCols;

							}
						return;}
				ByteZ-=SumTeplZones;
                if(!ByteZ){  													// Уставки
		                if(Y_menu2 > sizeof(NameConst))//SUM_NAME_TUNE)
		                	Y_menu2=0;
		                if(Y_menu2 >= sizeof(NameConst))//SUM_NAME_TUNE)
		                	Y_menu2=sizeof(NameConst)-1;//SUM_NAME_TUNE-1;
						w_txt(Mes91);
						Ad_Buf=Str3;
						if(Y_menu2 < StartY_menu2) StartY_menu2 = Y_menu2;
						if(Y_menu2 > (StartY_menu2+2)) StartY_menu2 = Y_menu2-2;
                        for (ByteY=StartY_menu2;ByteY < (StartY_menu2+3);ByteY++) {
							ByteX=ByteY % SUM_NAME_TUNE;
                			ByteW=ByteX+1;
							w_txt(Mes92);
							w_int(&ByteW,SS);
							Ad_Buf=((Ad_Buf / DisplCols))*DisplCols+20;
							buf[Ad_Buf++]='=';
	                        w_int(&GD.TuneClimate.s_TStart[ByteY],NameConst[ByteX].Frm);
							if(Y_menu2 == ByteY)
								BlkW=1;
							Ad_Buf=((Ad_Buf / DisplCols)+1)*DisplCols;

							}
                        return; }
				ByteZ--;  //if (!GD.Config[cfReturn1Val] && !GD.Config[cfRegulRetEC]) 
                if(!ByteZ)
				{
		                Y_menu2%=8;
                        w_txt(Mes126); //Controller num~
                        w_int(&GD.Control.NFCtr,SS);
                        if(!Y_menu2) BlkW=1;
						if(!GD.Hot.News) w_txt(" on line");
                        Ad_Buf=Str3;
                        w_txt(Mes127); //Language~
                        w_int(&GD.Control.Language,oS);
                       // w_int(&PowerOfLineRS,SSSS);
                        if(Y_menu2==1) BlkW=1;

                        Ad_Buf=Str4;
                        w_txt(Mes65); //Access code~
                        w_int(&GD.Control.Cod,SSS);
						SaveChar=127;
						if (!Menu) SaveChar=0;
                        if(Y_menu2==2) BlkW=1;
                       	Ad_Buf=Str5;
						if (Y_menu2<=3) 
						{
                        	w_txt(Mes5);
                        	w_int(&GD.Control.ConfSTepl,oS);
						}
						else
						{
                        	w_txt("IP:");
                        	w_int(&GD.Control.IPAddr[0],SSS);
                            if(Y_menu2==4) BlkW=1;
                        	w_txt(".");
                        	w_int(&GD.Control.IPAddr[1],SSS);
                            if(Y_menu2==5) BlkW=1;
                        	w_txt(".");
                        	w_int(&GD.Control.IPAddr[2],SSS);
                            if(Y_menu2==6) BlkW=1;
                        	w_txt(".");
                        	w_int(&GD.Control.IPAddr[3],SSS);

							//w_txt(Mes206);
                        	//w_int(&GD.Control.Screener,SSS);
						}
                       return;
				}
                if(ByteZ > 1)	x_menu=0;
				pmReset();                
}
void AutoMan(char RCS,int fint) 
{
	if (YesBit(RCS,fint)) 
      w_txt(Mes139);
	else
      w_txt(Mes138);
}
//====================== ручное управление ====================
void pmHand(void) {
                w_txt(Mes128); //Ручное управление //Manual control~
                if(!x_menu)  return;
                Ad_Buf=Str2;
                ByteZ=x_menu-1;
                if(ByteZ<SumTeplZones){  					/* Клапан полива N */
						SetPointersOnTepl(ByteZ);	
				        while (Y_menu2< cSRegCtrl*2)
						{
							ByteW=Y_menu2/2;
							if (pGD_MechConfig->RNum[ByteW]) break;
							Y_menu2++;
						}
                        if(Y_menu2 >= cSRegCtrl*2) {Y_menu2=0;w_txt(Mes89); return;}

						SetPointersOnKontur(ByteW);	
                        Ad_Buf=Str2;
						w_txt(Mes7);
						Ad_Buf++;
						ByteX=ByteZ+1;
						w_int(&ByteX,SS);
						Ad_Buf++;
                        w_txt(NameOutputConfig[ByteW].Name); /* Клап бойлера*/ //Boiler val \310ost~
                        Ad_Buf=Str3;
                        w_txt(Mes134); /* Ход клап */ //Boiler val time~~
						ByteX=0x01;
                        w_int(&pGD_Hot_Hand_Kontur->RCS,bS);
						AutoMan(pGD_Hot_Hand_Kontur->RCS,ByteX);
                        if (!(Y_menu2%2)) BlkW=1;
                        Ad_Buf=Str4;
                        w_txt(Mes133); /* Ход клап */ //Boiler val time~~
                        w_int(&pGD_Hot_Hand_Kontur->Position,SSS);
                        buf[Ad_Buf++]='%';
                        BlkW=1;
						buf[Ad_Buf++]='(';
                  	  	w_int(&pGD_TControl_Tepl->MechBusy[ByteW].TimeRealMech,SSSi);
				  	  	w_txt("s) #c) ");
				  	  	if  (pGD_TControl_Tepl->MechBusy[ByteW].Sens)
				  	  	{
	                        buf[Ad_Buf++]='(';
	                  	  	w_int(&pGD_TControl_Tepl->MechBusy[ByteW].Sens->Value,SSSpS);
	                        buf[Ad_Buf++]=')';
				  	  	}
 			  	  	    Ad_Buf=Str5;
						w_txt(Mes136); Ad_Buf++;
                        IntX=(*pGD_MechConfig_Kontur);
                        w_int(&IntX,SpSSpSS);


                        return;
                		}
				ByteZ-=SumTeplZones;
                if(ByteZ<SumTeplZones){
						SetPointersOnTepl(ByteZ);	
				        while (Y_menu2< cSDiskrCtrl*2)
						{
							ByteW=cSRegCtrl+Y_menu2/2;
							if (pGD_MechConfig->RNum[ByteW]) break;
							Y_menu2++;
						}
                        if(Y_menu2 >= cSDiskrCtrl*2) {Y_menu2=0;w_txt(Mes89); return;}
						SetPointersOnTepl(ByteZ);
	
						SetPointersOnKontur(ByteW);	
                        Ad_Buf=Str2;
						w_txt(Mes7);
						Ad_Buf++;
						ByteX=ByteZ+1;
						w_int(&ByteX,SS);
						Ad_Buf++;
                        w_txt(NameOutputConfig[ByteW].Name); /* Клап бойлера*/ //Boiler val \310ost~
                        Ad_Buf=Str3;
                        w_txt(Mes134); /* Ход клап */ //Boiler val time~~
						ByteX=0x01;
                        w_int(&pGD_Hot_Hand_Kontur->RCS,bS);
						AutoMan(pGD_Hot_Hand_Kontur->RCS,ByteX);
                        if (!(Y_menu2%2)) BlkW=1;

                        if (ByteW == cHSmSIOPump)
                        {
                        	buf[Ad_Buf++]='(';
                  	  		w_int(&fnSIOfaza[ByteZ],SS);
                  	    	buf[Ad_Buf++]=',';
                  	    	w_int(&fnSIOvelvOut[ByteZ],SSSS);
                  	    	buf[Ad_Buf++]=',';
                  	    	w_int(&fnSIOpumpOut[ByteZ],SSSS);
                  	    	buf[Ad_Buf++]=',';
                  	    	w_int(&fnSIOpause[ByteZ],SSSS);
                  	    	buf[Ad_Buf++]=')';
                        }

                        Ad_Buf=Str4;
                        w_txt(Mes133); /* Ход клап */ //Boiler val time~~
                        w_int(&pGD_Hot_Hand_Kontur->Position,bS);
                        BlkW=1;
						Ad_Buf=Str5;
						w_txt(Mes140);// Ad_Buf++;
                        IntX=(*pGD_MechConfig_Kontur);
                        w_int(&IntX,SpSSpSS);


						return;
                		}
                x_menu=0;
}

void DigitSens(void) {
//                  NowDigCal=ByteX;
                  w_txt(Mes144); //(digital)
                  Ad_Buf=Str5;
                  //Y_menu2%=3;
				  switch (ds18b20_ReadROM())
				  {
					case 1:
						w_txt(Mes145); //No sensor
						break;
					case 2:
						w_txt(Mes146); //Multi sensor
						break;
					case 0:
	                	w_txt("On-line:"); //On line:
						w_int(&Buf1W[0],StStStS);
				  		buf[Ad_Buf++]='-';
						w_int(&Buf1W[4],StStStS);
				  }

//                  Ad_Buf=Str5;
//				  w_txt("ResLong=");
//				  w_int(&ResLong,SSS);
//				  w_txt("Line long=");
//				  w_int(&ToHi,SSS);
                  Ad_Buf=Str4;
				  w_txt("Num:");
				  w_int(&eCS->U1,StStStS);
                  if (ByteY==1) BlkW=1;
				  buf[Ad_Buf++]='-';
				  w_int(&eCS->V1,StStStS);
}


/*======================== Настройка датчиков ================
        Если включен режим МЕНЮ, то выводим запомненный потенциал
        и измеренное значение раствора
        При режиме ВВОД выводится измеренный потенциал
        и значение калибровочного раствора
        При окончании ввода(EndInput=1) запоминаем
        новые значения потенциала и калибровочного раствора
        при условии если крутизна характеристики не изменилась более
        20%(или 30% 13.04.96)
*/
void pmCalibr(void) {
	w_txt(Mes148); //Sensor calibration
    if(!x_menu) return;

/*    if(x_menu>=(SumTeplZones+2))
	{
	    x_menu=SumTeplZones+2;
		CalPort=1;
		Ad_Buf=Str2;
        Y_menu2%=3;
        w_txt(Mes147);
		Ad_Buf=Str3;
		w_txt(Mes149);
        w_int(&nPortSave,SS);
        if ((nPortSave<1)||(nPortSave>4))  nPortSave=1;
        if (!Y_menu2) BlkW=1;
        w_txt(Mes157);
        w_int(&SaveChar,oS);
        if ((!SaveChar)||(SaveChar>8))  SaveChar=1;
        if (Y_menu2<2) BlkW=1;
        else  
			if (EndInput)
			{
        		EndInput=0;
                GD.Cal.Port=(int)((long)Mes*(long)1000/(long)SaveInt);
                SizeEEP=2;
                AdrRAM=&GD.Cal.Port;
                SetInSaveRam();
            }
        Ad_Buf=Str4;
		w_txt(Mes158);
		if(!Menu)
        	SaveInt=(uint16_t)((long)Mes*(long)1000/(long)(GD.Cal.Port));
        w_int(&SaveInt,SSSS);
        w_txt(Mes150);
        BlkW=1;
        w_txt(Mes152);
        w_int(&GD.Cal.Port,SpSSS);
        Ad_Buf=Str5;
        w_txt(Mes151);
        w_int(&Mes,SSSS);




        return;
	}
	*/
	Ad_Buf=Str2;
    if(x_menu>=(SumTeplZones+1))
	{
	    x_menu=SumTeplZones+1;
		w_txt(Mes159);
        Y_menu2%=(cConfSMetSens*5);
   		ByteX=Y_menu2/5;		//номер датчика в общих с 0
		IntY=ByteX+cSTepl*cConfSSens;			//номер в массиве калибровок
		IntX=GD.Hot.MeteoSensing[ByteX].Value;
		GD.TControl.TimeMeteoSensing[ByteX]=120;

		ByteZ=ByteX+1;			//номер датчика  с 1
   		ByteX+=cConfSSens;	//номер датчика после тепличных в общих именах с 0
	}
	else 
	{
       	w_txt(Mes7);
		ByteW=x_menu-1;		//номер теплицы с 0
		w_int(&x_menu,oS);
      	Y_menu2%=(cConfSSens*5);
        ByteX=Y_menu2/5;		//номер датчика в тепличных именах с 0
		ByteZ=ByteX+1;			//номер датчика в тепличных с 1
		IntX=GD.Hot.Tepl[ByteW].InTeplSens[ByteX].Value;
		GD.TControl.Tepl[ByteW].TimeInTepl[ByteX]=120;
//		if (!ByteX)
		IntY=ByteX+ByteW*cConfSSens;	//номер в массиве калибровок
	}
//    if (eCS->nPort>10) {DigitSens();return;}
	Ad_Buf++;
	eCS=&GD.Cal.InTeplSens[0][IntY];
    w_txt(NameSensConfig[ByteX].Name);
    buf[Ad_Buf++]='=';
    w_int(&IntX,NameSensConfig[ByteX].Frm);
//    PrintEd(NamesOfSens[ByteX].Ed);
	ByteY=Y_menu2%5;		//номер меню в датчике
	Ad_Buf=Str3;
    w_txt(Mes149);
    w_int(&eCS->Type,SS);
	//if(!eCS->Input) {Ad_Buf++; w_txt("<off>");return;}
    if (!(ByteY)) {BlkW=1;MaximVal=32;}
	//if((eCS->Input > 10)) {DigitSens();return;}

	w_txt(Mes157);
    w_int(&eCS->Output,SS);
    if ((ByteY)==1) {BlkW=1;MinimVal=1;MaximVal=32;}
	w_txt(Mes156);
    w_int(&eCS->Corr,SSS);
    if ((ByteY)==2) {BlkW=1;MinimVal=0;MaximVal=255;}
    Ad_Buf++;
	w_txt(Mes158);
    w_int(&GD.uInTeplSens[0][IntY],SSSS);
    w_txt(Mes150);

    if ((EndInput)&&(ByteY>=3))
	{	//конец ввода и ввели эталон	
    	ByteZ=(ByteY-3);			//номер эталона
    	if (!ByteZ) 
		{				
       		eCS->V1+=(SaveInt-eCS->V0);
       		eCS->U1+=(GD.uInTeplSens[0][IntY]-eCS->U0);
	       	eCS->V0=SaveInt;
    	   	eCS->U0=GD.uInTeplSens[0][IntY];
       	}
		else
		{
       		eCS->V1=SaveInt2;
       		eCS->U1=GD.uInTeplSens[0][IntY];
		}
    	SizeEEP=12;
    	AdrRAM=eCS;
    	SetInSaveRam();
       	EndInput=0;
	}
    Ad_Buf=Savebuf;
			
   	if(!Menu) 
	{
   		SaveInt=eCS->V0;
   		SaveInt2=eCS->V1;
    }
    Ad_Buf=Str4;
    w_txt(Mes154);
    buf[Ad_Buf++]='(';
    w_int(&eCS->U0,SSSS);
    w_txt(Mes150);
    buf[Ad_Buf++]=')';
	w_int(&SaveInt,NameSensConfig[ByteX].Frm);
	if (ByteY==3) BlkW=1;
    Ad_Buf=Str5;
    w_txt(Mes155);
    buf[Ad_Buf++]='(';
    w_int(&eCS->U1,SSSS);
    w_txt(Mes150);
    buf[Ad_Buf++]=')';
	w_int(&SaveInt2,NameSensConfig[ByteX].Frm);
    return;  
}

//==================== Параметры мехнаизмов ======================
void pmParMechanic(void) {                          
	w_txt(Mes211); //Condition
	x_menu%=(SumTeplZones+1);
    if (!x_menu) return;	
	Y_menu2%=cSRegCtrl*4;//cSMech;
    ByteX=x_menu-1;
    Ad_Buf=Str2;
	w_txt(Mes7);
	w_int(&x_menu,oS);
	ByteY=Y_menu2/4;
    Ad_Buf++;
    w_txt(NameOutputConfig[ByteY].Name);
	Ad_Buf=Str3;
	w_txt(Mes135);
    w_int(&GD.ConstMechanic[ByteX].ConstMixVal[ByteY].v_TimeMixVal,SSSi);
	w_txt(Mes96);
	if (!(Y_menu2%4)) BlkW=1;
	Ad_Buf=Str4;
	w_txt(Mes97);
    w_int(&GD.ConstMechanic[ByteX].ConstMixVal[ByteY].v_PFactor,SpSSS);
	if ((Y_menu2%4)==1) BlkW=1;
	w_txt(Mes98);
    w_int(&GD.ConstMechanic[ByteX].ConstMixVal[ByteY].v_IFactor,SpSSS);
	if ((Y_menu2%4)==2) BlkW=1;
	Ad_Buf=Str5;
	w_txt(Mes99);
    w_int(&GD.ConstMechanic[ByteX].ConstMixVal[ByteY].v_Type,SSS);
	BlkW=1;
	return;
}

//==================== Состояние ======================
void pmNow(void) {                          
	uint8_t Cond;
	uint8_t Err,Failure;
	uint8_t MaxIn;
	uint16_t CpM;
	uint16_t* InpVal;


	w_txt(Mes38); //Condition
	x_menu%=(SumTeplZones*3+2);

    if (!x_menu) return;
    if (x_menu==SumTeplZones*3+1)
    {
    	Y_menu2%=OUT_MODUL_SUM;//cSMech;
    	Form=0;
    	ModStatus(Y_menu2,&CpM,&Err,&Failure,&Cond,&MaxIn,&InpVal);
    	if (!CpM) {Y_menu2=0;return;}
        Ad_Buf=Str2;
        w_txt("Modul ");
        w_int(&Y_menu2,SS);
        w_txt(" num:");
        w_int(&CpM,SSSi);
        Ad_Buf=Str3;
        w_txt("Cond:");
        w_int(&Cond,SSS);
        w_txt(" Err:");
        w_int(&Err,SSS);
        Ad_Buf=Str4;
        w_txt("Max Input:");
        w_int(&MaxIn,SSS);
        Ad_Buf++;
        w_txt("Fails:");
        w_int(&Failure,SSS);


    	return;
    }
    if (x_menu>SumTeplZones)
    {
    	ByteY=(x_menu-SumTeplZones-1)/2+1;
    	ByteZ=(x_menu-SumTeplZones-1)%2+1;
    	Y_menu2%=OUT_MODUL_SUM;//cSMech;
    	Form=0;
    	Ad_Buf=Str2;
        w_txt("Fans zone ");
        w_int(ByteY,oS);
		buf[Ad_Buf++]='-';
        w_int(ByteZ,oS);
    	Ad_Buf=Str3;
    	for (ByteX=0;ByteX<MAX_FAN_COUNT;ByteX++)
    	{
    		if (ByteX==32)
    	        Ad_Buf=Str4;
    		if (GD.FanBlock[x_menu-SumTeplZones-1][0].FanData[ByteX].Actual)
    		{

    			buf[Ad_Buf++]='0';
    		}
    		else
    			buf[Ad_Buf++]='1';

    	}
/*        Ad_Buf=Str4;
    	for (ByteX=0;ByteX<MAX_FAN_COUNT;ByteX++)
    	{
    		if (ByteX==32)
    	        Ad_Buf=Str5;
    		if (GD.FanBlock[x_menu-SumTeplZones-1][1].FanData[ByteX].Actual)
    		{

    			buf[Ad_Buf++]='0';
    		}
    		else
    		buf[Ad_Buf++]='1';

    	}
//    	w_txt("05/02");*/
    	return;
    }
    ByteY=x_menu-1;
	Y_menu2=1;//cSMech;

    Ad_Buf=Str2;
	w_txt(Mes7);
	w_int(&x_menu,oS);
	Ad_Buf++;
	w_txt(Mes39);
	Ad_Buf=Str3;
	w_txt(" T  | RH |CO2 |Tp1 |Tp2 |Tp3 |Tp4 |Tp5");
    Ad_Buf=Str4;
	w_int(&GD.Hot.Tepl[ByteY].InTeplSens[cSmTSens1].Value,SSpS0);
	w_int(&GD.Hot.Tepl[ByteY].InTeplSens[cSmTSens2].Value,SSpS0);
	w_int(&GD.Hot.Tepl[ByteY].InTeplSens[cSmTSens3].Value,SSpS0);
	w_int(&GD.Hot.Tepl[ByteY].InTeplSens[cSmTSens4].Value,SSpS0);
	buf[Ad_Buf++]='|';
	w_int(&GD.Hot.Tepl[ByteY].InTeplSens[cSmRHSens].Value,SSpS0);
	buf[Ad_Buf++]='|';
	w_int(&GD.Hot.Tepl[ByteY].InTeplSens[cSmCOSens].Value,SSSS);
	for (ByteX=0;ByteX<cSWaterKontur-1;ByteX++)
	{
		buf[Ad_Buf++]='|';
		IntX=GD.Hot.Tepl[ByteY].InTeplSens[cSmWaterSens+ByteX].Value/10;
		w_int(&IntX,SSS);
		Ad_Buf++;
	}
    Ad_Buf=Str5;
	w_int(&GD.Hot.Tepl[ByteY].AllTask.DoTHeat,SSpS0);
	buf[Ad_Buf++]='|';
	w_int(&GD.Hot.Tepl[ByteY].AllTask.DoRHAir,SSpS0);
	buf[Ad_Buf++]='|';
	w_int(&GD.Hot.Tepl[ByteY].AllTask.DoCO2,SSSS);
	for (ByteX=0;ByteX<cSWaterKontur-1;ByteX++)
	{
		buf[Ad_Buf++]='|';
		IntX=GD.Hot.Tepl[ByteY].Kontur[ByteX].Do/10;
		w_int(&IntX,SSS);
		Ad_Buf++;
	}
	Form=0;
	return;
}

void	pmProgClimate(void)
{
    w_txt(Mes34);
	//w_int(airHeatTimeWork[0],SSS);

	if (x_menu > cSTimer) x_menu=0; 
	if (!x_menu) {
		return;
		}
    Ad_Buf=Str2;
    ByteZ=x_menu-1;
	if(Y_menu2 > SUM_NAME_TIMER) Y_menu2=0;
	if(Y_menu2 >=SUM_NAME_TIMER) Y_menu2=SUM_NAME_TIMER-1;
    w_txt(Mes35); //-Correction-
	w_int(&x_menu,SS);
    Ad_Buf=Str3;
	if(Y_menu2 < StartY_menu2) StartY_menu2 = Y_menu2;
	if(Y_menu2 > (StartY_menu2+2)) StartY_menu2 = Y_menu2-2;
    for (ByteY=StartY_menu2;ByteY < (StartY_menu2+3);ByteY++) 
	{
		ByteX=ByteY % SUM_NAME_TIMER;
       	w_txt(&NameTimer[ByteX].Name);
		Ad_Buf=(Ad_Buf / DisplCols)*DisplCols+20;
		buf[Ad_Buf++]='=';
	   	w_int(&GD.Timer[ByteZ].Zone[NameTimer[ByteX].Index],NameTimer[ByteX].Frm);
		if(Y_menu2 == ByteY) BlkW=1;
		Ad_Buf=((Ad_Buf / DisplCols)+1)*DisplCols;
	}
	return;
}

void pmVersion(void)
{
	if ((Y_menu2%4)==2) BlkW=1;
	w_txt(Mes223);
	if (x_menu > cSTimer) x_menu=0;
	if (!x_menu) {
		return;
		}
    Ad_Buf=Str2;
    ByteZ=x_menu-1;
	if(Y_menu2 > SUM_NAME_TIMER) Y_menu2=0;
	if(Y_menu2 >=SUM_NAME_TIMER) Y_menu2=SUM_NAME_TIMER-1;

	w_txt(FWVersion);
    Ad_Buf=Str3;
	return;
}

void YMenu(char vPozY) {
char pozY;
    ClrDog;
	pozY=vPozY;
	if(!pozY) 		{ pmProgClimate(); 	return; }
	if(!(--pozY))	{ pmParam(); 	return; }
	if(!(--pozY))	{ pmStrategy(); 	return; }
	if(!(--pozY))	{ pmHand(); 	return; }
	if(!(--pozY))	{ pmCalibr(); 	return; }
	if(!(--pozY))	{ pmNow();	 	return; }
	if(!(--pozY))	{ pmParMechanic();	return; }
	// NEW
	if(!(--pozY))	{ pmVersion();	return; }
	//----
	SumYMenu=vPozY-pozY;
	if(Y_menu > SumYMenu) Y_menu=SumYMenu;
}
