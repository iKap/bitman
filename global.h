#define VERSION "1.5"

#define DBG_FNAME (0)

#if DBG_FNAME
#define MY_NAME printf("[%s]\n",__FUNCTION__)
#else
#define MY_NAME
#endif

#define APP_NAME "bitman"
#define APP_NAME_SHORT "bm"

