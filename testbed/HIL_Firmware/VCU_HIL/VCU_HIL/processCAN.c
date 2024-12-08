#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/twai.h"
#include "userInit.h"
#include "dac.h"
#include "canReceive.h"
#include "processCAN.h"

static uint16_t byte_1 = 0U;
static uint16_t byte_2 = 0U;

twai_message_t message_status = {
        .identifier = MESSAGE_STATUS_CAN_ID,
        .extd = EXTENDED_MSG,
        .data_length_code = CAN_MSG_DATA_SIZE,
};

//reassemble data into a 16 bit uint to pass in desired voltage into set dac function
void process_rx_task (void * pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while(1)
    {
        xQueueReceive(vcu_hil_queue, &can_msg, portMAX_DELAY);

        switch (can_msg.identifier)
        {
            case BRAKE_POS_CAN_ID:     
                byte_1 = can_msg.data[0];
                byte_2 = can_msg.data[1];
                byte_2 = byte_2 << 8;
                byte_2 |= byte_1;
                set6551Voltage(byte_2, DacId_BrakePos);
                break;
            case BRAKE_PRES_RAW_CAN_ID:     
                byte_1 = can_msg.data[0];
                byte_2 = can_msg.data[1];
                byte_2 = byte_2 << 8;
                byte_2 |= byte_1;
                setDacVoltage(&brake_pres_raw, BRAKE_PRES_RAW_DAC_SET, &message_status, byte_2);
                break;
            case THROTTLE_A_CAN_ID:     
                byte_1 = can_msg.data[0];
                byte_2 = can_msg.data[1];
                byte_2 = byte_2 << 8;
                byte_2 |= byte_1;
                set6551Voltage(byte_2, DacId_ThrottleA);
                break;
            case THROTTLE_B_CAN_ID:    
                byte_1 = can_msg.data[0];
                byte_2 = can_msg.data[1];
                byte_2 = byte_2 << 8;
                byte_2 |= byte_1;
                set6551Voltage(byte_2, DacId_ThrottleB);
                break;
            case STEER_RAW_CAN_ID:     
                byte_1 = can_msg.data[0];
                byte_2 = can_msg.data[1];
                byte_2 = byte_2 << 8;
                byte_2 |= byte_1;
                set6551Voltage(byte_2, DacId_SteerRaw);
                break;
            default:
                printf("CAN ID not recognized %ld\r\n", can_msg.identifier);
                break;
        }

        vTaskDelayUntil(&xLastWakeTime, PROCESS_RX_TASK_INTERVAL_MS);
    }
}