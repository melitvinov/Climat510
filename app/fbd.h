#ifndef _FBD_H_
#define _FBD_H_

typedef struct
{
    u8 err_cnt;
    u8 reset_cnt;
    u8 permanent_errs;
} board_stat_t;

typedef struct
{
    u8 type;
    u8 output;
    u8 corr;
} board_input_cfg_t;

// XXX: just for now
typedef struct board_t board_t;

void fbd_init(void);
u8 fbd_get_last_bad_board(void);

board_t *fbd_mount(uint addr);
void fbd_unmount(board_t *board);

board_t *fbd_find_board_by_addr(uint addr);
board_t *fbd_next_board(board_t *board);

void fbd_write_discrete_outputs(board_t *board, u32 val, u32 mask);
void fbd_write_register(board_t *board, uint reg_idx, uint type, uint val);

u16 fbd_read_input(board_t *board, uint input_idx);
void fbd_configure_input(board_t *board, uint input_idx, const board_input_cfg_t *cfg);

const board_stat_t *fbd_get_stat(const board_t *board);
void fbd_reset_errors(board_t *board);
uint fbd_get_addr(const board_t *board);

#ifdef _FBD_C_
// private

#define FULL_SYNC ((1 << _SYNC_TASK_LAST) - 1)

#define MAX_N_BOARDS 	    30

#define MAX_N_INPUTS	    32
#define MAX_N_OUTPUTS	    8

#define ERR_LINK			0x02
#define ERR_CHECKSUM		0x04
#define ERR_RESET			0x40

#define SEQ_PERIOD_MS       50
#define READ_PERIOD_MS      5000
#define KEEPALIVE_PERIOD_MS 15000

#define LINK_ERRS_THRES   	100
#define RESET_ERRS_THRES   	125

// sync tasks are sorted by the execution order
enum sync_tasks_e
{
    SYNC_TASK_PUSH_INPUT_CONFIG,
    SYNC_TASK_PULL_STATUS,
    SYNC_TASK_PUSH_DISCRETE_OUTPUTS,
    SYNC_TASK_PUSH_OUTPUTS,
    SYNC_TASK_PULL_INPUTS,
    _SYNC_TASK_LAST,
};

typedef struct __packed
{
    u8 type;
    u16 val;
} board_output_register_t;

typedef struct __packed
{
    u8 type;
    u8 input;
    u8 output;
    u8 corr;
    u8 reserved[8];
} board_input_cfg_abi_t;

struct board_t
{
    board_output_register_t outputs[MAX_N_OUTPUTS];         // outputs registers (analog/continuous outputs ?)
    board_input_cfg_t inputs_cfg[MAX_N_INPUTS];             // input configs (only used fields are stored)
    u32 discrete_outputs;                                   // bitmap of discrete outputs (relays)
    u16 addr;                                               // board base address
    u16 status_word;                                        // board status word
    u16 inputs[MAX_N_INPUTS];                               // input values

    board_stat_t stat;

    u8 last_sync_errs;
    u8 requested_tasks;       // bitmap of tasks requested for board before the sync cycle
    u8 pending_tasks;         // bitmap of pending (uncompleted) tasks for board
};

typedef struct
{
    void (* const req)(void);
    bool (* const on_done)(void);
} sync_task_t;


typedef struct
{
    timer_t seq_timer;

    board_t pool[MAX_N_BOARDS];
    u32 used_entries;

    board_t *active_board;
    timer_t sync_timer;

    bool is_syncing;
    uint read_delay;
    uint keepalive_delay;
    uint last_bad_board;
} fbd_rt_t;

static void req_pull_status(void);
static bool on_pull_status_done(void);
static void req_push_input_config(void);
static void req_push_discrete_outputs(void);
static void req_push_outputs(void);
static void req_pull_inputs(void);

static void check_sync_progress(timer_t *dummy);

static void abort_sync(void);

static void fbd_periodic(timer_t *dummy);

#endif
#endif

