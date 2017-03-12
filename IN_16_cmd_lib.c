#include "IN_16_cmd_lib.h"

uint8_t pass_word[SIMPLEPROFILE_CHAR_LEN_MAX] = {0};
uint8_t pass_word_temp[SIMPLEPROFILE_CHAR_LEN_MAX] = {0};
SecurityState  security_state = PASS_CODE_FALSE;

void function_Init( uint8_t task_id )
{

}


void check_password(uint8_t* p_password)
{
  uint8_t sta;
  switch(security_state)
  {	
    case PASS_CODE_ACCEPT:
      memcpy(pass_word_temp, p_password, SIMPLEPROFILE_CHAR_LEN_MAX);
    break;
    
    case PASS_CODE_FALSE:
    case PASS_CODE_TRUE:
      sta = memcmp(pass_word, p_password, SIMPLEPROFILE_CHAR_LEN_MAX);
      if(sta == true)
      {
        security_state = PASS_CODE_TRUE;
      }
      else
      {
        security_state = PASS_CODE_FALSE;
        #ifdef DBG
        //SimpleProfile_SetParameter( SIMPLEPROFILE_CHAR3, SIMPLEPROFILE_CHAR_LEN_MAX, pass_word);
        #endif
      }
    break;

    default:
      security_state = PASS_CODE_FALSE;
    break;
  }

}

void reset_password(void)
{
  int word;
  for(word = 0; word < 4; ++word)
  {
    pass_word[word] = number_to_ASCII( word+1 );
  }
  for(word = 4; word < SIMPLEPROFILE_CHAR_LEN_MAX; ++word)
  {
    pass_word[word] = 'f';
  }
  //osal_snv_write( NVID_PASSWORD, SIMPLEPROFILE_CHAR_LEN_MAX, pass_word );
}

void PWMCB(void)
{
  //do not thing.
}

void power_on(uint8_t *pcmd)
{
  uint8_t port = Str_to_number(pcmd[5]);
  if( port > MAX_SIGNAL_PORT )
    return;
  else
    POWER_SW_state[port] = true;

// hal	
#if defined HAL_BUZZER

#elif defined HAL_SENSOR

#else
  //LED
#endif
//hal
}

void SW_power_off(uint8_t *pcmd)
{
	uint8_t port = Str_to_number(pcmd[5]);
	if( port > MAX_SIGNAL_PORT )
	  return;
	else
	  POWER_SW_state[port] = FALSE;
	
#if (defined HAL_BUZZER) && (HAL_BUZZER == true)
	HalBuzzerStop();
	HCI_EXT_ClkDivOnHaltCmd( HCI_EXT_ENABLE_CLK_DIVIDE_ON_HALT );
#elif (defined HAL_SENSOR) && (HAL_SENSOR == true)
	
  #if defined (EVN_SENSOR)
	//HAL_IO_OFF(POWER_LED);
	//HAL_IO_OFF(POWER_SIGNAL);
  #endif
#else
	HalLedSet(HAL_LED_1, HAL_LED_MODE_OFF);
#endif //HAL_BUZZER
}

uint8_t Str_to_number( uint8_t num_char )
{
  if(num_char>=0x30 && num_char<=0x39)
  {
    num_char = num_char-0x30;
    return num_char;
  }
  else
    return 0;
}

uint8_t number_to_ASCII( uint8_t num_value )
{
  uint8_t state = FAILURE;
  if(num_value<=15)
  {
    state = SUCCESS;
    if(num_value<=9)
    {
      num_value = num_value+0x30; // 0 = 0x30
    }
    else
    {
      num_value = (num_value-10)+0x61; // a = 0x61
    }
		return num_value;
  }
	else
	{
		return state;
	}
}

