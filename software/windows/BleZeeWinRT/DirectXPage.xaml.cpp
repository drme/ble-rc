#include "pch.h"
#include "DirectXPage.xaml.h"
//#include "StatusPage.xaml.h"
//#include "SettingsPage.xaml.h"

using namespace App1;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::System::Threading;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace concurrency;

//DirectXPage^ DirectXPage::instance = nullptr;

DirectXPage::DirectXPage():
	windowVisible(true),
	coreInput(nullptr),
	upPressed(false),
	dnPressed(false),
	leftPressed(false),
	rightPressed(false)
{
	InitializeComponent();

	SmartRacerShared::AppSettings::Instance = ref new SmartRacerShared::AppSettings();

	CoreWindow^ window = Window::Current->CoreWindow;

	window->VisibilityChanged += ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &DirectXPage::OnVisibilityChanged);

	DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

	currentDisplayInformation->DpiChanged += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &DirectXPage::OnDpiChanged);
	currentDisplayInformation->OrientationChanged += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &DirectXPage::OnOrientationChanged);

	DisplayInformation::DisplayContentsInvalidated += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &DirectXPage::OnDisplayContentsInvalidated);

	swapChainPanel->CompositionScaleChanged += ref new TypedEventHandler<SwapChainPanel^, Object^>(this, &DirectXPage::OnCompositionScaleChanged);
	swapChainPanel->SizeChanged += ref new SizeChangedEventHandler(this, &DirectXPage::OnSwapChainPanelSizeChanged);

	auto pointerVisualizationSettings = PointerVisualizationSettings::GetForCurrentView();

	pointerVisualizationSettings->IsContactFeedbackEnabled = false; 
	pointerVisualizationSettings->IsBarrelButtonFeedbackEnabled = false;

	this->deviceResources = std::make_shared<DX::DeviceResources>();
	this->deviceResources->SetSwapChainPanel(swapChainPanel);

	auto workItemHandler = ref new WorkItemHandler([this] (IAsyncAction ^)
	{
		this->coreInput = swapChainPanel->CreateCoreIndependentInputSource(Windows::UI::Core::CoreInputDeviceTypes::Mouse | Windows::UI::Core::CoreInputDeviceTypes::Touch | Windows::UI::Core::CoreInputDeviceTypes::Pen);
		this->coreInput->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessUntilQuit);
	});

	this->inputLoopWorker = ThreadPool::RunAsync(workItemHandler, WorkItemPriority::High, WorkItemOptions::TimeSliced);

	
	this->main = std::unique_ptr<App1Main>(new App1Main(this->deviceResources));
	this->main->StartRenderLoop();

	auto accelerometer = Windows::Devices::Sensors::Accelerometer::GetDefault();

	if (nullptr != accelerometer)
	{
		
		if (accelerometer->MinimumReportInterval > 20)
		{
			accelerometer->ReportInterval = accelerometer->MinimumReportInterval;
		}
		else
		{
			accelerometer->ReportInterval = 20;
		}

		//accelerometer->ReadingChanged += ref new Windows::Foundation::TypedEventHandler<Windows::Devices::Sensors::Accelerometer ^, Windows::Devices::Sensors::AccelerometerReadingChangedEventArgs ^>(this, &App1::DirectXPage::OnReadingChanged);
	}

	this->steeringStick->Moved += ref new SmartRacerShared::PositionChangedHandler(this, &App1::DirectXPage::OnMoved);
	this->throttleStick->Moved += ref new SmartRacerShared::PositionChangedHandler(this, &App1::DirectXPage::OnMoved);

	window->KeyDown += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^, Windows::UI::Core::KeyEventArgs ^>(this, &App1::DirectXPage::OnKeyDown);
	window->KeyUp += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^, Windows::UI::Core::KeyEventArgs ^>(this, &App1::DirectXPage::OnKeyUp);
};

DirectXPage::~DirectXPage()
{
	this->main->StopRenderLoop();
	this->coreInput->Dispatcher->StopProcessEvents();
};

void DirectXPage::SaveInternalState(IPropertySet^ state)
{
	critical_section::scoped_lock lock(this->main->GetCriticalSection());

	this->deviceResources->Trim();
	this->main->StopRenderLoop();
};

void DirectXPage::LoadInternalState(IPropertySet^ state)
{
	this->main->StartRenderLoop();
};

void DirectXPage::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
	this->windowVisible = args->Visible;
	
	if (this->windowVisible)
	{
		this->main->StartRenderLoop();
	}
	else
	{
		this->main->StopRenderLoop();
	}
};

void DirectXPage::OnDpiChanged(DisplayInformation^ sender, Object^ args)
{
	critical_section::scoped_lock lock(this->main->GetCriticalSection());
	
	this->deviceResources->SetDpi(sender->LogicalDpi);
};

