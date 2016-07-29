#include "bcomdef.h"

static uint8 Ina220Address = 0x40;
//static uint8 ShuntRegisterAddress = 0x01;
static uint8 BusVoltageRegisterAddress = 0x02;
static uint8 CurrentRegisterAddress = 0x04;
static uint8 CalibrationRegisterAddress = 0x05;

#if defined(HAL_I2C1111)

	#include "hal_ina220.h"
	#include "hal_i2c.h"
	#include "OnBoard.h"

	static i2cClock_t BusRate = i2cClock_267KHZ;

	void PowerOnIna220()
	{
		P0_1 = 0x01;
	};

	void PowerOffIna220()
	{
		P0_1 = 0x00;
	};

	static int SetRegisterValue(uint8 registerAddress, uint16 value)
	{
		uint8 hi = (uint8)(value >> 8);
		uint8 lo = (uint8)value;
  
		HalI2CInit(Ina220Address, BusRate);
	
		uint8 buffer[3] = { registerAddress, hi, lo };
	
		if (HalI2CWrite(3, buffer) != 3)
		{
			return -1;
		}
	
		HalI2CDisable();

		return 0;
	};

	static uint16 ReadRegister(uint8 registerAddress)
	{
		uint8 result[] = { 0xff, 0xff };

		HalI2CInit(Ina220Address, BusRate);

		if (1 != HalI2CWrite(1, &registerAddress))
		{
			HalI2CDisable();

			return 0;
		}

		if (2 != HalI2CRead(2, result))
		{
			HalI2CDisable();

			return 0;
		}

		HalI2CDisable();

		uint16 t = ((uint16)result[0]);

		t = (uint16)(t << 8);
		t |= (uint16)result[1];

		return t;
	};

	void HalIna220Init(uint16 calibrationRegister)
	{
		P0SEL &= ~0x02;
		P0DIR |= 0x02;

		PowerOnIna220();
	
		SetRegisterValue(0x0, 0x06ef);
		SetRegisterValue(CalibrationRegisterAddress, 0xa000);
	};

	void HalIna220DeInit()
	{
		PowerOffIna220();
	};

	void HalIna220UpdateVoltage(uint16* voltage)
	{
		uint16 busVoltage = ReadRegister(BusVoltageRegisterAddress);

		bool hasValue = (busVoltage & 0x2) > 0;
		bool hasOverflow = (busVoltage & 0x1) > 0;

		if (true == hasOverflow)
		{
			*voltage = -1;
		}
		else if (true == hasValue)
		{
			busVoltage = (uint16)(busVoltage >> 3);

			*voltage = (int)((((float)busVoltage) * 4.0f /*/ 1000.0f*/)/* * 10.0f*/);
		}
		else
		{
			*voltage = -2;
		}
	};

	void HalIna220UpdateCurrent(uint16* current)
	{
		*current = ReadRegister(CurrentRegisterAddress);
	};

