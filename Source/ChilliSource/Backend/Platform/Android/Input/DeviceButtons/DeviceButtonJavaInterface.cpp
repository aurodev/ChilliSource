//
//  DeviceButtonJavaInterface.cpp
//  Chilli Source
//  Created by Ian Copland on 16/06/2014.
//
//  The MIT License (MIT)
//
//  Copyright (c) 2014 Tag Games Limited
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.
//

#include <ChilliSource/Backend/Platform/Android/Input/DeviceButtons/DeviceButtonJavaInterface.h>

#include <ChilliSource/Backend/Platform/Android/Core/JNI/JavaInterfaceManager.h>
#include <ChilliSource/Backend/Platform/Android/Core/JNI/JavaInterfaceUtils.h>
#include <ChilliSource/Core/Base/Application.h>
#include <ChilliSource/Core/Threading/TaskScheduler.h>

#include <jni.h>

extern "C"
{
	//------------------------------------------------------
	/// Called from the java side of the engine when a
	/// device button event is received. This is potentially
	/// called on the UI thread.
	///
	/// @author I Copland
	///
	/// @param The jni environment.
	/// @param The java object calling the function.
	/// @param An integer representing the button pressed.
	//-----------------------------------------------------
	void Java_com_chillisource_input_DeviceButtonNativeInterface_OnDeviceButtonPressed(JNIEnv* in_env, jobject in_this, s32 in_buttonPressed);
}
//-----------------------------------------------
//-----------------------------------------------
void Java_com_chillisource_input_DeviceButtonNativeInterface_OnDeviceButtonPressed(JNIEnv* in_env, jobject in_this, s32 in_buttonPressed)
{
	ChilliSource::Android::DeviceButtonJavaInterfaceSPtr javaInterface = ChilliSource::Android::JavaInterfaceManager::GetSingletonPtr()->GetJavaInterface<ChilliSource::Android::DeviceButtonJavaInterface>();
	if (javaInterface != nullptr)
	{
		auto task = std::bind(&ChilliSource::Android::DeviceButtonJavaInterface::OnTextDeleted, pKeyboardJI.get());
		CSCore::Application::Get()->GetTaskScheduler()->ScheduleMainThreadTask(task);
	}
}

namespace ChilliSource
{
	namespace Android
	{
		CS_DEFINE_NAMEDTYPE(DeviceButtonJavaInterface);
		//-----------------------------------------------
		//-----------------------------------------------
		DeviceButtonJavaInterface::DeviceButtonJavaInterface()
		{
			CreateNativeInterface("com/chillisource/input/KeyboardNativeInterface");
		}
		//-----------------------------------------------
		//-----------------------------------------------
		bool DeviceButtonJavaInterface::IsA(Core::InterfaceIDType in_interfaceId) const
		{
			return (DeviceButtonJavaInterface::InterfaceID == in_interfaceId);
		}
		//-----------------------------------------------
		//-----------------------------------------------
		void DeviceButtonJavaInterface::SetPressedDelegate(const DeviceButtonPressedDelegate& in_delegate);
		{
			m_pressedDelegate = in_delegate;
		}
		//-----------------------------------------------
		//-----------------------------------------------
		void DeviceButtonJavaInterface::OnPressed(s32 in_buttonPressed)
		{
			if (m_pressedDelegate != nullptr)
			{
				m_pressedDelegate(in_buttonPressed);
			}
		}
	}
}