void DirectXPage::OnOrientationChanged(DisplayInformation^ sender, Object^ args)
{
	critical_section::scoped_lock lock(this->main->GetCriticalSection());

	this->deviceResources->SetCurrentOrientation(sender->CurrentOrientation);
};

void DirectXPage::OnDisplayContentsInvalidated(DisplayInformation^ sender, Object^ args)
{
	critical_section::scoped_lock lock(this->main->GetCriticalSection());
	
	this->deviceResources->ValidateDevice();
};

void DirectXPage::OnConnectButtonClick(Object^ sender, RoutedEventArgs^ e)
{
	//Window::Current->Content = ref new SelectCarPage();
	Window::Current->Content = ref new SmartRacerShared::SelectCarPage();
};

void DirectXPage::ConnectCar(SmartRacerShared::ICar^ car)
{
	if (nullptr != car)
	{
		this->connectedEventToken = car->Connected += ref new Windows::Foundation::EventHandler<SmartRacerShared::ICar ^>(this, &App1::DirectXPage::OnConnected);
		this->disconnectedEventToken = car->Disconnected += ref new Windows::Foundation::EventHandler<SmartRacerShared::ICar ^>(this, &App1::DirectXPage::OnDisconnected);
		this->main->Connect(car);
	}
};

void DirectXPage::OnCompositionScaleChanged(SwapChainPanel^ sender, Object^ args)
{
	critical_section::scoped_lock lock(this->main->GetCriticalSection());

	this->deviceResources->SetCompositionScale(sender->CompositionScaleX, sender->CompositionScaleY);
};

void DirectXPage::OnSwapChainPanelSizeChanged(Object^ sender, SizeChangedEventArgs^ e)
{
	critical_section::scoped_lock lock(this->main->GetCriticalSection());

	this->deviceResources->SetLogicalSize(e->NewSize);
};

DirectXPage^ DirectXPage::Instance::get()
{
	if (nullptr == SmartRacerShared::AppSettings::MainPage)
	{
		SmartRacerShared::AppSettings::MainPage = ref new DirectXPage();
	}

	return static_cast<DirectXPage^>(SmartRacerShared::AppSettings::MainPage);
};

void App1::DirectXPage::ShowStatusClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
//	Window::Current->Content = ref new StatusPage();
	Window::Current->Content = ref new SmartRacerShared::StatusPage();
};

void App1::DirectXPage::SettingsClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	//	Window::Current->Content = ref new SettingsPage();
	Window::Current->Content = ref new SmartRacerShared::SettingsPage();
};

SmartRacerShared::ICar^ App1::DirectXPage::ConnectedCar::get()
{
//	return this->main->rc;
	return SmartRacerShared::CarConnector::ActiveCar;
};

SmartRacerShared::AppSettings^ App1::DirectXPage::AppSettings::get()
{
	return SmartRacerShared::AppSettings::Instance;
};

void App1::DirectXPage::OnConnected(Platform::Object^ sender, SmartRacerShared::ICar^ car)
{
	UpdateInfo();
};

void App1::DirectXPage::OnDisconnected(Platform::Object^ sender, SmartRacerShared::ICar^ car)
{
	if (nullptr != car)
	{
		car->Disconnected -= this->disconnectedEventToken;
		car->Connected -= this->connectedEventToken;
	}

	this->main->Connect(nullptr);

	UpdateInfo();
};

void App1::DirectXPage::OnDisconnectButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (nullptr != this->ConnectedCar)
	{
		this->ConnectedCar->Disconnect();
	}
};

void App1::DirectXPage::OnReadingChanged(Windows::Devices::Sensors::Accelerometer ^sender, Windows::Devices::Sensors::AccelerometerReadingChangedEventArgs ^args)
{
	if (this->AppSettings->ControllType != SmartRacerShared::SteeringType::Accelerometer)
	{
		return;
	}

	if (false == this->windowVisible)
	{
		return;
	}

	if (nullptr == this->ConnectedCar)
	{
		return;
	}

	float v = args->Reading->AccelerationY * 2.0f;

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

	if (true == this->AppSettings->InvertSteering)
	{
		v *= -1.0f;
	}

	this->ConnectedCar->Steering = v;
};

void App1::DirectXPage::OnMoved(SmartRacerShared::ThumbStick^ sender, float position)
{
	if (false == this->windowVisible)
	{
		return;
	}

	if (nullptr == this->ConnectedCar)
	{
		return;
	}

	if (sender == this->steeringStick)
	{
		if (this->AppSettings->ControllType != SmartRacerShared::SteeringType::TouchScreen)
		{
			return;
		}

		if (true == this->AppSettings->InvertSteering)
		{
			this->ConnectedCar->Steering = -position;
		}
		else
		{
			this->ConnectedCar->Steering = position;
		}
	}
	else if (sender == this->throttleStick)
	{
		if ((this->AppSettings->ControllType != SmartRacerShared::SteeringType::TouchScreen) && (this->AppSettings->ControllType != SmartRacerShared::SteeringType::Accelerometer))
		{
			return;
		}

		if (true == this->AppSettings->InvertThrottle)
		{
			this->ConnectedCar->Throttle = -position;
		}
		else
		{
			this->ConnectedCar->Throttle = position;
		}
	}
};

