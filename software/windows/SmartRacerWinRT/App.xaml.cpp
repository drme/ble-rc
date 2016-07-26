/*#include <wrl.h>
#include <wrl/client.h>
#include <d3d11_2.h>
#include <d2d1_2.h>
#include <d2d1effects_1.h>
#include <dwrite_2.h>
#include <wincodec.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <memory>
#include <agile.h>
#include <concrt.h>
#include <collection.h>

#include <wrl.h>
#include <d3d11_2.h>
#include <d2d1_2.h>
#include <d2d1effects_1.h>
#include <dwrite_2.h>
#include <wincodec.h>
#include <XInput.h>
#include <stdio.h>
#include <agile.h>

#include "App.xaml.h"

  */
#include "pch.h"
#include "DirectXPage.xaml.h"
//#include "StatusPage.xaml.h"

using namespace App1;
using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

App::App()
{
	InitializeComponent();
	Suspending += ref new SuspendingEventHandler(this, &App::OnSuspending);
	Resuming += ref new EventHandler<Object^>(this, &App::OnResuming);
};

void App::OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e)
{
	#if _DEBUG
		if (IsDebuggerPresent())
		{
			DebugSettings->EnableFrameRateCounter = true;
		}
	#endif

	this->directXPage = DirectXPage::Instance;

	if (e->PreviousExecutionState == ApplicationExecutionState::Terminated)
	{
		this->directXPage->LoadInternalState(ApplicationData::Current->LocalSettings->Values);
	}

	Window::Current->Content = this->directXPage;
	Window::Current->Activate();
};

void App::OnSuspending(Object^ sender, SuspendingEventArgs^ e)
{
	this->directXPage->SaveInternalState(ApplicationData::Current->LocalSettings->Values);
};

void App::OnResuming(Object ^sender, Object ^args)
{
	this->directXPage->LoadInternalState(ApplicationData::Current->LocalSettings->Values);
};