CmdReturn ctrl_command_service( uint8_t *pcmd )
{
  uint8_t newValue;
  uint8_t device_port = Str_to_number(pcmd[5]);
  CmdReturn Return_CMD = CMD_RETURN_MAX;
	
  if(device_port > MAX_SIGNAL_PORT)
  {
    return Return_CMD;
  }

	if( security_state == PASS_CODE_ACCEPT )
	{
  //Command format: s,sw,1,on,0000,000,e
  //                01234567890123456789
#ifdef ILO 
		if(pcmd[7] == 0x70 && pcmd[8] == 0x77) //PWM
		{   
			newValue = 100*Str_to_number(pcmd[11]);
			newValue = 10*Str_to_number(pcmd[12]) + newValue; 
			newValue = Str_to_number(pcmd[13]) + newValue;
							
			//When Value is 0 ,timer is full duty ,so +1 to get right value.
			newValue = 256-newValue; //inverse
			if(newValue == 1)
			{
				HalBuzzerStop();
				HCI_EXT_ClkDivOnHaltCmd( HCI_EXT_ENABLE_CLK_DIVIDE_ON_HALT );
			}
			else
			{
				HCI_EXT_ClkDivOnHaltCmd( HCI_EXT_DISABLE_CLK_DIVIDE_ON_HALT );
				HalBuzzerRing(newValue, 1, PWMCB);
			}
		}
		else if(pcmd[7] == 0x6f && pcmd[8] == 0x66) //when command off, PWM on(Inverse logic)
		{
			#if (defined HAL_BUZZER) && (HAL_BUZZER == true)
				HCI_EXT_ClkDivOnHaltCmd( HCI_EXT_DISABLE_CLK_DIVIDE_ON_HALT );
				HalBuzzerRing(0, 0, PWMCB);
			#else //HAL_BUZZER
				HalLedSet (HAL_LED_1,HAL_LED_MODE_OFF);
			#endif //HAL_BUZZER
		}
		else if(pcmd[7] == 0x6f && pcmd[8] == 0x6e) //when command is on, PWM off(Inverse logic)
		{
			#if (defined HAL_BUZZER) && (HAL_BUZZER == true)
				HalBuzzerStop();
				HCI_EXT_ClkDivOnHaltCmd( HCI_EXT_ENABLE_CLK_DIVIDE_ON_HALT );
			#else //HAL_BUZZER
				HalLedSet (HAL_LED_1,HAL_LED_MODE_ON);
			#endif //HAL_BUZZER
		}
#else //ILO
		if(pcmd[7] == 0x70 && pcmd[8] == 0x77) //pw:PWM
		{   
			newValue = 100*Str_to_number(pcmd[11]);
			newValue = 10*Str_to_number(pcmd[12]) + newValue; 
			newValue = Str_to_number(pcmd[13]) + newValue;
			//When Value is 0 ,timer is full duty ,so +1 to get right value.
			newValue = newValue+1;
			if(newValue == 1)
			{
				POWER_SW_state[device_port] = FALSE;
				HalBuzzerStop();
				HCI_EXT_ClkDivOnHaltCmd( HCI_EXT_ENABLE_CLK_DIVIDE_ON_HALT );
			}
			else
			{
				POWER_SW_state[device_port] = true;
				HCI_EXT_ClkDivOnHaltCmd( HCI_EXT_DISABLE_CLK_DIVIDE_ON_HALT );
				HalBuzzerRing(newValue, 1, PWMCB);
			}
		}
		else if(pcmd[7] == 0x6f && pcmd[8] == 0x6e) //on
		{
			SW_power_on( pcmd );
			Return_CMD = DEVICE_ON;
		}
		else if(pcmd[7] == 0x6f && pcmd[8] == 0x66) //off
		{
			SW_power_off( pcmd );
			Return_CMD = DEVICE_OFF;
		}
		else if(pcmd[7] == 0x74 && pcmd[8] == 0x6d) //tm:timer
		{
			write_timer_from_cmd(device_port, pcmd);
			Return_CMD = TIMER_SAVE_NV;
		}
		else if(pcmd[7] == 0x73 && pcmd[8] == 0x79) //sy
		{ 
			Return_CMD = SYSTEM_SYNC;
		}
		else if(pcmd[7] == 'p' && pcmd[8] == 's') //ps password
		{ 
			if( pcmd[10] == 'p' && pcmd[11] == 's' && pcmd[12] == 'e' && pcmd[13] == 't' ) // passcode set
			{
				Return_CMD = KEY_SAVE_NV;
			}
		}
	}
	else if(security_state == PASS_CODE_TRUE)
	{
		if(pcmd[7] == 'p' && pcmd[8] == 's') //ps password
		{ 
      if( pcmd[10] == 'c' && pcmd[11] == 'o' && pcmd[12] == 'n' && pcmd[13] == 'n' ) // connect
			{
				security_state = PASS_CODE_ACCEPT;
				Return_CMD = HOST_CONTROLLABLE;
			}
		}
	}
	else if( security_state == PASS_CODE_FALSE )
	{
	  Return_CMD = CONNECT_FAIL;
	}
#endif //ILO
		
  #if (defined HAL_LCD) && (HAL_LCD == true)
		HalLcdWriteString( "command :",  HAL_LCD_LINE_4 );
		HalLcdWriteString( (char*)pcmd, HAL_LCD_LINE_5 );
  #endif // (defined HAL_LCD) && (HAL_LCD == true)
	
	return Return_CMD;
}

