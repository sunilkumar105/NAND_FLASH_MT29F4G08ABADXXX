#include "pico/stdlib.h"
#include <stdio.h>

// CONTROLS PIN
#define Read_Beasy_2 0         // R/B2/#^3 -->PIN6
#define Read_Beasy_1 1         // R/B/#^ ---->PIN7
#define Read_Enable 2          // RE# ------->PIN8
#define Chip_Enable_1 3        // CE# ------->PIN9
#define Chip_Enable_2 4        // CE2#^3----->PIN10
#define Command_Latch_Enable 5 // CLE ------->PIN16
#define Address_Latch_Enable 6 // ALE ------->PIN17
#define Write_Enable 7         // WE -------->PIN18
#define Write_Protect 8        // WP -------->PIN19

// DATA PIN
#define IO_0 16
#define IO_1 17
#define IO_2 18
#define IO_3 19
#define IO_4 20
#define IO_5 21
#define IO_6 22
#define IO_7 26

#define UART_ID uart0
#define BAUD_RATE 115200

const uint data_bus_pins[8] = {IO_0, IO_1, IO_2, IO_3, IO_4, IO_5, IO_6, IO_7};
// const uint data_bus_pins[8] = {IO_7, IO_6, IO_5, IO_4, IO_3, IO_2, IO_1, IO_0};

const uint LED_PIN = PICO_DEFAULT_LED_PIN;

void Nand_Flash_Gpio_Init(void);
void Enable_Address_Latch_Pin(void);
void Disable_Address_Latch_Pin(void);
/*ACTIVE HIGH
Loads an address from I/O[7:0] into the address register
*/
void Chip_Pin_Enable(void);
void Chip_Pin_Disable(void);
/* ACTIVE LOW
Enables or disables one or more die (LUNs) in a target*/
void Enable_Command_Latch_Pin(void);
void Disable_Command_Latch_Pin(void);
/*ACTIVE HIGH
Loads a command from I/O[7:0] into the command register*/
void Enable_Read_Pin(void);
void Disable_Read_Pin(void);
/* ACTIVE LOW
 Transfers serial data from the NAND Flash to the host system*/
void Enable_Write_Pin(void);
void Disable_Write_Pin(void);
/*ACTIVE LOW
Transfers commands, addresses, and serial data from the host system to the
NAND Flash.*/
void Enable_Write_Protect_Pin(void);
void Disable_Write_Protect_Pin(void);
/* ACTIVE LOW
Enables or disables array PROGRAM and ERASE operations.
*/
void Enable_Read_Busy_Pin(void);
void Disable_Read_Busy_Pin(void);
/*ACTIVE LOW
An open-drain, active-low output that requires an external pull-up resistor.
This signal indicates target array activity.
*/

void Set_Data_Bus_As_Input();
void Set_Data_Bus_As_Output();

uint8_t Read_Data_Bus();
void Write_Data_Bus(uint8_t Data);

// READ WRITE OPERATION FUNCTIONS
uint8_t Read_Data(void);
void Write_Data(uint8_t Data);
void NAND_Idle_Bus(void);
void nandSendCommand(uint8_t cmd, bool last = true);
void nandReadSignature(void);
void nandEnable(bool e);

int main()
{
    stdio_init_all();
    // uart_init(UART_ID, BAUD_RATE);
    

    while (true)
    {
        while(getchar_timeout_us(10) != '/');
        printf("INITIALIZING NAND \n");
        gpio_init(LED_PIN);
        gpio_set_dir(LED_PIN, GPIO_OUT);

        Nand_Flash_Gpio_Init();
        NAND_Idle_Bus();

        nandEnable(true);
        nandReadSignature();
        gpio_put(LED_PIN, 1);
        sleep_ms(250);
        gpio_put(LED_PIN, 0);
        sleep_ms(250);
        // printf("IN VOID LOOP \n");
    }
}

