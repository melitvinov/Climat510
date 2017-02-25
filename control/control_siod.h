#ifndef _65_SIOD_H_
#define _65_SIOD_H_

typedef struct
{
    uint16_t fnSIOfaza[8];
    uint16_t fnSIOpumpOut[8];
    uint16_t fnSIOvelvOut[8];
    uint16_t fnSIOpause[8];
} siod_ctx_t;


const siod_ctx_t *siod_view(void);

void siodInit(void);
void SetUpSiod(const gh_t *gh);
void DoSiod(const gh_t *gh);

#endif
