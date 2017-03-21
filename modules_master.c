#include "syntax.h"

#include "module.h"
#include "modules_master.h"

#include "control_gd.h"

#include "debug.h"


static uint8_t cCycle;
static uint8_t  ncFan;
static int cModule = -1;

static module_t ModulData[N_MAX_MODULES];

uint16_t GetIPCComMod(uint16_t nAddress)
{
    return nAddress/100;
}

uint16_t GetIPCNum(uint16_t nAddress)
{
    return nAddress%100;
}




void ClrAllOutIPCDigit(void)
{
    int i;
    for (i=0; i< N_MAX_MODULES; i++)
    {
        // ok, so the zero module terminates the list
        if (!ModulData[i].CpM)
            return;
        ModulData[i].discrete_outputs=0;
    }
}


module_t *find_module_by_address(u16 addr)
{
    u32 vCpM = GetIPCComMod(addr);

    if (!vCpM) return NULL;
    if (vCpM/100 == 6) return NULL;

    for (uint i=0; i< N_MAX_MODULES; i++)
    {
        module_t *p =  &ModulData[i];

        if (p->CpM == vCpM)
            return p;
    }

    return NULL;
}


module_t *alloc_module(u16 addr)
{
    u32 vCpM = GetIPCComMod(addr);

    if (!vCpM) return NULL;
    if (vCpM/100 == 6) return NULL;

    for (uint i=0; i< N_MAX_MODULES; i++)
    {
        module_t *p =  &ModulData[i];

        if (p->CpM == vCpM)
            return p;
        if (p->CpM == 0)
        {
            p->CpM = vCpM;
            return p;
        }
    }

    return NULL;
}


void SetOutIPCDigit(char How, uint16_t nAddress)
{
    module_t *m = alloc_module(nAddress);
    if (! m)
        return;

    u32 bOut=1;
    bOut <<= GetIPCNum(nAddress)-1;

    if (How)
        m->discrete_outputs |= bOut;
    else
        m->discrete_outputs &= ~(bOut);
}


void SetOutIPCReg(uint16_t How, uint8_t fType, uint16_t nAddress,char* nErr, module_fandata_t *fandata)
{
    module_t *m = alloc_module(nAddress);
    if (! m)
        return;

    u32 bOut=GetIPCNum(nAddress)-1;
    if (bOut>=MODULE_MAX_N_OUTPUTS)
        return;

    *nErr=m->err_cnt;
    m->outputs[bOut].val=How;
    m->outputs[bOut].type= fType;
    m->fandata = fandata;
    return;
}


// GUESS: read binary value from the cached data (relay pin output)
char GetOutIPCDigit(uint16_t nAddress)
{
    u32 vCpM=GetIPCComMod(nAddress);
    if (!vCpM) return -1;
    if (vCpM/100 == 6) return 1;

    const module_t *m = find_module_by_address(nAddress);
    if (! m)
        return -1;

    u32 bIn = 1U << (GetIPCNum(nAddress) - 1);
    if (m->discrete_outputs & bIn)
        return 1;
    else
        return 0;
}


// GUESS: read analog values from the cached data
uint16_t GetInIPC(uint16_t nAddress, s8 *nErr)
{
//TODO
    u32 vCpM=GetIPCComMod(nAddress);
    if (!vCpM)
    {
        *nErr=-1;
        return 0;
    }
    if (vCpM/100 == 6)
    {
        *nErr=0;
        return 0;
    }

    u32 vInput=GetIPCNum(nAddress);
    if (!vInput)
    {
        *nErr=-1;
        return 0;
    }

    const module_t *m = alloc_module(nAddress);

    #warning "4444 ? WTF ?"
    if (! m)
    {
        *nErr=0;
        return 4444;
    }

    *nErr = m->err_cnt;
    return m->inputs[vInput-1];
}


// GUESS: read the the discrete input from cached data
uint16_t GetDiskrIPC(uint16_t nAddress, s8 *nErr)
{
    u32 vCpM=GetIPCComMod(nAddress);
    if (!vCpM)
    {
        *nErr=-1;
        return 0;
    }
    if (vCpM/100 == 6)
    {
        *nErr=0;
        return 1;
    }
    u32 vInput=GetIPCNum(nAddress);
    if (!vInput)
    {
        *nErr=-1;
        return 0;
    }

    const module_t *m = alloc_module(nAddress);

    if (! m)
    {
        *nErr=0;
        return 0;
    }

    *nErr=m->err_cnt;
    if ((m->inputs[vInput-1]>2500) && (m->err_cnt < iMODULE_MAX_ERR))
        return 1;
    else
        return 0;
}