void Nand_Flash_Gpio_Init(void)
{
    // INIT GPIO INTENDED TO BE USED
    gpio_init(Read_Beasy_2);
    gpio_init(Read_Beasy_1);
    gpio_init(Read_Enable);
    gpio_init(Chip_Enable_1);
    gpio_init(Chip_Enable_2);
    gpio_init(Command_Latch_Enable);
    gpio_init(Address_Latch_Enable);
    gpio_init(Write_Enable);
    gpio_init(Write_Protect);

    // SET INPUT/OUTPUT DIRECTION OF COMMAND PINS
    gpio_set_dir(Read_Beasy_2, GPIO_IN);
    gpio_set_dir(Read_Beasy_1, GPIO_IN);
    gpio_pull_up(Read_Beasy_1);
    gpio_set_dir(Read_Enable, GPIO_OUT);
    gpio_set_dir(Chip_Enable_1, GPIO_OUT);
    gpio_set_dir(Chip_Enable_2, GPIO_OUT);
    gpio_set_dir(Command_Latch_Enable, GPIO_OUT);
    gpio_set_dir(Address_Latch_Enable, GPIO_OUT);
    gpio_set_dir(Write_Enable, GPIO_OUT);
    gpio_set_dir(Write_Protect, GPIO_OUT);

    // INITIALIZE IO GPIO
    gpio_init(IO_0);
    gpio_init(IO_1);
    gpio_init(IO_2);
    gpio_init(IO_3);
    gpio_init(IO_4);
    gpio_init(IO_5);
    gpio_init(IO_6);
    gpio_init(IO_7);
}

void Enable_Address_Latch_Pin(void) // ACTIVE HIGH
{
    gpio_put(Address_Latch_Enable, 1);
}
void Disable_Address_Latch_Pin(void)
{
    gpio_put(Address_Latch_Enable, 0);
}

void Enable_Chip_Enable_Pin(void) // ACTIVE LOW
{
    gpio_put(Chip_Enable_1, 1);
}
void Disable_Chip_Disable_Pin(void)
{
    gpio_put(Chip_Enable_1, 0);
}

void Enable_Command_Latch_Pin(void) // ACTIVE HIGH
{
    gpio_put(Command_Latch_Enable, 1);
}
void Disable_Command_Latch_Pin(void)
{
    gpio_put(Command_Latch_Enable, 0);
}

void Enable_Read_Pin(void) // ACTIVE LOW
{
    gpio_put(Read_Enable, 1);
}
void Disable_Read_Pin(void)
{
    gpio_put(Read_Enable, 0);
}

void Enable_Write_Pin(void) // ACTIVE LOW
{
    gpio_put(Write_Enable, 1);
}

void Disable_Write_Pin(void)
{
    gpio_put(Write_Enable, 0);
}

void Enable_Write_Protect_Pin(void) // ACTIVE LOW
{
    gpio_put(Write_Protect, 1);
}
void Disable_Write_Protect_Pin(void)
{
    gpio_put(Write_Protect, 0);
}

void Enable_Read_Busy_Pin(void) // ACTIVE LOW
{
    gpio_put(Read_Beasy_1, 1);
}
void Disable_Read_Busy_Pin(void)
{
    gpio_put(Read_Beasy_1, 0);
}

void Set_Data_Bus_As_Input()
{
    gpio_set_dir(IO_0, GPIO_IN);
    gpio_set_dir(IO_1, GPIO_IN);
    gpio_set_dir(IO_2, GPIO_IN);
    gpio_set_dir(IO_3, GPIO_IN);
    gpio_set_dir(IO_4, GPIO_IN);
    gpio_set_dir(IO_5, GPIO_IN);
    gpio_set_dir(IO_6, GPIO_IN);
    gpio_set_dir(IO_7, GPIO_IN);
}

