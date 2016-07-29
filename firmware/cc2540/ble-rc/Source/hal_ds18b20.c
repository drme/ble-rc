#include "OSAL.h"
#include "bcomdef.h"
//#include "bletempservice.h"
#include "OSAL_PwrMgr.h"
#include "OnBoard.h"
#include "hal_ds18b20.h"
#include "car_control.h"
//#include "hal_temp.h"

static int readingDS18B20 = 0;

#define delay_ms(ms) HalDS18B20DelayMicroseconds(ms * 1000)
#define delay_us(us) HalDS18B20DelayMicroseconds(us)

// DS18B20 on P0.0
#define DS1820_OUT 					P0
#define DS1820_DIR 					P0DIR
#define DS1820_SEL					P0SEL
#define DS1820_IN					P0
#define DS1820_DATA_IN_PIN          0x01
#define DS1820_SKIP_ROM             0xCC
#define DS1820_READ_SCRATCHPAD      0xBE
#define DS1820_CONVERT_T            0x44

static void HalDS18B20DelayMicroseconds(uint16 duration)
{
	#define NOP() asm("NOP")

	duration >>= 1;

	while (duration-- > 0)
	{
		NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP();
		NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP();
	}
};

static void DS1820_HI()
{
	DS1820_DIR |= DS1820_DATA_IN_PIN;	// set port as output
	DS1820_OUT |= DS1820_DATA_IN_PIN;	// set port high
};

static void DS1820_LO()
{
	DS1820_DIR |= DS1820_DATA_IN_PIN;	  // set port as output
	DS1820_OUT &= ~DS1820_DATA_IN_PIN;	// set port low
};

static unsigned int ResetDS1820(void)
{
	/**
	 * Steps to reset one wire bus
   * Pull bus low
   * hold condition for 480us
   * release bus
   * wait for 60us
   * read bus
   * if bus low then device present set / return var accordingly
   * wait for balance period (480-60)
   */

	int device_present = 0;
	DS1820_LO();						            // Drive bus low
	delay_us(480);						          // hold for 480us
	DS1820_DIR &= ~DS1820_DATA_IN_PIN;	// release bus. set port in input mode

	if (DS1820_IN & DS1820_DATA_IN_PIN)
	{
		device_present = 0;
	}

	delay_us(480);						          // wait for 480us

	return device_present;
};

static void WriteZero(void)		
{
	/*
	 * Steps for master to transmit logical zero to slave device on bus
	 * pull bus low
	 * hold for 60us
	 * release bus
	 * wait for 1us for recovery
	 */
	
	DS1820_LO();         				        // Drive bus low
	delay_us(60);						            // sample time slot for the slave
	DS1820_DIR &= ~DS1820_DATA_IN_PIN;	// release bus. set port in input mode
	delay_us(1);						            // recovery time slot
};

static void WriteOne(void)			
{
	/*
	 * Steps for master to transmit logical one to slave device on bus
	 * pull bus low
	 * hold for 5us
	 * release bus
	 * wait for 1us for recovery
	 */

	DS1820_LO();         				        // Drive bus low
	delay_us(5);
	DS1820_DIR &= ~DS1820_DATA_IN_PIN;	// release bus. set port in input mode
	delay_us(55);						            // sample time slot for the slave
	delay_us(1);						            // recovery time slot
};

static void WriteDS1820(unsigned char data, int power)
{
	for (unsigned char i = 8; i > 0; i--)
	{
		if (data & 0x01)
		{
			WriteOne();
		}
		else
		{
			WriteZero();
		}

		data >>= 1;
	}

	// uncommeted for parasire power
	if (power == 1)
	{
		DS1820_HI();

		delay_ms(10);
	}
};

static unsigned int ReadBit(void)
{
	/*
	 * Steps for master to issue a read request to slave device on bus aka milk slave device
	 * pull bus low
	 * hold for 5us
	 * release bus
	 * wait for 45us for recovery
	 */
  
	int bit = 0;
	DS1820_LO();         				        // Drive bus low
	delay_us(5);  						          // hold for 5us
	DS1820_DIR &= ~DS1820_DATA_IN_PIN;	// release bus. set port in input mode
	delay_us(10);						            // wait for slave to drive port either high or low
    
	if (DS1820_IN & DS1820_DATA_IN_PIN)	// read bus
	{
		bit = 1;						              // if read high set bit high
	}

	delay_us(45);						            // recovery time slot

	return bit;
};

static unsigned int ReadDS1820(void)
{
	unsigned int data = 0;
	
	// release bus. set port in input mode
	DS1820_DIR &= ~DS1820_DATA_IN_PIN;

	for (unsigned char i = 16; i > 0; i--)
	{
		data >>= 1;
		
		if (ReadBit())
		{
			data |=0x8000;
		}
	}
	
	return (data);
};

/*
void HalDS18B20PowerOn()
{
	P0_0 = 1;
};

void HalDS18B20PowerOff()
{
	P0_0 = 0;
};
*/

void HalDS18B20Init()
{
/*	P0SEL &= ~0x3;
	P0DIR |= 0x1;
	P0DIR &= ~0x2;
  
	HalDS18B20PowerOn();*/
};

void HalDS18B20StartReading(uint8 taskId)
{
	if (0 == readingDS18B20)
	{
		readingDS18B20 = 1;
  
		//		HalDS18B20PowerOn();

		ResetDS1820();
		WriteDS1820(DS1820_SKIP_ROM, 0);
		WriteDS1820(DS1820_CONVERT_T, 1);

		osal_start_timerEx(taskId, TEMPERATURE_READING_EVENT, 750);
	}	
};

float HalDS18B20FinishReading(uint16* temperature, uint16* humidity)
{
	if (1 == readingDS18B20)
	{
		ResetDS1820();
		WriteDS1820(DS1820_SKIP_ROM, 0);
		WriteDS1820(DS1820_READ_SCRATCHPAD, 0);
		
		unsigned int temp = ReadDS1820();
	
		float t = 0.0f;
	
		if (temp < 0x8000)
		{
			t = (temp * 0.0625);
		}
		else
		{
			temp = (~temp) + 1;

			t = (temp * 0.0625);
		}
		
		//		HalDS18B20PowerOff();

		readingDS18B20 = 0;
	
		*temperature = (uint16)(t * 100.0f);
		
		return t;
	}
	
	return 0.0f;
};
