#ifndef PROCESS_CAN_H
#define PROCESS_CAN_H

#include "driver/twai.h"

#define PROCESS_RX_TASK_INTERVAL_MS 1
#define BRAKE_POS_CAN_ID 0x404020F
#define BRAKE_PRES_RAW_CAN_ID 0x403020F
#define THROTTLE_A_CAN_ID 0x401020F
#define THROTTLE_B_CAN_ID 0x402020F
#define STEER_RAW_CAN_ID 0x405020F

#define MESSAGE_STATUS_CAN_ID 0x8060F02

#define BRAKE_PRES_RAW_DAC_SET 1
#define BRAKE_POS_DAC_SET 2
#define STEER_RAW_DAC_SET 4
#define THROTTLE_A_DAC_SET 8
#define THROTTLE_B_DAC_SET 16

extern twai_message_t message_status;

void process_rx_task (void * pvParameters);

#endif/*PROCESS_CAN_H*/
