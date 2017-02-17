/*====================================================
Общие Подпрограммы
====================================================*/
typedef struct
{
    uint16_t year;  /* 1..4095 */
    uint8_t  month; /* 1..12 */
    uint8_t  mday;  /* 1.. 31 */
    uint8_t  wday;  /* 0..6, Sunday = 0*/
    uint8_t  hour;  /* 0..23 */
    uint8_t  min;   /* 0..59 */
    uint8_t  sec;   /* 0..59 */
    uint8_t  dst;   /* 0 Winter, !=0 Summer */
} eDateTime;

// XXX: this buddy is defined a lot of times
char NowDayOfWeek;

/*struct TDateTime {
        uchar Addr;
        uchar Sec;
        uchar Min;
        uchar Hr;
        uchar Day;
        uchar Date;
        uchar Month;
        uchar Year;
        } DateTime;*/

#define FIRSTYEAR   2000		// start year
#define FIRSTDAY    6			// 0 = Sunday

static const uint8_t DaysInMonth[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static uint8_t isDST( const RTC_t *t )
{
    uint8_t wday, month;        // locals for faster access

    month = t->month;

    if (month < 3 || month > 10)     // month 1, 2, 11, 12
    {
        return 0;                   // -> Winter
    }

    wday  = t->wday;

    if (t->mday - wday >= 25 && (wday || t->hour >= 2)) // after last Sunday 2:00
    {
        if (month == 10)             // October -> Winter
        {
            return 0;
        }
    }
    else                            // before last Sunday 2:00
    {
        if (month == 3)              // March -> Winter
        {
            return 0;
        }
    }

    return 1;
}

static uint8_t adjustDST( RTC_t *t )
{
    uint8_t hour, day, wday, month;         // locals for faster access

    hour  = t->hour;
    day   = t->mday;
    wday  = t->wday;
    month = t->month;

    if (isDST(t))
    {
        t->dst = 1;
        hour++;                             // add one hour
        if (hour == 24)                   // next day
        {
            hour = 0;
            wday++;                         // next weekday
            if (wday == 7)
            {
                wday = 0;
            }
            if (day == DaysInMonth[month-1])     // next month
            {
                day = 0;
                month++;
            }
            day++;
        }
        t->month = month;
        t->hour  = hour;
        t->mday  = day;
        t->wday  = wday;
        return 1;
    }
    else
    {
        t->dst = 0;
        return 0;
    }
}


static void counter_to_struct( uint32_t sec, eDateTime *t )
{
    uint16_t day;
    uint8_t year;
    uint16_t dayofyear;
    uint8_t leap400;
    uint8_t month;

    t->sec = sec % 60;
    sec /= 60;
    t->min = sec % 60;
    sec /= 60;
    t->hour = sec % 24;
    day = (uint16_t)(sec / 24);

    t->wday = (day + FIRSTDAY) % 7;     // weekday

    year = FIRSTYEAR % 100;             // 0..99
    leap400 = 4 - ((FIRSTYEAR - 1) / 100 & 3);  // 4, 3, 2, 1

    for (;;)
    {
        dayofyear = 365;
        if ((year & 3) == 0)
        {
            dayofyear = 366;                    // leap year
            if (year == 0 || year == 100 || year == 200) // 100 year exception
            {
                if (--leap400)                   // 400 year exception
                {
                    dayofyear = 365;
                }
            }
        }
        if (day < dayofyear)
        {
            break;
        }
        day -= dayofyear;
        year++;                 // 00..136 / 99..235
    }
    t->year = year + FIRSTYEAR / 100 * 100; // + century

    if (dayofyear & 1 && day > 58)   // no leap year and after 28.2.
    {
        day++;                  // skip 29.2.
    }

    for (month = 1; day >= DaysInMonth[month-1]; month++)
    {
        day -= DaysInMonth[month-1];
    }

    t->month = month;               // 1..12
    t->mday = day + 1;              // 1..31
}

static uint32_t struct_to_counter( const RTC_t *t )
{
    uint8_t i;
    uint32_t result = 0;
    uint16_t idx, year;

    year = t->year;

    /* Calculate days of years before */
    result = (uint32_t)year * 365;
    if (t->year >= 1)
    {
        result += (year + 3) / 4;
        result -= (year - 1) / 100;
        result += (year - 1) / 400;
    }

    /* Start with 2000 a.d. */
    result -= 730485UL;

    /* Make month an array index */
    idx = t->month - 1;

    /* Loop thru each month, adding the days */
    for (i = 0; i < idx; i++)
    {
        result += DaysInMonth[i];
    }

    /* Leap year? adjust February */
    if (year%400 == 0 || (year%4 == 0 && year%100 !=0))
    {
        ;
    }
    else
    {
        if (t->month > 1)
        {
            result--;
        }
    }

    /* Add remaining days */
    result += t->mday;

    /* Convert to seconds, add all the other stuff */
    result = (result-1) * 86400L + (uint32_t)t->hour * 3600 +
             (uint32_t)t->min * 60 + t->sec;

    return result;
}


char ReadDateTime()
{
    uint32_t t;

    while (( t = RTC_GetCounter() ) != RTC_GetCounter())
    {
        ;
    }
    counter_to_struct( t, DateTime ); // get non DST time
    adjustDST( DateTime );
}

static void my_RTC_SetCounter(uint32_t cnt)
{
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
    /* Change the current time */
    RTC_SetCounter(cnt);
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
}

void WriteDateTime()
{
    uint32_t cnt;
    volatile uint16_t i;
    eDateTime ts;

    cnt = struct_to_counter( DateTime ); // non-DST counter-value
    counter_to_struct( cnt, &ts );  // normalize struct (for weekday)
    if (isDST( &ts ))
    {
        cnt -= 60*60; // Subtract one hour
    }
    PWR_BackupAccessCmd(ENABLE);
    my_RTC_SetCounter( cnt );
    PWR_BackupAccessCmd(DISABLE);
}


void SetRTC(void) {
    DateTime.sec=Second;
    DateTime.min=CtrTime%60;
    DateTime.hour=CtrTime/60;
    DateTime.mday=CtrData&0xff;
    DateTime.month=CtrData>>8;
    DateTime.year=CtrYear+2000;
    ClrDog;
    WriteDateTime();
}
void GetRTC(void) {
    ReadDateTime(); //CtrTime=0;

    //Second=DateTime.Sec&0x0F;
    //Second+=(DateTime.Sec>>4)*10;
    CtrTime=DateTime.min;
    CtrTime+=DateTime.hour*60;
    CtrData=DateTime.mday;
    CtrData+=DateTime.month<<8;
    CtrYear=DateTime.year-2000;
    NowDayOfWeek=DateTime.wday;
}

//---------------------- end рассчет дня недели ---------------------


