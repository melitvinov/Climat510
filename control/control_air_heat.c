#include "syntax.h"

#define _FRIEND_OF_CONTROL_

#include "control.h"
#include "control_subr.h"
#include "control_air_heat.h"


typedef struct
{
    int16_t pause;
    int16_t time_work;
    uint8_t is_on;
} heat_rt_t;

typedef struct
{
    heat_rt_t heat_rt[8];
} heat_ctx_t;

static heat_ctx_t ctx;

static void airHeatSetHeatPause(char timeInc, heat_rt_t *rt)
{
    if (timeInc)
        rt->pause += timeInc;
    else
        rt->pause = 0;
}

static void airHeatSetTimeWork(char timeInc, heat_rt_t *rt)
{
    if (timeInc)
        rt->time_work += timeInc;
    else
        rt->time_work = 0;
}

static void airHeatOn(heat_rt_t *rt)
{
    _GD.Hot.Tepl[rt - ctx.heat_rt].HandCtrl[cHSmHeat].Position = 1;
    rt->is_on = 1;
    airHeatSetHeatPause(1, rt);
}

static void airHeatOff(heat_rt_t *rt)
{
    _GD.Hot.Tepl[rt - ctx.heat_rt].HandCtrl[cHSmHeat].Position = 1;

    airHeatSetTimeWork(0, rt);
    airHeatSetHeatPause(0, rt);
    airHeatSetHeatPause(1, rt);  // запускаем счетчик паузы

    rt->is_on = 0;
}

void airHeatInit(void)
{
    char tCTepl;
    for (tCTepl=0;tCTepl<cSTepl;tCTepl++)
    {
        heat_rt_t *rt = &ctx.heat_rt[tCTepl];

        rt->pause = 0;
        rt->is_on = 0;
        rt->time_work = 0;
        //airHeatSetTimeWork(GD.TuneClimate.airHeatMinWork,tCTepl);  // что бы сработывало условие выключения обогревателя при вервом старте
        airHeatOff(rt);
    }
}

void airHeatTimers(void)
{
    char tCTepl;
    for (tCTepl=0;tCTepl<cSTepl;tCTepl++)
    {
        heat_rt_t *rt = &ctx.heat_rt[tCTepl];

        if (rt->is_on == 1)
        {
            if (rt->time_work >= (_GD.TuneClimate.airHeatMaxWork / 100)) // если обогреватель включен проверяем макс время работы
                airHeatOff(rt);
            else
                airHeatSetTimeWork(1, rt);   // если обогреватель включен увеличиваем его время на 1 мин
        }
        if (rt->is_on  ==  0)
        {
            if (rt->pause >= (_GD.TuneClimate.airHeatPauseWork / 100))
                airHeatSetHeatPause(0, rt); // 0 означает что обогреватель может быть включен
            else if (rt->pause > 0)
                airHeatSetHeatPause(1, rt);     // считаем паузу пока она не обнулится
        }
    }
}

void airHeat(char fnTepl)
{
    if (_GDP.Hot_Hand[cHSmHeat].RCS & cbManMech) return;

    heat_rt_t *rt = &ctx.heat_rt[fnTepl];

    int tempTon = 0;
    int tempToff = 0;
//	if ( fnTepl != 0) return;
    int tempT = getTempHeat(fnTepl);
    if (tempT < _GD.Hot.Tepl[fnTepl].AllTask.TAir)
        tempTon = _GD.Hot.Tepl[fnTepl].AllTask.TAir - tempT;
    else
        tempToff = tempT - _GD.Hot.Tepl[fnTepl].AllTask.TAir;
    if (tempT > 0)
    {
        //if ((GD.TuneClimate.airHeatTemperOn >= tempTon) && (GD.TuneClimate.airHeatTemperOff > tempToff) && (airHeatGetHeatPause(fnTepl)  ==  0))  // обогреватель можно вкл и пауза между вкл прошла
        if ((_GD.TuneClimate.airHeatTemperOn <= tempTon) && (rt->pause ==  0))  // обогреватель можно вкл и пауза между вкл прошла
            airHeatOn(rt);
        if ((_GD.TuneClimate.airHeatTemperOff <= tempToff) && (rt->pause >= (_GD.TuneClimate.airHeatMinWork / 100)))  // обогреватель можно выклюсить если мин время работы прошло и максимальная температура достигнута
            airHeatOff(rt);
    }
    _GD.Hot.Tepl[fnTepl].airHeatTimeWork = rt->time_work*100;
    _GD.Hot.Tepl[fnTepl].airHeatOnOff = rt->is_on;
}