void Set_Data_Bus_As_Output()
{
    gpio_set_dir(IO_0, GPIO_OUT);
    gpio_set_dir(IO_1, GPIO_OUT);
    gpio_set_dir(IO_2, GPIO_OUT);
    gpio_set_dir(IO_3, GPIO_OUT);
    gpio_set_dir(IO_4, GPIO_OUT);
    gpio_set_dir(IO_5, GPIO_OUT);
    gpio_set_dir(IO_6, GPIO_OUT);
    gpio_set_dir(IO_7, GPIO_OUT);
}

// printf("Data Written: 0x%02X\n", data_to_write);
// printf("Data Read:    0x%02X\n", data_read);
uint8_t Read_Data_Bus()
{
    uint8_t Read_Data;
    // READ DATA BUS
    for (int i = 0; i < 8; i++)
    {
        Read_Data |= (gpio_get(data_bus_pins[i]) << i);
    }
    return Read_Data;
}

void Write_Data_Bus(uint8_t Data)
{
    for (int i = 0; i < 8; i++)
    {
        // Set the GPIO pin to the corresponding data bit
        gpio_put(data_bus_pins[i], (Data >> i) & 0x01);
    }
}

uint8_t Read_Data(void)
{
    Set_Data_Bus_As_Input();
    uint8_t Data_Read = Read_Data_Bus();
    return Data_Read;
}

void Write_Data(uint8_t Data)
{
    Set_Data_Bus_As_Output();
    Write_Data_Bus(Data);
}
uint8_t dataInput();
void sendAddress(uint8_t addr);
void nandReadSignature()
{
    uint8_t sm = 0x00, sd = 0x00;

    nandSendCommand(0x90);
    sendAddress(0x00);
    // Enable_Address_Latch_Pin();
    // Disable_Write_Pin();
    // Write_Data(0x00);
    // Enable_Write_Pin();
    // Disable_Address_Latch_Pin();

    // // Disable_Read_Pin();
    // sm = dataInput();
    // // Enable_Read_Pin();
    // // Disable_Read_Pin();
    // sd = dataInput();
    // // Enable_Read_Pin();

    // printf("Nand Manufacturer: 0x%02X\n", sm);
    // printf("NAND device ID: 0x%02X\n", sd);

    for(uint8_t i = 0; i < 5; i++){
        sm = dataInput();
        printf("0x%02X ", sm);
    }
    printf("\n");

    nandSendCommand(0x90);
    sendAddress(0x20);
    for(uint8_t i = 0; i < 5; i++){
        sm = dataInput();
        printf("0x%02X ", sm);
    }
    printf("\n");

    nandSendCommand(0xEC);
    sendAddress(0x00);
    for(uint8_t i = 0; i < 44; i++){
        sm = dataInput();
        printf("0x%02X ", sm);
    }
    printf("\n");
}

void nandSendCommand(uint8_t cmd, bool last)
{
    Disable_Write_Pin();
    Enable_Read_Pin();
    Disable_Address_Latch_Pin();
    Enable_Command_Latch_Pin();
    
    Write_Data(cmd);
    Enable_Write_Pin();
    
    Disable_Command_Latch_Pin();
}

void sendAddress(uint8_t addr){
    Disable_Command_Latch_Pin();
    Disable_Write_Pin();
    Enable_Read_Pin();
    Enable_Address_Latch_Pin();

    Write_Data(addr);
    Enable_Write_Pin();
    Disable_Address_Latch_Pin();
}

uint8_t dataInput(){
    Disable_Command_Latch_Pin();
    Disable_Address_Latch_Pin();
    Enable_Write_Pin();

    Enable_Read_Pin();
    Disable_Read_Pin();
    uint8_t data = Read_Data();
    Enable_Read_Pin();
    return data;
}

void NAND_Idle_Bus()
{
    Enable_Read_Pin();
    Enable_Write_Pin();
    Disable_Command_Latch_Pin();
    Disable_Address_Latch_Pin();
    Enable_Chip_Enable_Pin();
}
void nandEnable(bool e)
{
    gpio_put(Chip_Enable_1, !e);
}

