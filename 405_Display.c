//--------------------------------------------------
//================= ������ 403 =====================
/*====================================================
 ������������ ����������
 ------------------------------------------------------
 -�������� �� �������
 -����� ����� ������ � ���������
 -����� ����� ������� � ���������
 -��������� �������������
 (�������� ��������������� �� asm)
 -����� ������ � ����������� - Video
 ------------------------------------------------------
 ��������� �� 08.01.97
 ======================================================*/
#include "keyboard.h"

#include "stm32f10x_LCD240x64.h"
#include "climdefstuff.h"

// XXX: from climdef
uchar   SizeForm=0;

int CopyVal;
int MinimVal, MaximVal;
char ValSize;
uchar SumYMenu;
bool     B_input;
int8_t      SaveChar;
int16_t     SaveInt;
void    *AdrVal;

void in_val(void);

void SetValue(int vVal) {
    if (ValSize == 1)
        (*(unsigned char *) AdrVal) = (unsigned char) vVal;
    if (ValSize == 2)
        (*(unsigned int *) AdrVal) = vVal;
}

/*===================================================
 ��������� ������������
 -����� � ����� ������ � �����
 -���� ����� � ����� � ����������
 -������� ������
 ���������� ����������
 ���������� �����  -             buf
 ������� ��������� ������ -      Ad_Buf
 ����� ���������� -              bu
 ====================================================*/
/*------------------------------------------------------
 ���������� �������� ���� �� ����������
 --------------------------------------------------------*/


/*
void KeyBoard(void) {
//        GD.TControl.tAutoInf=GD.Control.AutoInf;
    if (GrafView)
        GrafView = 1;
    GD.TControl.tCodTime = 3;
    keyboardSetBITKL(0);
    not += 180;
    ton = 2;
    if (Menu && (keyboardGetSIM() < 10)) {
        in_val();
        B_input = 1;
    } else {
        MinimVal = MaximVal = 0;
        ClrDog;
        EndInput = 0;
        B_input = 0;
        switch (keyboardGetSIM()) {
        case 10:
            if (!Form)
                break;
            if (!Menu) {
                Menu = 1;
                CopyVal = *(unsigned int *) AdrVal;
            } else {
                (*(unsigned int *) AdrVal) = CopyVal;
                Menu = 0;
                Mark = 0;
            }
            break;
        case 1:
            Y_menu = Y_menu2 = 0;
            x_menu = 0;
            break;
        case 2:
        case 11:
            if (x_menu)
                Y_menu2--;
            else {
                if (Y_menu)
                    Y_menu--;
                Y_menu2 = 0;
            }
            break;
        case 3:
            if (Y_menu)
                Y_menu--;
            x_menu = 0;
            Y_menu2 = 0;
            break;
        case 4:
        case 12:
            if (x_menu)
                x_menu--;
            break;
        case 6:
        case 13:
            x_menu++;
            break;

        case 7:
            Y_menu = SumYMenu;
            Y_menu2 = 0;
            x_menu = 0;
            break;
        case 8:
        case 14:
            if (x_menu)
                Y_menu2++;
            else {
                Y_menu++;
                Y_menu2 = 0;
            }
            break;
        case 9:
            Y_menu++;
            x_menu = 0;
            Y_menu2 = 0;
            break;
        }
    }
}
*/

void KeyBoard(void) {
//        GD.TControl.tAutoInf=GD.Control.AutoInf;
    if (GrafView)
        GrafView = 1;
    GD.TControl.tCodTime = 3;
    keyboardSetBITKL(0);
    not += 180;
    ton = 2;
    if (ClimDefStuff.Menu && (keyboardGetSIM() < 10))
    {
        in_val();
        B_input = 1;
    }
    else
    {
        MinimVal = MaximVal = 0;
        ClrDog;
        EndInput = 0;
        B_input = 0;
        switch (keyboardGetSIM())
        {
        case KEY_10 : if (!Form) break;
            if (!ClimDefStuff.Menu)
            {
                ClimDefStuff.Menu=1;
                CopyVal=*(unsigned int *)AdrVal;
            }
            else
            {
                (*(unsigned int *)AdrVal)=CopyVal;
                ClimDefStuff.Menu=0;
                Mark=0;
            }
            break;
        case KEY_1 : Y_menu=Y_menu2=0;x_menu=0;break;
        case KEY_2_UP :
        case 11:
            if (x_menu) Y_menu2--;
            else
            {
                if (Y_menu) Y_menu--;
                Y_menu2=0;
            }
            break;
        case KEY_3 : if (Y_menu) Y_menu--;
            x_menu=0; Y_menu2=0; break;
        case KEY_4_LEFT :
        case KEY_LEFT :
            if (x_menu) x_menu--;break;
        case KEY_6_RIGHT :
        case KEY_UP :
            x_menu++;break;

        case KEY_7 : Y_menu=SumYMenu;Y_menu2=0;x_menu=0;break;
        case KEY_8_DOWN :
        case KEY_RIGHT :
            if (x_menu) Y_menu2++;
            else
            {
                Y_menu++;Y_menu2=0;
            } break;
        case KEY_9 : Y_menu++; x_menu=0; Y_menu2=0; break;
        }
    }
}

