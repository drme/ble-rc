#include "car_control_lame.h"
#include "OSAL.h"
#include "bcomdef.h"
#include "OnBoard.h"

#define GREEN_LED			P0_0
#define RED_LED				P0_1

#define	IN1					P2_0
#define	IN2					P0_3

#define IN3					P1_4
#define IN4					P1_2

#define V4

#if defined(V2)
	#define STEERING_LEFT_PIN	P1_0
	#define STEERING_RIGHT_PIN	P1_1

	#define H1	IN3
	#define H2	IN4
#elif defined(V1)
	#define STEERING_LEFT_PIN	P1_0
	#define STEERING_RIGHT_PIN	P1_1

	#define H1	IN1
	#define H2	IN2
#elif defined(V3)
	#define H1	IN3
	#define H2	IN4

	#define STEERING_LEFT_PIN	IN1
	#define STEERING_RIGHT_PIN	IN2
#elif defined(V4)
	#define H1	IN3
	#define H2	IN4

	#define STEERING_LEFT_PIN	IN1 = P1_0 = GREEN_LED
	#define STEERING_RIGHT_PIN	IN2 = P1_1 = RED_LED
#else
	#error "Undefined configuration"
#endif

void CarLameReset()
{
	// turn off stinking leds
	GREEN_LED = 0x1;
	RED_LED = 0x1;

	STEERING_RIGHT_PIN = 0x0;
	STEERING_LEFT_PIN = 0x0;

	H1 = 0x0;
	H2 = 0x0;
};

void CarLameInit()
{
	P1SEL = 0x00;
	P0SEL = 0x00;
	P2SEL &= ~0x07;

#if defined(V2)
	P0DIR = 0x03;
	P1DIR = 0x17;
	P2DIR = 0x00;
#elif defined(V1)
	P0DIR = 0x0b;
	P1DIR = 0x03;
	P2DIR = 0x01;
#elif defined(V3)
	P0DIR = 0x0b;
	P1DIR = 0x14;
	P2DIR = 0x01;
#elif defined(V4)
	P0DIR = 0x0b;
	P1DIR = 0x14;
	P2DIR = 0x01;
#else
	#error "Undefined type"
#endif
	
	CarLameReset();
};

void CarLameSetSteering(uint16 value)
{
	if (value < 350)
	{
		STEERING_LEFT_PIN	= 0;
		STEERING_RIGHT_PIN	= 1;
	}
	else if (value > 400)
	{
		STEERING_LEFT_PIN	= 1;
		STEERING_RIGHT_PIN	= 0;
	}
	else
	{
		STEERING_LEFT_PIN	= 0;
		STEERING_RIGHT_PIN	= 0;
	}
};

void CarLameSetThrottle(uint16 value)
{
	if (value < 350)
	{
		H1 = 1;
		H2 = 0;
	}
	else if (value > 400)
	{
		H1 = 0;
		H2 = 1;
	}
	else
	{
		H1 = 0;
		H2 = 0;
	}
};

void CarLameConnected()
{
	CarLameReset();
};

void CarLameDisConnected()
{
	CarLameReset();
};

void CarLameUpdateStatus()
{
};
