//****************************************************************************
// MIT Media Lab - Biomechatronics
// Jean-Francois (Jeff) Duval
// jfduval@media.mit.edu
// 03/2016
//****************************************************************************
// flexsea: Master file for the FlexSEA stack.
//****************************************************************************

// FlexSEA: Flexible & Scalable Electronics Architecture

// v0.0 Limitations and known bugs:
// ================================
// - The board config is pretty much fixed, at compile time.
// - Only 1 command per transmission
// - Fixed payload length: 12 bytes (allows you to send 1 command with up to
//   8 arguments (uint8) (or 4x uint16))
// - Fixed comm_str length: 24 bytes (min. to accomodate a payload where all the
//   data bytes need escaping)
// - In comm_str #OfBytes isn't escaped. Ok as long as the count is less than
//   the decimal value of the flags ('a', 'z', 'e') so max 97 bytes.
// - Data transfer could be faster with shorter ACK sequence, more than 1 command
//   per packet and no repackaging on the Manage board (straight pass-through)
//   To be optimized later.

#ifdef __cplusplus
extern "C" {
#endif

//****************************************************************************
// Include(s)
//****************************************************************************

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
#include "../inc/flexsea.h"
#include "flexsea_system.h"
#include "flexsea_board.h"

//****************************************************************************
// Variable(s)
//****************************************************************************

unsigned char test_payload[PAYLOAD_BUF_LEN];

//****************************************************************************
// Private Function Prototype(s)
//****************************************************************************

static void clear_rx_command(uint8_t x, uint8_t y, uint8_t rx_cmd[][PACKAGED_PAYLOAD_LEN]);

//****************************************************************************
// Public Function(s)
//****************************************************************************

//When something goes wrong in the code it will land here:
unsigned int flexsea_error(unsigned int err_code)
{
	//ToDo something useful
	return err_code;
}

//From 1 uint32 to 4 uint8
void uint32_to_bytes(uint32_t x, uint8_t *b0, uint8_t *b1, uint8_t *b2, uint8_t *b3)
{
	*b0 = (uint8_t) ((x >> 24) & 0xFF);
	*b1 = (uint8_t) ((x >> 16) & 0xFF);
	*b2 = (uint8_t) ((x >> 8) & 0xFF);
	*b3 = (uint8_t) (x & 0xFF);
}

//From 1 uint16 to 2 uint8
void uint16_to_bytes(uint32_t x, uint8_t *b0, uint8_t *b1)
{
	*b0 = (uint8_t) ((x >> 8) & 0xFF);
	*b1 = (uint8_t) (x & 0xFF);
}

//ToDo: look at inline functions

//Can be used to fill a buffer of any length with any value
void fill_uint8_buf(uint8_t *buf, uint32_t len, uint8_t filler)
{
	uint32_t i = 0;

	for(i = 0; i < len; i++)
	{
		buf[i] = filler;
	}
}

//Quick way to debug the comm functions with the debugger and the terminal.
//Make sure to enable the printf statements.
void test_flexsea_stack(void)
{
    uint8_t i = 0, res = 0, bytes = 0;

    //We are using a command that Plan can receive to test the parser too:
    bytes = tx_cmd_test(FLEXSEA_PHOTON_1, CMD_WRITE, test_payload, PAYLOAD_BUF_LEN, 100, 200);
    //(this fills payload_str[])

    DEBUG_PRINTF("bytes = %i\n", bytes);

    //Clear current payload:
    clear_rx_command(PAYLOAD_BUFFERS, PACKAGED_PAYLOAD_LEN, rx_command_1);

    //Build comm_str
    res = comm_gen_str(test_payload, comm_str_1, bytes);

    DEBUG_PRINTF("comm_str[]: >> %s <<\n", (char*)comm_str_spi);
    DEBUG_PRINTF("res = %i\n", res);

    DEBUG_PRINTF("\nrx_buf_spi[]: >> %s <<\n", (char*)rx_buf_spi);

    //Feed it to the input buffer
    for(i = 0; i < PACKAGED_PAYLOAD_LEN; i++)
    {
        update_rx_buf_byte_1(comm_str_1[i]);
    }

    DEBUG_PRINTF("rx_buf_spi[]: >> %s <<\n", (char*)rx_buf_spi);

    //Try to decode
    res = unpack_payload_1();

    DEBUG_PRINTF("Found %i payload(s).\n", res);

    //Can we parse it?

    res = payload_parse_str(rx_command_1[0]);

    //If it works, the console/terminal should display
    //"Received CMD_TEST. Val1 = 200, Val2 = 100."
}

//Transmission of a TEST command
uint32_t tx_cmd_test(uint8_t receiver, uint8_t cmd_type, uint8_t *buf, uint32_t len, int16_t val1, int16_t val2)
{
	uint8_t tmp0 = 0, tmp1 = 0;
	uint32_t bytes = 0;

	//Fresh payload string:
	prepare_empty_payload(board_id, receiver, buf, len);

	//Command:
	buf[P_CMDS] = 1;                     //1 command in string

	if(cmd_type == CMD_READ)
	{
		buf[P_CMD1] = CMD_R(CMD_TEST);

		bytes = P_CMD1 + 1;     //Bytes is always last+1
	}
	else if(cmd_type == CMD_WRITE)
	{
		buf[P_CMD1] = CMD_W(CMD_TEST);

		//Arguments:
		uint16_to_bytes(val1, &tmp0, &tmp1);
		buf[P_DATA1] = tmp0;
		buf[P_DATA1 + 1] = tmp1;
		uint16_to_bytes(val2, &tmp0, &tmp1);
		buf[P_DATA1 + 2] = tmp0;
		buf[P_DATA1 + 3] = tmp1;

		bytes = P_DATA1 + 4;     //Bytes is always last+1
	}
	else
	{
		//Invalid
		flexsea_error(SE_INVALID_READ_TYPE);
		bytes = 0;
	}

	return bytes;
}

//Reception of a TEST command
void rx_cmd_test(uint8_t *buf)
{
	uint32_t numb = 0;
	int16_t tmp_val1 = 0, tmp_val2 = 0;

	if(IS_CMD_RW(buf[P_CMD1]) == READ)
	{
		//Received a Read command from our master, prepare a reply:

        //...
	}
	else if(IS_CMD_RW(buf[P_CMD1]) == WRITE)
	{
		//Two options: from Master of from slave (a read reply)

		//Decode data:
		tmp_val1 = (int16_t) (BYTES_TO_UINT16(buf[P_DATA1], buf[P_DATA1+1]));
		tmp_val2 = (int16_t) (BYTES_TO_UINT16(buf[P_DATA1+2], buf[P_DATA1+3]));
		//ToDo store that value somewhere useful

		if(sent_from_a_slave(buf))
		{
			//We received a reply to our read request

			DEBUG_PRINTF("Received CMD_TEST_REPLY. Val1 = %i, Val2 = %i.\n", tmp_val1, tmp_val2);
		}
		else
		{
			//Master is writing a value to this board

			DEBUG_PRINTF("Received CMD_TEST. Val1 = %i, Val2 = %i.\n", tmp_val1, tmp_val2);
		}
	}
}

//****************************************************************************
// Private Function(s)
//****************************************************************************

//Empties the buffer - used by the flexes test function
static void clear_rx_command(uint8_t x, uint8_t y, uint8_t rx_cmd[][PACKAGED_PAYLOAD_LEN])
{
    unsigned char i = 0, j = 0;

    for(i = 0; i < x; i++)
    {
        for(j = 0; j < y; j++)
        {
        	rx_cmd[i][j] = 0;
        }
    }
}

#ifdef __cplusplus
}
#endif