// GUESS: resets the input config in cached data and schedule the module update
void UpdateInIPC(uint16_t nAddress, const module_input_cfg_t *cfg)
{
//  nAddress = 101;

    u32 vInput=GetIPCNum(nAddress);
    if (! vInput)
        return;

    module_t *m = alloc_module(nAddress);

    if (! m)
        return;

//			if ((NoSameBuf(((char*)(&ModulData[i].InConfig[vInput-1]))+2,((char*)ModulConf)+2,2/*sizeof(TIModulConf)-2*/)) //·ÂÁ Í‡ÎË·Ó‚ÓÍ
//				||(ModulData[i].InConfig[vInput-1].Type!=ModulConf->Type))
    m->cond |= MODULE_ERR_NEED_RESET;
    memcpy(&m->inputs_cfg[vInput-1], cfg, sizeof(module_input_cfg_t));
    //********************** Õ¿ƒŒ ”¡–¿“‹ *****************************
    for (uint k=0;k < 32;k++)
    {

        //ModulData[i].InConfig[k].Type=3;
        m->inputs_cfg[k].input=k+1;
    }
    //****************************************************************

    if (m->max_n_inputs<vInput)
        m->max_n_inputs = vInput;
}


void ModStatus(uint8_t nMod,uint16_t* fCpM,uint8_t *fErr,uint8_t *fFail, uint8_t *fCond,uint8_t *fMaxIn,uint16_t **fInputs)
{
    *fCpM=ModulData[nMod].CpM;
    *fErr=ModulData[nMod].err_cnt;
    *fFail=ModulData[nMod].fail_cnt;
    *fCond=ModulData[nMod].cond;
    *fMaxIn=ModulData[nMod].max_n_inputs;
    *fInputs=ModulData[nMod].inputs;
}

static bool any_modules_defined(void)
{
    if (ModulData[0].CpM == 0)
        return 0;
    return 1;
}

void SendIPC(s8 *fErrModule)
{
    if (! any_modules_defined())
    {
//        LOG("no modules defined");
        return;
    }

    if (cModule < 0)
    {
        // XXX: once at startup
        cModule = 0;
        module_processor_run(&ModulData[cModule], 0, 0);
        return;
    }

    if (module_processor_is_busy())
        return;

    if (ModulData[cModule].cond)
        ModulData[cModule].err_cnt++;
    else
        ModulData[cModule].err_cnt=0;

    if (ModulData[cModule].fail_cnt>iMODULE_MAX_FAILURES)
        ModulData[cModule].fail_cnt=iMODULE_MAX_FAILURES;

    if (ModulData[cModule].err_cnt>iMODULE_MAX_ERR)
    {
        ModulData[cModule].err_cnt=iMODULE_MAX_ERR;
        *fErrModule=ModulData[cModule].CpM%100;
    }

    #warning "ncfan, ccycle are seriously broken"

    ncFan %= MAX_FAN_COUNT;
    cCycle %= 10;

    cModule = (cModule + 1) % N_MAX_MODULES;

    // if module is not defined, reset cycle and start from the first module
    if (ModulData[cModule].CpM == 0)
    {
        cModule=0;
        cCycle++;
        ncFan++;
        module_processor_run(&ModulData[cModule], (cCycle % 10) != (cModule % 10), ncFan);
        return;
    }


/*	if ((ModulData[cModule].CpM/100) == 6)
    {
        ModulData[cModule].Err=0;
        cModule++;
        return;
    }*/
//  if (ModulData[cModule].CpM/100)
//  {
//      WARN("attempt to send something over uart2. ignored");
//      ModulData[cModule].Cond=0;
//      //RS485_Out2_Transmit(120+ModulData[cModule].CpM%100,ModulData[cModule].OutValues);
//      cOperInModule= ACTION_TOUCH_UART2;
//      return;
//  }
//
//  if (IMOD_Exchange(&ModulData[cModule]) == 0)
//      cOperInModule++;
}