uint8_t SW_cmd_set( uint8_t* pcmd, uint8_t port, CmdType CMD, uint32_t ASCII_0000, uint32_t ASCII_000, bool feedback )
{
  uint8_t  state;
	uint8_t* SW_cmd;
  
	if( pcmd != NULL)
	{
		SW_cmd = pcmd;
	}
	else
	{
		uint8_t  new_SW_cmd[SIMPLEPROFILE_CHAR_LEN_MAX];
		SW_get_cmd_prototype(new_SW_cmd);
	  SW_cmd = new_SW_cmd;
	}
	
  if(port > MAX_SIGNAL_PORT)
    return FAILURE;
  else
    SW_cmd[5] = number_to_ASCII( port );

	if( ASCII_0000 != NO_CMD_DATA )
	{
		SW_cmd[10] = BREAK_uint32_t(ASCII_0000, 3);
		SW_cmd[11] = BREAK_uint32_t(ASCII_0000, 2);
		SW_cmd[12] = BREAK_uint32_t(ASCII_0000, 1);
		SW_cmd[13] = BREAK_uint32_t(ASCII_0000, 0);
	}
	
	if( ASCII_000 != NO_CMD_DATA )
	{
		SW_cmd[15] = BREAK_uint32_t(ASCII_000, 2);
		SW_cmd[16] = BREAK_uint32_t(ASCII_000, 1);
		SW_cmd[17] = BREAK_uint32_t(ASCII_000, 0);	
	}

	switch(CMD)
	{
    case CMD_ON:
			SW_cmd[7] = 0x6f; //o
			SW_cmd[8] = 0x6e; //n
			break;
    case CMD_OFF:
			SW_cmd[7] = 0x6f; //o
			SW_cmd[8] = 0x66; //f
			break;
    case CMD_PWM:		
			SW_cmd[7] = 0x70; //p
			SW_cmd[8] = 0x77; //w
			break;
    case CMD_H:
			SW_cmd[7] = 0x68; //h
			SW_cmd[8] = 0x68; //h
			break;
		case CMD_S:
			SW_cmd[7] = 0x73; //s
			SW_cmd[8] = 0x73; //s
			break;
		case CMD_V:
			SW_cmd[7] = 0x76; //v
			SW_cmd[8] = 0x76; //v
			break;
		case CMD_TM:
			SW_cmd[7] = 0x74; //t
			SW_cmd[8] = 0x6d; //m
			break;
		case CMD_SY:
			SW_cmd[7] = 0x73; //s
			SW_cmd[8] = 0x79; //y
			break;
		case CMD_PS:
			SW_cmd[7] = 0x70; //p
			SW_cmd[8] = 0x73; //s
			break;
		default:
			return FAILURE;
			
	}
	uartprint( (char*)SW_cmd, 880+feedback);
	if(feedback)
	{
                state = SimpleProfile_SetParameter( SIMPLEPROFILE_CHAR3, SIMPLEPROFILE_CHAR_LEN_MAX, SW_cmd);
	}
	else
	{
		state = SimpleProfile_SetParameter( SIMPLEPROFILE_CHAR4, SIMPLEPROFILE_CHAR_LEN_MAX, SW_cmd);
	}
	
  return state;
}

