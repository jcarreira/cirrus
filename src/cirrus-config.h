#define USE_EFS

#define USE_REDIS

#ifdef USE_REDIS
#define PS_IP "172.31.2.117"
#define PS_PORT 1337
#endif

#define LOADING_TASK_RANK -100000
#define PERFORMANCE_LAMBDA_RANK (-100000)

#define WORKER_SPARSE_TASK_RANK (3)
#define ERROR_SPARSE_TASK_RANK (2)
#define PS_SPARSE_SERVER_TASK_RANK (1)
#define LOADING_SPARSE_TASK_RANK (0)
#define WORKERS_BASE 3 // used in wait_for_start

#define SAMPLE_BASE (0)

using FEATURE_TYPE = float;

//#define CRITEO_HASH_BITS 19
#define RCV1_HASH_BITS 19

#define LIMIT_NUMBER_PASSES 3

// number of factors for neflix workload
#define NUM_FACTORS 10

// define the number of poll threads
#define NUM_POLL_THREADS 2

// define number of parameter server working threads
# define NUM_PS_WORK_THREADS 4