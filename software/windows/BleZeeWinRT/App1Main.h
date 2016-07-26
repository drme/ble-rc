#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"

namespace App1
{
	class App1Main
	{
		public:
			App1Main(const std::shared_ptr<DX::DeviceResources>& deviceResources);
			~App1Main();
	
			void StartRenderLoop();
			void StopRenderLoop();

			void Connect(SmartRacerShared::ICar^ car);

			Concurrency::critical_section& GetCriticalSection()
			{
				return this->criticalSection;
			};

		private:
			void ProcessInput();
			void Update();
			bool Render();

		private:
			std::shared_ptr<DX::DeviceResources> deviceResources;
			Windows::Foundation::IAsyncAction^ renderLoopWorker;
			Concurrency::critical_section criticalSection;
			DX::StepTimer timer;
			bool isControllerConnected;
			XINPUT_CAPABILITIES xInputCaps;
			XINPUT_STATE inputState;
			uint64 lastEnumTime;
			Windows::Devices::Sensors::Accelerometer^ accelerometer;

		public:
//			SmartRacerShared::ICar^ rc;
	};
};