void get_cmd_prototype( uint8_t* pcmd )
{
  uint8_t  SW_cmd_prototype[SIMPLEPROFILE_CHAR_LEN_MAX] = 
  {
    
    's', '_',                //0
    't', 'm', '_',           //2 3
    0, '_',                  //5
    0, 0, '_',               //7 8
    '0', '0', '0', '0', '_', //10 11 12 13
    '0', '0', '0', '_',      //15 16 17
    'e'                      //19
      
//    0x73, 0x2c, //"s" ","                               0
//    0x73, 0x77, 0x2c, //"s" "w" ","                     2  3
//    0x00, 0x2c, //"0x00" ","                            5
//    0x00, 0x00, 0x2c, //"0x00" "0x00" ","               7  8
//    0x30, 0x30, 0x30, 0x30, 0x2c, //"0" "0" "0" "0" "," 10 11 12 13
//    0x30, 0x30, 0x30, 0x2c, //"0" "0" "0" ","           15 16 17
//    0x65 //"e"                                          19
  };
  memcpy( pcmd, SW_cmd_prototype, SIMPLEPROFILE_CHAR_LEN_MAX);
}

void Send_BME280_data( bme280data_t data_t, int32 data )
{
	uint8_t cmd[SIMPLEPROFILE_CHAR_LEN_MAX]={0}; //array for command's string
	get_cmd_prototype(cmd);
	cmd[2] = 's';
	cmd[3] = 's';
	cmd[5] = '1';
	switch( data_t )
	{
		case T_DATA:
			cmd[5] = '2';
			cmd[7] = 't';
			cmd[8] = 'p';
			if(data<0)
			{
				cmd[10] = '-';
				data = data*(-1);
			}
			else
			{
				cmd[10] = '+';
			}
			cmd[11] = (data/10000)+ '0';
		  data = data%10000;
			cmd[12] = (data/1000)+ '0';
			data = data%1000;
			cmd[13] = (data/100)+ '0';
			data = data%100;
			
			cmd[15] = (data/10)+ '0';
			cmd[16] = (data%10)+ '0';
			cmd[17] = '0';
			break;
		case P_DATA:
			cmd[5] = '4';
			cmd[7] = 'p';
			cmd[8] = 'r';
			
			cmd[10] = (data/100000)+ '0';
			data = data%100000;
			cmd[11] = (data/10000)+ '0';
			data = data%10000;
			cmd[12] = (data/1000)+ '0';
			data = data%1000;
			cmd[13] = (data/100)+ '0';
			data = data%100;
			cmd[15] = (data/10)+ '0';
			cmd[16] = (data%10)+ '0';
			cmd[17] = '0';
			break;
		case H_DATA:
			
                        data = (data*1000)/1024;
                        cmd[5] = '3';
                        cmd[7] = 'h';
                        cmd[8] = 'u';

                        cmd[10] = '0';
                        cmd[11] = (data/100000)+ '0';
                        data = data%100000;
                        cmd[12] = (data/10000)+ '0';
                        data = data%10000;
                        cmd[13] = (data/1000)+ '0';
                        data = data%1000;

                        cmd[15] = (data/100)+ '0';
                        data = data%100;
                        cmd[16] = (data/10)+ '0';
                        cmd[17] = (data%10)+ '0';

			break;
			
	}
	uartprint(cmd,535); //sensor data trans
	//SimpleProfile_SetParameter( SIMPLEPROFILE_CHAR3, SIMPLEPROFILE_CHAR_LEN_MAX, cmd);
	
}

uint8_t Single_digits_to_ascii(uint8_t vaule)
{
  vaule = (vaule % 10) + '0';
	return vaule;
}

uint8_t Tens_digits_to_ascii(uint8_t vaule)
{
	vaule = (vaule / 10) + '0';
	return vaule;
}

uint8_t Hundreds_digit_to_ascii(uint16_t vaule)
{
	vaule = (vaule / 100) + '0';
	return vaule;
}
