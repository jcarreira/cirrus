#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

enum PS_OP {
  SEND_LR_GRADIENT,
  SEND_MF_GRADIENT,
  GET_LR_FULL_MODEL,
  GET_MF_FULL_MODEL,
  GET_LR_SPARSE_MODEL,
  GET_MF_SPARSE_MODEL,
  SET_TASK_STATUS,
  GET_TASK_STATUS,
  REGISTER_TASK,
  GET_NUM_CONNS,
  GET_NUM_UPDATES,
  GET_LAST_TIME_ERROR,
  KILL_SIGNAL,
  SEND_LDA_UPDATE,
  GET_LDA_MODEL,
  GET_LDA_SLICES_IDX,
  SEND_LL_NDT,
  SEND_TIME,
  DEREGISTER_TASK
};

#define MAGIC_NUMBER (0x1337)

#endif // _CONSTANTS_H_
