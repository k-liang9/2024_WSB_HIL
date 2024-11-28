#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_err.h"
#include "driver/twai.h"
#include "driver/spi_master.h"
#include "../Inc/userInit.h"
#include "dac.h"
#include "canReceive.h"
#include "../Inc/processCAN.h"


spi_device_handle_t wheel_enc; //External DAC (double check)
spi_device_handle_t bellcrank_rot_enc; //External DAC
spi_device_handle_t hall_effect; //External DAC
dac_oneshot_handle_t brake_ir_temp; //Internal DAC (Double Check)
dac_oneshot_handle_t brake_press; //Internal DAC
//spi_device_handle_t mc_flow; //PWM (Double check)

void taskRegister (void)
{
    BaseType_t xReturned = pdPASS;
    TaskHandle_t can_rx;
    TaskHandle_t can_process;

    xReturned = xTaskCreate(
        can_rx_task,
        "CAN_RECEIVE_TASK",
        4000,
        ( void * ) NULL,
        configMAX_PRIORITIES-1,
        &can_rx
    );

    if(xReturned != pdPASS)
    {
        while(1)
        {
            printf("Failed to register can_rx_task to RTOS\r\n");
        }
    }

    xReturned = xTaskCreate(
        process_rx_task,
        "CAN_PROCESS_TASK",
        4000,
        ( void * ) NULL,
        configMAX_PRIORITIES-1,
        &can_process
    );

    if(xReturned != pdPASS)
    {
        while(1)
        {
            printf("Failed to register process_rx_task to RTOS\r\n");
        }
    }
}

esp_err_t CAN_init (void) {

    memset(&rx_msg, 0, sizeof(twai_message_t));
    memset(&can_msg, 0, sizeof(twai_message_t));
    memset(&vcu_hil_queue, 0, sizeof(QueueHandle_t));
    memset(&pdu_hil_queue, 0, sizeof(QueueHandle_t));
    memset(&wsb_hil_queue, 0, sizeof(QueueHandle_t));

    twai_general_config_t g_config = {
        .mode = TWAI_MODE_NORMAL, 
        .tx_io = CAN_TX, 
        .rx_io = CAN_RX,
        .clkout_io = TWAI_IO_UNUSED, 
        .bus_off_io = TWAI_IO_UNUSED,      
        .tx_queue_len = MAX_CAN_MSG_QUEUE_LENGTH, 
        .rx_queue_len = MAX_CAN_MSG_QUEUE_LENGTH,                          
        .alerts_enabled = TWAI_ALERT_NONE,  
        .clkout_divider = 0,        
        .intr_flags = ESP_INTR_FLAG_LEVEL1
    };

    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

     if (twai_driver_install(&g_config, &t_config, &f_config) != ESP_OK)
    {
        printf("Failed to install TWAI driver\r\n");
        return ESP_FAIL;
    } 
    printf("TWAI driver installed\r\n");

    if (twai_start() != ESP_OK) 
    {
        printf("Failed to start TWAI driver\r\n");
        return ESP_FAIL;
        
    } 

    printf("TWAI driver started\r\n");
    return ESP_OK;

}

esp_err_t spi_init(void) {
    printf("Initializing SPI bus");

    esp_err_t ret = ESP_OK;

    memset(&brake_press, 0, sizeof(spi_device_handle_t));
    memset(&wheel_enc, 0, sizeof(spi_device_handle_t));
    memset(&bellcrank_rot_enc, 0, sizeof(spi_device_handle_t));
    memset(&brake_press, 0, sizeof(dac_oneshot_handle_t));
    memset(&brake_ir_temp, 0, sizeof(dac_oneshot_handle_t));
    //memset(&mc_flow, 0, sizeof(NO IDEA));

    spi_bus_config_t BusCfg = {
        .mosi_io_num = SPI_MOSI_PIN,
        .sclk_io_num = SPI_CLK_PIN,
        .quadwp_io_num = NOT_USED,
        .quadhd_io_num = NOT_USED,
    };

    //Verify if External DAC or not
    spi_device_interface_config_t wheel_enc_cfg = {
        .clock_speed_hz = 20*HZ_PER_MHZ,           //Clock out at 20 MHz
        .mode = 0,                                //SPI mode 0
        .spics_io_num = WHE_ENC,            //CS pin
        .queue_size = MAX_SPI_QUEUE_SIZE,                          //We want to be able to queue 7 transactions at a time
    };

    spi_device_interface_config_t bc_rot_enc_cfg = {
        .clock_speed_hz = 20*HZ_PER_MHZ,           //Clock out at 20 MHz
        .mode = 0,                                //SPI mode 0
        .spics_io_num = WHE_ENC,            //CS pin
        .queue_size = MAX_SPI_QUEUE_SIZE,                          //We want to be able to queue 7 transactions at a time
    };
    
    //Verify if External DAC or not
    spi_device_interface_config_t hall_effect_cfg = {
        .clock_speed_hz = 20*HZ_PER_MHZ,           //Clock out at 20 MHz
        .mode = 0,                                //SPI mode 0
        .spics_io_num = BC_ROT_ENC,            //CS pin
        .queue_size = MAX_SPI_QUEUE_SIZE,                          //We want to be able to queue 7 transactions at a time
    };
    
    

    dac_oneshot_config_t brake_pres_raw_cfg = {
        .chan_id = DAC_CHAN_0,
    };

    dac_oneshot_config_t brake_ir_temp_cfg = {
        .chan_id = DAC_CHAN_1,
    };

    ret = spi_bus_initialize(SPI2_HOST, &BusCfg, SPI_DMA_CH_AUTO);
    if(ret != ESP_OK){
        printf("failed to init SPI bus %d\r\n", ret);
        return ESP_FAIL;
    }
    
    //Conditionals for SPI stuff coming soon

    ret = dac_oneshot_new_channel(&brake_pres_raw_cfg, &brake_pres_raw);
    if(ret != ESP_OK){
        printf("failed to start dac channel for brake pres raw %d\r\n", ret);
        return ESP_FAIL;
    } 




    return ESP_OK;
}


void app_main(void)
{
    spi_init();
    CAN_init();
    taskRegister();
}