/*---------------------------------------------------
 ����� ������ � �����
 ----------------------------------------------------*/
void w_txt(const char *bu)
{
    char *vu;
    if (GD.Control.Language)
    {
        vu=bu;
        while ((*vu)) if ((*(vu++))=='#')
            {
                bu=vu;break;
            }
    }
    while ((*bu)&&((*bu)!='#'))
    {
        lcdbuf[Ad_Buf++]=(*(bu++));
    };
}
/*void w_txt(char code *bu)
 {
 while((*bu)) {buf[Ad_Buf++]=(*(bu++));};
 }
 */

/*---------------------------------------------------
 ����� ����� � �����
 ----------------------------------------------------*/
void w_int(void *bu, char frmt) {
    uint16_t vre;
    unsigned char vrel;
    uchar i;
    if (!BlkW)
    {
        AdrVal = bu;
        AdinB = Ad_Buf;
        Form = frmt;
        SizeForm = Form & 0xF;
    }
    ClrDog;
    switch (frmt)
    {
    case SRelay:
        vre = (*(uint16_t *) bu);
        lcdbuf[Ad_Buf++] = '=';
        ClrDog;
        if (!BlkW)
        {
            SaveInt = vre;
            AdinB = Ad_Buf;
        }
        if (TestRelay(vre) > 0)
            lcdbuf[Ad_Buf++] = '1';
        else
            lcdbuf[Ad_Buf++] = '0';
        Ad_Buf = ((Ad_Buf / 20) * 20) + 26;
        lcdbuf[Ad_Buf++] = '[';
        lcdbuf[Ad_Buf++] = vre / 10 + '0';
        lcdbuf[Ad_Buf++] = vre % 10 + '0';
        lcdbuf[Ad_Buf++] = ']';
        break;
    case bS:
        if (!BlkW)
            SaveChar = ByteX;
        if (ByteX & (*(char *) bu))
            lcdbuf[Ad_Buf++] = '1';
        else
            lcdbuf[Ad_Buf++] = '0';
        ClrDog;
        break;
    case SSpS0:
        if ((*(int16_t *) bu) < 0)
        {
            lcdbuf[Ad_Buf] = '-';
            Ad_Buf++;
            vre = -(*(int16_t *) bu);
        }
        else
            vre = *(uint16_t *) bu;
        vre += 5;
        vre /= 10;
        lcdbuf[Ad_Buf + 3] = vre % 10 + '0';
        vre /= 10;
        lcdbuf[Ad_Buf + 1] = vre % 10 + '0';
        vre /= 10;
        ClrDog;
        lcdbuf[Ad_Buf] = vre + '0';
        lcdbuf[Ad_Buf + 2] = '.';
        Ad_Buf += 4;
        break;
    case SpSSpSS:
        vre = *(uint16_t *) bu;
        lcdbuf[Ad_Buf + 6] = vre % 10 + '0';
        vre /= 10;
        lcdbuf[Ad_Buf + 5] = vre % 10 + '0';
        vre /= 10;
        lcdbuf[Ad_Buf + 3] = vre % 10 + '0';
        vre /= 10;
        lcdbuf[Ad_Buf + 2] = vre % 10 + '0';
        vre /= 10;
        lcdbuf[Ad_Buf] = vre + '0';
        lcdbuf[Ad_Buf + 1] = '.';
        lcdbuf[Ad_Buf + 4] = '.';
        ClrDog;
        Ad_Buf += 7;
        break;

    case SSpSS:
        vre = *(uint16_t *) bu;
        lcdbuf[Ad_Buf + 4] = vre % 10 + '0';
        vre /= 10;
        lcdbuf[Ad_Buf + 3] = vre % 10 + '0';
        vre /= 10;
        lcdbuf[Ad_Buf + 1] = vre % 10 + '0';
        vre /= 10;
        lcdbuf[Ad_Buf] = vre + '0';
        lcdbuf[Ad_Buf + 2] = '.';
        ClrDog;
        if (vre > 9)
        {
            lcdbuf[Ad_Buf] = '!';
            lcdbuf[Ad_Buf + 1] = '0';
            lcdbuf[Ad_Buf + 3] = '0';
            lcdbuf[Ad_Buf + 4] = '0';
        }
        Ad_Buf += 5;
        break;
    case SSSi:
    case SSSS:
        vre = *(uint16_t *) bu;
        for (i = frmt & 0xF; i > 0; i--)
        {
            lcdbuf[Ad_Buf - 1 + i] = vre % 10 + '0';
            vre /= 10;
        }
        Ad_Buf += frmt & 0xF;
        ClrDog;
        break;
    case SSdSS:
        vre = *(uint16_t *) bu;
        if (ClimDefStuff.Second & 1)
            lcdbuf[Ad_Buf + 2] = ':';
        lcdbuf[Ad_Buf + 4] = vre % 10 + '0';
        vre /= 10;
        lcdbuf[Ad_Buf + 3] = vre % 6 + '0';
        vre /= 6;
        lcdbuf[Ad_Buf + 1] = vre % 10 + '0';
        vre /= 10;
        lcdbuf[Ad_Buf] = vre + '0';
        Ad_Buf += 5;
        ClrDog;
        break;
    case DsMsY:
    case SSsSS:
        vre = *(uint16_t *) bu;
        lcdbuf[Ad_Buf + 2] = '/';
        i = vre % 256;
        lcdbuf[Ad_Buf + 1] = i % 10 + '0';
        lcdbuf[Ad_Buf] = i / 10 + '0';
        i = vre / 256;
        lcdbuf[Ad_Buf + 4] = i % 10 + '0';
        lcdbuf[Ad_Buf + 3] = i / 10 + '0';
        Ad_Buf += 5;
        ClrDog;
        if (frmt == DsMsY)
        {
            lcdbuf[Ad_Buf] = '/';
            lcdbuf[Ad_Buf + 2] = CtrYear % 10 + '0';
            lcdbuf[Ad_Buf + 1] = CtrYear / 10 + '0';
            Ad_Buf += 3;
        }
        break;
    case SSSpS:
        vre = *(uint16_t *) bu;
        lcdbuf[Ad_Buf + 3] = '.';
        lcdbuf[Ad_Buf + 4] = vre % 10 + '0';
        vre /= 10;
        lcdbuf[Ad_Buf + 2] = vre % 10 + '0';
        vre /= 10;
        lcdbuf[Ad_Buf + 1] = vre % 10 + '0';
        vre /= 10;
        lcdbuf[Ad_Buf] = vre + '0';
        Ad_Buf += 5;
        ClrDog;
        break;
    case SpSSS:
        vre = *(uint16_t *) bu;
        lcdbuf[Ad_Buf + 1] = '.';
        lcdbuf[Ad_Buf + 4] = vre % 10 + '0';
        vre /= 10;
        lcdbuf[Ad_Buf + 3] = vre % 10 + '0';
        vre /= 10;
        lcdbuf[Ad_Buf + 2] = vre % 10 + '0';
        vre /= 10;
        lcdbuf[Ad_Buf] = vre + '0';
        Ad_Buf += 5;
        ClrDog;
        break;
    case StStStS:
        for (i = 0; i < 4; i++)
        {
            vrel = ((unsigned char *) bu)[i];
            lcdbuf[Ad_Buf + i * 4 + 2] = vrel % 10 + '0';
            vrel /= 10;
            lcdbuf[Ad_Buf + i * 4 + 1] = vrel % 10 + '0';
            vrel /= 10;
            lcdbuf[Ad_Buf + i * 4] = vrel % 10 + '0';
            lcdbuf[Ad_Buf + i * 4 + 3] = '-';
        }
        Ad_Buf += 15;
        lcdbuf[Ad_Buf] = ' ';
        ClrDog;
        break;

    default:
        vre = (uint16_t)(*(unsigned char *) bu);
        for (i = frmt & 0xF; i > 0; i--)
        {
            lcdbuf[Ad_Buf - 1 + i] = (unsigned char) vre % 10 + '0';
            vre /= 10;
        }
        Ad_Buf += frmt & 0xF;
    }
}
/*---------------------------------------------------
 ���� ����� � ����� � ����������
 ----------------------------------------------------*/
