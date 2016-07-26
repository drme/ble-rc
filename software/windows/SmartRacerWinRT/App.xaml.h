#pragma once

#include "App.g.h"
#include "DirectXPage.xaml.h"

namespace App1
{
	ref class App sealed
	{
		public:
			App();
			virtual void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e) override;

		private:
			void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e);
			void OnResuming(Platform::Object ^sender, Platform::Object ^args);
			DirectXPage^ directXPage;
	};
};
