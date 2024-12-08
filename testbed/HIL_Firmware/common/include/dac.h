#ifndef DAC_H
#define DAC_H

#include "driver/spi_master.h"
#include "driver/dac_oneshot.h"
#include "driver/twai.h"

//the 8 MSB on the DAC6551 is constant
//the power down mode connects a 100kOhm pulldow resistor to Vout
#define BYTE_0 0x02
#define MAX_SPI_QUEUE_LENGTH 24
#define V_REF_MV 3300.0
#define MAX_12_BIT_VAL 2048
#define MAX_8_BIT_VAL 255

#define EXTENDED_MSG 1              //CAN message has extended ID
#define CAN_MSG_DATA_SIZE 1     //size in bytes

//todo: Add WSB Sensors
typedef enum DacId_E{
    DacId_ThrottleA = 0,
    DacId_ThrottleB,
    DacId_BrakePos,
    DacId_SteerRaw,
} DacId_E;

esp_err_t setDacVoltage(dac_oneshot_handle_t *channel, uint8_t dac_set, twai_message_t *can_msg, float voltage);
esp_err_t set6551Voltage (float voltage, DacId_E id, uint8_t dac_set,
                          spi_device_handle_t *spi_device_handle, twai_message_t *can_msg);

#endif/*DAC_H*/