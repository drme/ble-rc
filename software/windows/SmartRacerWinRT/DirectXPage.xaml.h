#pragma once

#include "DirectXPage.g.h"
#include "Common\DeviceResources.h"
#include "App1Main.h"

namespace App1
{
	public ref class DirectXPage sealed : SmartRacerShared::IMainPage
	{
		public:
			DirectXPage();
			virtual ~DirectXPage();

			void SaveInternalState(Windows::Foundation::Collections::IPropertySet^ state);
			void LoadInternalState(Windows::Foundation::Collections::IPropertySet^ state);
			virtual void ConnectCar(SmartRacerShared::ICar^ car);
			virtual void UpdateInfo();
	
		internal:
			property static DirectXPage^ Instance { DirectXPage^ get(); };
			property SmartRacerShared::ICar^ ConnectedCar { SmartRacerShared::ICar^ get(); };
			property SmartRacerShared::AppSettings^ AppSettings { SmartRacerShared::AppSettings^ get(); };

		private:
			void OnVisibilityChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ args);
			void OnDpiChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);
			void OnOrientationChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);
			void OnDisplayContentsInvalidated(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);
			void OnCompositionScaleChanged(Windows::UI::Xaml::Controls::SwapChainPanel^ sender, Object^ args);
			void OnSwapChainPanelSizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);
			void ShowStatusClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
			void SettingsClicked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
			void UpdateKeyboard();
			void OnConnected(Platform::Object ^sender, SmartRacerShared::ICar ^args);
			void OnDisconnected(Platform::Object ^sender, SmartRacerShared::ICar ^args);
			void OnConnectButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
			void OnDisconnectButtonClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
			void OnReadingChanged(Windows::Devices::Sensors::Accelerometer ^sender, Windows::Devices::Sensors::AccelerometerReadingChangedEventArgs ^args);
			void OnMoved(SmartRacerShared::ThumbStick ^sender, float position);
			void OnKeyDown(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::KeyEventArgs ^args);
			void OnKeyUp(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::KeyEventArgs ^args);

		private:
//			SmartRacerShared::AppSettings^ settings;
			Windows::Foundation::EventRegistrationToken connectedEventToken;
			Windows::Foundation::EventRegistrationToken disconnectedEventToken;
			bool upPressed;
			bool dnPressed;
			bool leftPressed;
			bool rightPressed;
			std::shared_ptr<DX::DeviceResources> deviceResources;
			std::unique_ptr<App1Main> main;
			bool windowVisible;
			Windows::Foundation::IAsyncAction^ inputLoopWorker;
			Windows::UI::Core::CoreIndependentInputSource^ coreInput;
//			static DirectXPage^	instance;
			void Page_GotFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
};
