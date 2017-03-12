#include "appTimer.h"

void reset_timer(void)
{
	uint8_t len = sizeof(timers);
	memset( timers, 0, len);
	//osal_snv_write( NVID_TIMER, len, SW_timers );
}