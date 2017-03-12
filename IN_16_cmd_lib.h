#ifndef _CMD_LIB_H
#define _CMD_LIB_H

#include "nrf_global.h"

#define NO_CMD_DATA                     0
#define SIMPLEPROFILE_CHAR_LEN_MAX      20


typedef enum {
  CMD_ON,
  CMD_OFF,
  CMD_PWM,
  CMD_H,
  CMD_S,
  CMD_V,
  CMD_TM,
  CMD_SY,
  CMD_PS,
  CMD_TYPE_MAX
} CmdType;

typedef enum {
  TIMER_SAVE_NV,
  SYSTEM_SYNC,
  HOST_CONTROLLABLE,
  KEY_SAVE_NV,
  CONNECT_FAIL,
  DEVICE_ON,
  DEVICE_OFF,
  CMD_RETURN_MAX
} CmdReturn;

typedef enum {
  PASS_CODE_FALSE,
  PASS_CODE_TRUE,
  PASS_CODE_CHANGE,
  PASS_CODE_ACCEPT,
  PASS_CODE_MAX
} SecurityState;

enum {
  FAILURE,
  SUCCESS
};

/*********************************************************************/
/* security extern variable */
extern uint8_t pass_word[SIMPLEPROFILE_CHAR_LEN_MAX];
extern uint8_t pass_word_temp[SIMPLEPROFILE_CHAR_LEN_MAX];
extern SecurityState  security_state;


/* function */
extern uint8_t Str_to_number( uint8_t num_char );
extern uint8_t number_to_ASCII( uint8_t num_value );
extern CmdReturn ctrl_command_service( uint8_t *pcmd );
extern CmdReturn password_service( uint8_t *pcmd );
extern uint8_t cmd_set( uint8_t* pcmd, uint8_t port, CmdType CMD, uint32_t ASCII_0000, uint32_t ASCII_000, bool feedback );
extern uint8_t Single_digits_to_ascii(uint8_t vaule);
extern uint8_t Tens_digits_to_ascii(uint8_t vaule);
extern uint8_t Hundreds_digit_to_ascii(uint16_t vaule);
extern void get_cmd_prototype( uint8_t* pcmd );
extern void power_on(uint8_t *pcmd);
extern void power_off(uint8_t *pcmd);
extern void reset_password(void);
extern void check_password(uint8_t* p_password);
extern void reset_timer(void);
extern void function_Init( uint8_t task_id );
#endif //_CMD_LIB_H