void App1::DirectXPage::OnKeyDown(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::KeyEventArgs ^args)
{
	this->upPressed |= (args->VirtualKey == Windows::System::VirtualKey::Up);
	this->upPressed |= (args->VirtualKey == Windows::System::VirtualKey::W);

	this->dnPressed |= (args->VirtualKey == Windows::System::VirtualKey::Down);
	this->dnPressed |= (args->VirtualKey == Windows::System::VirtualKey::S);

	this->leftPressed |= (args->VirtualKey == Windows::System::VirtualKey::Left);
	this->leftPressed |= (args->VirtualKey == Windows::System::VirtualKey::A);

	this->rightPressed |= (args->VirtualKey == Windows::System::VirtualKey::Right);
	this->rightPressed |= (args->VirtualKey == Windows::System::VirtualKey::D);

	UpdateKeyboard();
};

void App1::DirectXPage::OnKeyUp(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::KeyEventArgs ^args)
{
	this->upPressed &= (args->VirtualKey != Windows::System::VirtualKey::Up) && (args->VirtualKey != Windows::System::VirtualKey::W);
	this->dnPressed &= (args->VirtualKey != Windows::System::VirtualKey::Down) && (args->VirtualKey != Windows::System::VirtualKey::S);
	this->leftPressed &= (args->VirtualKey != Windows::System::VirtualKey::Left) && (args->VirtualKey != Windows::System::VirtualKey::A);
	this->rightPressed &= (args->VirtualKey != Windows::System::VirtualKey::Right) && (args->VirtualKey != Windows::System::VirtualKey::D);

	UpdateKeyboard();
};

void App1::DirectXPage::UpdateKeyboard()
{
	if (this->AppSettings->ControllType != SmartRacerShared::SteeringType::Keyboard)
	{
		return;
	}

	if (nullptr == this->ConnectedCar)
	{
		return;
	}

	float invertThrottle = (this->AppSettings->InvertThrottle) ? -1.0f : 1.0f;
	float invertSteering = (this->AppSettings->InvertSteering) ? -1.0f : 1.0f;

	float steering = 0.0f;
	float throttle = 0.0f;

	if (true == this->upPressed)
	{
		throttle += 1.0f;
	}

	if (true == this->dnPressed)
	{
		throttle -= 1.0f;
	}

	if (true == this->leftPressed)
	{
		steering -= 1.0f;
	}

	if (true == this->rightPressed)
	{
		steering += 1.0f;
	}

	this->ConnectedCar->Steering = invertSteering * steering;
	this->ConnectedCar->Throttle = invertThrottle * throttle;
};

void App1::DirectXPage::UpdateInfo()
{
	this->Dispatcher->RunAsync(CoreDispatcherPriority::High, ref new DispatchedHandler([this]()
	{
		bool connected = this->ConnectedCar != nullptr;

		this->connectButton->IsEnabled = !connected;
		this->disconnectButton->IsEnabled = connected;
		this->settingsButton->IsEnabled = connected;
		this->statusButton->IsEnabled = connected;

		if (false == connected)
		{
			this->keyboardInfo->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
			this->controllerInfo->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
			this->steeringStick->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
			this->throttleStick->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		}
		else
		{
			switch (this->AppSettings->ControllType)
			{
				case SmartRacerShared::SteeringType::Accelerometer:
					this->keyboardInfo->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
					this->controllerInfo->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
					this->steeringStick->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
					this->throttleStick->Visibility = Windows::UI::Xaml::Visibility::Visible;
					break;
				case SmartRacerShared::SteeringType::TouchScreen:
					this->keyboardInfo->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
					this->controllerInfo->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
					this->steeringStick->Visibility = Windows::UI::Xaml::Visibility::Visible;
					this->throttleStick->Visibility = Windows::UI::Xaml::Visibility::Visible;
					break;
				case SmartRacerShared::SteeringType::Keyboard:
					this->keyboardInfo->Visibility = Windows::UI::Xaml::Visibility::Visible;
					this->controllerInfo->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
					this->steeringStick->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
					this->throttleStick->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
					break;			
				case SmartRacerShared::SteeringType::GamePad:
				default:
					this->keyboardInfo->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
					this->controllerInfo->Visibility = Windows::UI::Xaml::Visibility::Visible;
					this->steeringStick->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
					this->throttleStick->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
					break;
			}
		}

		this->UpdateLayout();
	}));
};


void App1::DirectXPage::Page_GotFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	UpdateInfo();
}