void in_val(void) {
    uint16_t vre;
    unsigned char vrel;
    uchar i;
    vre = 0;
    lcdbuf[AdinB + Mark++] = keyboardGetSIM() + '0';
    if ((lcdbuf[AdinB + Mark] > '9') || (lcdbuf[AdinB + Mark] < '0'))
        Mark++;
    switch (Form)
    {
    case 0:
        return;
//                        ValSize=0;
//                        break;

    case SRelay:
        if (lcdbuf[AdinB] == '1')
            SetRelay(SaveInt);
        else
            ClrRelay(SaveInt);
        ValSize = 0;
        break;
    case bS:
        if (lcdbuf[AdinB] == '1')
            (*(char *) AdrVal) |= SaveChar;
        else
            (*(char *) AdrVal) &= ~SaveChar;
        ValSize = 0;
        break;
    case SSdSS:
        vre = (lcdbuf[AdinB] - '0') * 10 + lcdbuf[AdinB + 1] - '0';
        vre *= 60;
        vre += (lcdbuf[AdinB + 3] - '0') * 10 + lcdbuf[AdinB + 4] - '0';
        (*(uint16_t *) AdrVal) = vre;
        ValSize = 2;
        ClrDog;
        break;
    case DsMsY:
        CtrYear = (lcdbuf[AdinB + 6] - '0') * 10 + lcdbuf[AdinB + 7] - '0';
    case SSsSS:
        ValSize = (lcdbuf[AdinB] - '0') * 10 + lcdbuf[AdinB + 1] - '0'; /*���� ������*/
        vre = (lcdbuf[AdinB + 3] - '0') * 10 + lcdbuf[AdinB + 4] - '0'; /*�����*/
        if (EndInput)
        {
            if (!ValSize)
                ValSize = 1;
            if (!vre)
                vre = 1;
            if (vre > 12)
                vre = 12;
            if (ValSize > Mon[vre - 1])
                ValSize = Mon[vre - 1];
        }
        vre *= 256;
        vre += ValSize;
        (*(uint16_t *) AdrVal) = vre;
        ValSize = 2;
        ClrDog;
        break;
    case SSpS0:
        vre = (lcdbuf[AdinB] - '0') * 10 + lcdbuf[AdinB + 1] - '0';
        vre *= 10;
        vre += lcdbuf[AdinB + 3] - '0';
        vre *= 10;
        (*(uint16_t *) AdrVal) = vre;
        ValSize = 2;
        break;
    case SSpSS:
        vre = (lcdbuf[AdinB] - '0') * 10 + lcdbuf[AdinB + 1] - '0';
        vre *= 10;
        vre += lcdbuf[AdinB + 3] - '0';
        vre *= 10;
        vre += lcdbuf[AdinB + 4] - '0';
        (*(uint16_t *) AdrVal) = vre;
        ValSize = 2;
        break;
    case SpSSpSS:
        vre = (lcdbuf[AdinB] - '0') * 10 + lcdbuf[AdinB + 2] - '0';
        vre *= 10;
        vre += lcdbuf[AdinB + 3] - '0';
        vre *= 10;
        vre += lcdbuf[AdinB + 5] - '0';
        vre *= 10;
        vre += lcdbuf[AdinB + 6] - '0';
        (*(uint16_t *) AdrVal) = vre;
        ValSize = 2;
        break;
    case SSSpS:
        vre = (lcdbuf[AdinB] - '0') * 10 + lcdbuf[AdinB + 1] - '0';
        vre *= 10;
        vre += lcdbuf[AdinB + 2] - '0';
        vre *= 10;
        vre += lcdbuf[AdinB + 4] - '0';
        (*(uint16_t *) AdrVal) = vre;
        ValSize = 2;
        break;
    case SpSSS:
        vre = (lcdbuf[AdinB] - '0') * 10 + lcdbuf[AdinB + 2] - '0';
        vre *= 10;
        vre += lcdbuf[AdinB + 3] - '0';
        vre *= 10;
        vre += lcdbuf[AdinB + 4] - '0';
        (*(uint16_t *) AdrVal) = vre;
        ValSize = 2;
        break;
    case SSSi:

    case SSSS:
        for (ValSize = 0; ValSize < SizeForm; ValSize++)
            vre = vre * 10 + lcdbuf[AdinB + ValSize] - '0';
        (*(uint16_t *) AdrVal) = vre;
        ValSize = 2;
        break;

    case StStStS:
        for (i = 0; i < 4; i++)
        {
            vrel = 0;
            vrel += (lcdbuf[AdinB + i * 4] - '0');
            vrel *= 10;
            vrel += (lcdbuf[AdinB + i * 4 + 1] - '0');
            vrel *= 10;
            vrel += (lcdbuf[AdinB + i * 4 + 2] - '0');
            ((unsigned char *) AdrVal)[i] = vrel;
        }
        ValSize = 4;
        break;

    default:
        for (ValSize = 0; ValSize < SizeForm; ValSize++)
            vre = vre * 10 + lcdbuf[AdinB + ValSize] - '0';
        (*(unsigned char *) AdrVal) = (unsigned char) vre;
        ValSize = 1;
    }
    if (Mark >= SizeForm)  // ��������� �����
    {
        EndInput = 1;
        Mark = 0;
        ClimDefStuff.Menu = 0;
        if (vre < MinimVal)
            SetValue(MinimVal);
        if (MaximVal && (vre > MaximVal))
            SetValue(MaximVal);
        if (!ValSize)
            return;
        SetInSaveRam(AdrVal, ValSize);
    }
}

