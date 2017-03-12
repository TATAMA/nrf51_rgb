#define MAX_ID_NUM           8
extern TMcmd apptimers[MAX_SIGNAL_PORT+1][MAX_ID_NUM+1]; 
//apptimers[0][0] is reserved

//Timer command structure 
typedef struct appTimer_struct
{
	uint8 hour;
	uint8 min;
	uint8 on_off;
	uint8 repeat;
	uint8 valid;
}TMcmd;

extern void reset_timer(void);