#elif defined(I2C_BB)

	#include "hal_ina220.h"
	#include "OnBoard.h"	

	static void delayMicroseconds(uint16 duration)
	{
		#define NOP() asm("NOP")

		duration >>= 1;

		while (duration-- > 0)
		{
			NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP();
			NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP();
		}
	};

	static void i2c_wait1()
	{
		asm("nop");
		delayMicroseconds(1 << 1);
	};

	static void SDA_PIN(uint16 v)
	{
  	if (v > 0)
		{
			I2CIO |= 0x01; // SDA
		}
		else
		{
			I2CIO &= ~0x01; // SDA
		}			
	};

	static void SCL_PIN(uint16 v)
	{
  	if (v > 0)
		{
			I2CIO |= 0x02; // SCL
		}
		else
		{
			I2CIO &= ~0x02; // SCL
		}
	};

	#define HIGH 1
	#define LOW  0

	static void i2c_init1()
	{
		// P1SEL	&=	~(SCL | SDA);	// 0 = I/O
		// P1DIR	|=	(SDA);			// 1 = OUTPUT
		SDA_PIN(HIGH);			// P1_6 = SDA
		// P1DIR	|=	(SCL);			// 1 = OUTPUT
		SCL_PIN(HIGH);			// P1_7 = SCL
	};
	
	static unsigned char i2c_write1(unsigned char data)
	{
		// write byte
		for (unsigned char m = 0x80; m != 0; m >>= 1)
		{
			if (m & data)
			{
				SDA_PIN(HIGH);
			}
			else
			{
				SDA_PIN(LOW);
			}

			i2c_wait1();

			SCL_PIN(HIGH);
			i2c_wait1();
			i2c_wait1();

			SCL_PIN(LOW);
			i2c_wait1();
		}

		// get Ack or Nak
		//P1DIR &= ~(SDA); // INPUT
		I2CWC &= ~0x1;
	
		i2c_wait1();

		SDA_PIN(HIGH);
		i2c_wait1();

		SCL_PIN(HIGH);
		i2c_wait1();
		i2c_wait1();

		unsigned char rtn = I2CIO & 0x1;  //SDA_PIN;
		i2c_wait1();
		i2c_wait1();

		SCL_PIN(LOW);
		i2c_wait1();
		i2c_wait1();

		//P1DIR |= (SDA); // OUTPUT
		I2CWC |= 0x1;
	
		i2c_wait1();
		i2c_wait1();

		SDA_PIN(LOW);
		i2c_wait1();
		i2c_wait1();

		return rtn == 0;
	};

	/**
	 * Set up start condition for I2C
	 */
	static unsigned char i2c_start1(unsigned char addressRW)
	{
		SDA_PIN(LOW); // SDA
		i2c_wait1();
		SCL_PIN(LOW); // SCL

		return i2c_write1(addressRW);
	};

	/**
	 * Read 8 bits of I2C data
	 */
	static unsigned char i2c_read1(unsigned char last)
	{
		unsigned char b = 0;

		SDA_PIN(HIGH);
		//P1DIR &= ~(SDA); // INPUT
		I2CWC &= ~0x1;
	
		i2c_wait1();
		i2c_wait1();

		// read byte
		for (unsigned char i = 0; i < 8; i++)
		{
			b <<= 1;
			i2c_wait1();

			SCL_PIN(HIGH);
			i2c_wait1();
			i2c_wait1();

			//if (SDA_PIN)
			if (I2CIO & 0x1)
			{
				b |= 1;
			}

			SCL_PIN(LOW);
		}

		// send Ack or Nak
		// P1DIR |= (SDA); // OUTPUT
		I2CWC |= 0x1;
		i2c_wait1();
		i2c_wait1();

		if (last)
		{
			SDA_PIN(HIGH);
		}
		else
		{
			SDA_PIN(LOW);
		}

		i2c_wait1();

		SCL_PIN(HIGH);
		i2c_wait1();

		SCL_PIN(LOW);
		i2c_wait1();

		SDA_PIN(LOW);
		i2c_wait1();

		return b;
	};
	
	static unsigned char i2c_restart1(unsigned char addressRW)
	{
		SDA_PIN(HIGH);
		SCL_PIN(HIGH);

		i2c_wait1();
		i2c_wait1();

		return i2c_start1(addressRW);
	};

	static void i2c_stop1()
	{
		SDA_PIN(LOW);
		i2c_wait1();

		SCL_PIN(HIGH);
		i2c_wait1();

		SDA_PIN(HIGH);
		i2c_wait1();
	};

	static int SetRegisterValue(uint8 registerAddress, uint16 value)
	{
		uint8 hi = (uint8)(value >> 8);
		uint8 lo = (uint8)value;
  
		uint8 r1 = i2c_start1(Ina220Address << 1);
		uint8 r2 = i2c_write1(registerAddress);
		uint8 r3 = i2c_write1(hi);
		uint8 r4 = i2c_write1(lo);
	
		i2c_stop1();
	
		return 0;
	};

	static uint16 ReadRegister(uint8 registerAddress)
	{
		uint8 result[] = { 0xff, 0xff };

		uint8 r4 = i2c_start1(Ina220Address << 1);
		uint8 r6 = i2c_write1(registerAddress);
		uint8 r7 = i2c_restart1((Ina220Address << 1) | 0x1);

		result[0] = i2c_read1(0);
		result[1] = i2c_read1(1);

		i2c_stop1();	

		uint16 t = ((uint16)result[0]);

		t = (uint16)(t << 8);
		t |= (uint16)result[1];

		return t;
	};

	void HalIna220Init(uint16 calibrationRegister)
	{
		P0_1 = 0x01;
  
		// SDA & SCL as output pins
		I2CCFG = 0x0;
		I2CWC = 0x83;
	
		// SDA = SCL = 0;
		I2CIO = 0x0;

		i2c_init1();
  
		if (0 == SetRegisterValue(0x0, 0x06ef))
		{
			if (0 == SetRegisterValue(CalibrationRegisterAddress, 0xa000))
			{
				//CalibrationRegisterAddress = 2;
			}
			//calibrationRegister);
		}
	};

	void HalIna220UpdateVoltage(uint16* voltage)
	{
		uint16 busVoltage = ReadRegister(BusVoltageRegisterAddress);

		bool hasValue = (busVoltage & 0x2) > 0;
		bool hasOverflow = (busVoltage & 0x1) > 0;

		if (true == hasOverflow)
		{
	  	*voltage = 3;
		}
		else if (true == hasValue)
		{
			busVoltage = (uint16)(busVoltage >> 3);
		
			*voltage = (int)((((float)busVoltage) * 4.0f / 1000.0f) * 10.0f);
			*voltage = *voltage + 1;
		}
		else
		{
		  *voltage = 4;
		}
	
		*voltage = busVoltage;
	};

	void HalIna220UpdateCurrent(uint16* current)
	{
		*current = ReadRegister(CurrentRegisterAddress);

		//return (float)current * 10.0f / 1000000.0f;
	};
	
#else

	void HalIna220Init(uint16 calibratrionRegister)
	{
	};

	void HalIna220DeInit()
	{
	};

	void HalIna220UpdateVoltage(uint16* voltage)
	{
	};

	void HalIna220UpdateCurrent(uint16* current)
	{
	};

	void PowerOnIna220()
	{
	};

	void PowerOffIna220()
	{
	};

#endif
