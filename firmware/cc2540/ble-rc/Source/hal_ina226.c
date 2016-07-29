#if defined(HAL_I2C)

	#include "bcomdef.h"
	#include "hal_ina226.h"
	#include "hal_i2c.h"
	#include "OnBoard.h"
	#include "math.h"

	static uint8 ina226Address = 0x40;
	static uint8 busVoltageRegisterAddress = 0x02;
	static uint8 currentRegisterAddress = 0x04;
	static uint8 calibrationRegisterAddress = 0x05;
	static i2cClock_t busRate = i2cClock_267KHZ;
	static float maxCurrent;
	static float shuntResistorValue;
	static float currentLsb;
	static bool available = true;

	static int SetRegisterValue(uint8 registerAddress, uint16 value)
	{
		if (false == available)
		{
			return 0;
		}
		
		uint8 hi = (uint8)(value >> 8);
		uint8 lo = (uint8)value;
  
		HalI2CInit(ina226Address, busRate);
	
		uint8 buffer[3] = { registerAddress, hi, lo };
	
		if (HalI2CWrite(3, buffer) != 3)
		{
			available = false;

			return -1;
		}
	
		HalI2CDisable();

		return 0;
	};

	static uint16 ReadRegister(uint8 registerAddress)
	{
		if (false == available)
		{
			return 0;
		}
		
		uint8 result[] = { 0xff, 0xff };

		HalI2CInit(ina226Address, busRate);

		if (1 != HalI2CWrite(1, &registerAddress))
		{
			available = false;
			
			HalI2CDisable();
			
			return 0;
		}

		if (2 != HalI2CRead(2, result))
		{
			available = false;
			
			HalI2CDisable();

			return 0;
		}

		HalI2CDisable();

		uint16 t = ((uint16)result[0]);

		t = (uint16)(t << 8);
		t |= (uint16)result[1];

		return t;
	};

	void PowerOnIna226()
	{
		P0_1 = 0x01;
	
		SetRegisterValue(0x0, 0x06ef);

		float minimumLsb = maxCurrent / 32767;

		currentLsb = (uint16)(minimumLsb * 100000000);
		currentLsb /= 100000000;
		currentLsb /= 0.0001;
		currentLsb = ceil(currentLsb);
		currentLsb *= 0.0001;

		uint16 calibrationValue = (uint16)((0.00512) / (currentLsb * shuntResistorValue));

		SetRegisterValue(calibrationRegisterAddress, calibrationValue);
	};

	void PowerOffIna226()
	{
		P0_1 = 0x00;
	};

	void HalIna226Init(float maxCurrent1, float shuntResistorValue1)
	{
		P0SEL &= ~0x02;
		P0DIR |= 0x02;

		maxCurrent = maxCurrent1;
		shuntResistorValue = shuntResistorValue1;
	
		PowerOffIna226();
	};

	void HalIna226DeInit()
	{
		PowerOffIna226();
	};

	void HalIna226UpdateVoltage(uint16* voltage)
	{
		*voltage = (uint16)((float)ReadRegister(busVoltageRegisterAddress) * 1.25f);
	};

	void HalIna226UpdateCurrent(uint16* current)
	{
		*current = (uint16)((float)ReadRegister(currentRegisterAddress) * currentLsb * 1000.0f);
	};

#else

	void HalIna226Init(float maxCurrent, float shuntResistorValue)
	{
	};

	extern void HalIna226DeInit()
	{
	};

	void HalIna226UpdateVoltage(uint16* voltage)
	{
	};

	void HalIna226UpdateCurrent(uint16* current)
	{
	};

	void PowerOnIna226()
	{
	};

	void PowerOffIna226()
	{
	};

#endif
