#include "pch.h"
#include "App1Main.h"
#include "Common\DirectXHelper.h"

using namespace App1;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;
using namespace Windows::UI::Xaml;

const uint64 XINPUT_ENUM_TIMEOUT_MS = 2000;

App1Main::App1Main(const std::shared_ptr<DX::DeviceResources>& deviceResources1) : deviceResources(deviceResources1)
{
	this->timer.SetFixedTimeStep(true);
	this->timer.SetTargetElapsedSeconds(1.0 / 60);
	this->accelerometer = Windows::Devices::Sensors::Accelerometer::GetDefault();
};

void App1Main::Connect(SmartRacerShared::ICar^ car)
{
	SmartRacerShared::CarConnector::ActiveCar = car;

	if (nullptr != car)
	{
		SmartRacerShared::CarConnector::ActiveCar->Connect();
	}
};

App1Main::~App1Main()
{
};

void App1Main::StartRenderLoop()
{
	if ((this->renderLoopWorker != nullptr) && (this->renderLoopWorker->Status == AsyncStatus::Started))
	{
		return;
	}

	auto workItemHandler = ref new WorkItemHandler([this](IAsyncAction ^ action)
	{
		while (action->Status == AsyncStatus::Started)
		{
			critical_section::scoped_lock lock(this->criticalSection);
			
			Update();
			
			if (Render())
			{
				this->deviceResources->Present();
			}
		}
	});

	this->renderLoopWorker = ThreadPool::RunAsync(workItemHandler, WorkItemPriority::High, WorkItemOptions::TimeSliced);
};

void App1Main::StopRenderLoop()
{
	this->renderLoopWorker->Cancel();
};

void App1Main::Update() 
{
	ProcessInput();

	this->timer.Tick([&]() { });
};

void App1Main::ProcessInput()
{
	if (DirectXPage::Instance->AppSettings->ControllType == SmartRacerShared::SteeringType::Accelerometer)
	{
		if ((nullptr != SmartRacerShared::CarConnector::ActiveCar) && (nullptr != this->accelerometer))
		{
			float v = this->accelerometer->GetCurrentReading()->AccelerationX * 2.0f;

			if (v < -1.0f)
			{
				v = -1.0f;
			}

			if (v > 1.0f)
			{
				v = 1.0f;
			}

			if ((v > 0.0f) && (v < 0.1f))
			{
				v = 0.0f;
			}

			if ((v < 0.0f) && (v > -0.1f))
			{
				v = 0.0f;
			}

			if (true == DirectXPage::Instance->AppSettings->InvertSteering)
			{
				v *= -1.0f;
			}

			SmartRacerShared::CarConnector::ActiveCar->Steering = v;
		}
	}

	if (false == this->isControllerConnected)
	{
		uint64 currentTime = ::GetTickCount64();
		
		if (currentTime - this->lastEnumTime < XINPUT_ENUM_TIMEOUT_MS)
		{
			return;
		}

		this->lastEnumTime = currentTime;

		uint32 capsResult = XInputGetCapabilities(0, XINPUT_FLAG_GAMEPAD, &this->xInputCaps);

		if (capsResult != ERROR_SUCCESS)
		{
			return;
		}

		this->isControllerConnected = true;
	}

	uint32 stateResult = XInputGetState(0, &this->inputState);

	if (stateResult != ERROR_SUCCESS)
	{
		this->isControllerConnected = false;
		this->lastEnumTime = ::GetTickCount64();
	}
	else
	{
		if (nullptr != SmartRacerShared::CarConnector::ActiveCar)
		{
			float invertThrottle = (DirectXPage::Instance->AppSettings->InvertThrottle) ? -1.0f : 1.0f;
			float invertSteering = (DirectXPage::Instance->AppSettings->InvertSteering) ? -1.0f : 1.0f;

			if (DirectXPage::Instance->AppSettings->ControllType == SmartRacerShared::SteeringType::GamePad)
			{
				if (this->inputState.Gamepad.bLeftTrigger > 0)
				{
					SmartRacerShared::CarConnector::ActiveCar->Throttle = -1 * ((float)this->inputState.Gamepad.bLeftTrigger / 255.0f) * invertThrottle;
				}
				else if (this->inputState.Gamepad.bRightTrigger > 0)
				{
					SmartRacerShared::CarConnector::ActiveCar->Throttle = (float)(this->inputState.Gamepad.bRightTrigger) / 255.0f * invertThrottle;
				}
				else
				{
					SmartRacerShared::CarConnector::ActiveCar->Throttle = 0;
				}

				SmartRacerShared::CarConnector::ActiveCar->Steering = (-this->inputState.Gamepad.sThumbLX * invertSteering) / 32767.0f;
			}
		}
	}
};

bool App1Main::Render() 
{
	if (this->timer.GetFrameCount() == 0)
	{
		return false;
	}

	auto context = this->deviceResources->GetD3DDeviceContext();
	auto viewport = this->deviceResources->GetScreenViewport();
	
	context->RSSetViewports(1, &viewport);

	ID3D11RenderTargetView* const targets[1] = { this->deviceResources->GetBackBufferRenderTargetView() };
	context->OMSetRenderTargets(1, targets, this->deviceResources->GetDepthStencilView());

	context->ClearRenderTargetView(this->deviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::Black);
	context->ClearDepthStencilView(this->deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	return true;
};
