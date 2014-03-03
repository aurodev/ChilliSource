/*
* File: WindowsPlatformSystem.cpp
* Date: 24/11/2010 2010 
* Description: 
*/

/*
* Author: Scott Downie
* Version: v 1.0
* Copyright ©2010 Tag Games Limited - All rights reserved 
*/

#define WIN32_LEAN_AND_MEAN

#include <ChilliSource/Backend/Platform/Windows/PlatformSystemWindows.h>
#include <ChilliSource/Backend/Platform/Windows/ImageLoaderWindows.h>
#include <ChilliSource/Backend/Platform/Windows/Input/InputSystemWindows.h>
#include <ChilliSource/Backend/Platform/Windows/FileIO/FileSystemWindows.h>
#include <ChilliSource/Backend/Platform/Windows/HttpConnectionSystemWindows.h>
#include <ChilliSource/Backend/Audio/FMOD/Base/FMODSystem.h>
#include <ChilliSource/Backend/Audio/FMOD/Base/FMODAudioLoader.h>

#include <ChilliSource/Backend/Rendering/OpenGL/Base/RenderSystem.h>
#include <ChilliSource/Backend/Rendering/OpenGL/Base/RenderCapabilities.h>

#include <ChilliSource/Rendering/Material/MaterialLoader.h>
#include <ChilliSource/Rendering/Material/MaterialFactory.h>
#include <ChilliSource/Rendering/Sprite/SpriteSheetLoader.h>
#include <ChilliSource/Rendering/Sprite/XMLSpriteSheetLoader.h>
#include <ChilliSource/Rendering/Font/FontLoader.h>
#include <ChilliSource/Rendering/Base/Renderer.h>
#include <ChilliSource/Rendering/Model/AnimatedMeshComponentUpdater.h>

#include <ChilliSource/Audio/Base/AudioLoader.h>
#include <ChilliSource/Audio/Base/AudioPlayer.h>

#include <ChilliSource/Core/Base/Application.h>
#include <ChilliSource/Core/Image/MoImageProvider.h>

#include <ChilliSource/GUI/Base/GUIViewFactory.h>

#include <windows.h>

namespace ChilliSource 
{
	//This is global as LARGE_INTEGER is defined in windows.h. Including windows.h in PlatformSystemWindows.h will cause compiler errors
	//in FileSystemWindows.h
	LARGE_INTEGER gFrequency;

	namespace Windows
	{
		//-----------------------------------------
		/// Constructor
		///
		/// Default
		//-----------------------------------------
		CPlatformSystem::CPlatformSystem() : mbIsRunning(true), mbIsSuspended(false), muddwAppStartTime(0), mffAppPreviousTime(0.0)
		{
			//CNotificationScheduler::Initialise(new CLocalNotificationScheduler(), new CRemoteNotificationScheduler());
			Core::Application::SetFileSystem(new Windows::CFileSystem());
			Core::Logging::Init();
		}
		//-----------------------------------------
		/// Init
		///
		/// Create the GLFW window
		//-----------------------------------------
		void CPlatformSystem::Init()
		{
			QueryPerformanceFrequency(&gFrequency);

			if(!glfwInit())
			{
				CS_LOG_FATAL("Cannot initialise GLFW");
			}

			//Set the window based on the retina resolution
			if(glfwOpenWindow(960, 640, 8, 8, 8, 8, 8, 0, GLFW_WINDOW) != GL_TRUE)
			{
				glfwTerminate();
				CS_LOG_FATAL("Cannot create GLFW window");
			}

			glfwSetWindowTitle("MoFlow");

			//Register callbacks
			glfwSetWindowSizeCallback((GLFWwindowsizefun)&CPlatformSystem::OnWindowResized);
			glfwSetWindowCloseCallback((GLFWwindowclosefun)&CPlatformSystem::OnWindowClosed);

            //Initialise GUI factory
            GUI::GUIViewFactory::RegisterDefaults();
		}
		//-------------------------------------------------
		/// Add Default Systems
		///
		/// Adds default systems to the applications system
		/// list.
		///
		/// @param the system list
		//-------------------------------------------------
		void CPlatformSystem::CreateDefaultSystems(std::vector<Core::SystemSPtr> & inaSystems)
		{
			//create the main systems
			OpenGL::CRenderSystem* pRenderSystem = new OpenGL::CRenderSystem();
			inaSystems.push_back(Core::SystemSPtr(pRenderSystem));
			Core::Application::SetRenderSystem(pRenderSystem);

			Input::InputSystem* pInputSystem = new Windows::CInputSystem();
			inaSystems.push_back(Core::SystemSPtr(pInputSystem));
			Core::Application::SetInputSystem(pInputSystem);

			Audio::AudioSystem* pAudioSystem = new FMOD::CFMODSystem();
			inaSystems.push_back(Core::SystemSPtr(pAudioSystem));
			inaSystems.push_back(Core::SystemSPtr(new FMOD::CFMODAudioLoader(pAudioSystem)));
			Core::Application::SetAudioSystem(pAudioSystem);

			//create other important systems
			OpenGL::CRenderCapabilities* pRenderCapabilities = new OpenGL::CRenderCapabilities();
			inaSystems.push_back(Core::SystemSPtr(pRenderCapabilities));
			inaSystems.push_back(Core::SystemSPtr(new Windows::ImageLoader()));
			inaSystems.push_back(Core::SystemSPtr(new Core::MoImageProvider()));
			inaSystems.push_back(Core::SystemSPtr(new Rendering::SpriteSheetLoader()));
			inaSystems.push_back(Core::SystemSPtr(new Rendering::XMLSpriteSheetLoader()));
			inaSystems.push_back(Core::SystemSPtr(new Rendering::MaterialLoader(pRenderCapabilities)));
			inaSystems.push_back(Core::SystemSPtr(new Rendering::FontLoader()));
			inaSystems.push_back(Core::SystemSPtr(new Rendering::AnimatedMeshComponentUpdater()));
			inaSystems.push_back(Core::SystemSPtr(new Rendering::MaterialFactory(pRenderSystem->GetTextureManager(), pRenderSystem->GetShaderManager(), pRenderSystem->GetCubemapManager(), pRenderCapabilities)));

			//Initialise the render system
			Core::Application::GetRenderSystemPtr()->Init((u32)Core::Screen::GetRawDimensions().x, (u32)Core::Screen::GetRawDimensions().y);
            
			//Create the renderer
			Core::Application::SetRenderer(new Rendering::Renderer(Core::Application::GetRenderSystemPtr()));
            
			//Initialise the input system
			if(Core::Application::GetInputSystemPtr() != NULL)
			{
				Core::Application::SetHasTouchInput((Core::Application::GetInputSystemPtr()->GetTouchScreenPtr() != NULL));
			}
		}
		//-------------------------------------------------
		/// Post Create Systems
		///
		/// Deals with anything that needs to be handled after
		/// creating the systems.
		///
		/// @param the system list
		//-------------------------------------------------
		void CPlatformSystem::PostCreateSystems()
		{
			if(Core::Application::GetAudioSystemPtr() != NULL)
			{
				Audio::AudioPlayer::Init();
			}
		}
		//-----------------------------------------
		/// Run
		///
		/// Begin the game loop
		//-----------------------------------------
		void CPlatformSystem::Run()
		{
			muddwAppStartTime = (u64)glfwGetTime();

			while(mbIsRunning)
			{
				if(!mbIsSuspended)
				{
					f64 ffAppCurrentTime = glfwGetTime();

					f32 fDt = (f32)(ffAppCurrentTime - mffAppPreviousTime);
					u64 uddwAppRunningTime = ((u64)mffAppPreviousTime - muddwAppStartTime);

					//Update event
					Core::Application::OnFrameBegin(fDt, uddwAppRunningTime);

					mffAppPreviousTime = ffAppCurrentTime;
				}
			}
		}
		//-----------------------------------------
		/// Set Updater Active
		///
		/// Starts or stops the platforms 
		/// update loop.
		///
		/// @param Whether to end or begin
		//-----------------------------------------
		void CPlatformSystem::SetUpdaterActive(bool inbIsActive)
		{
			mbIsSuspended = !inbIsActive;
		}
		//--------------------------------------------
		/// Terminate Updater
		///
		/// Stops the update loop causing the 
		/// application to terminate
		//--------------------------------------------
		void CPlatformSystem::TerminateUpdater()
		{
			mbIsRunning = false;
		}
		//--------------------------------------------
		/// Can Create System With Interface 
		///
		/// @param Interface ID
		/// @param Whether system can be created
		//--------------------------------------------
		bool CPlatformSystem::CanCreateSystemWithInterface(Core::InterfaceIDType inInterfaceID) const
		{
			return	inInterfaceID == Networking::HttpConnectionSystem::InterfaceID;
		}
		//--------------------------------------------
		/// Create and Add System With Interface
		///
		/// @param Interface ID
		/// @param Vector of exisiting systems to append
		/// @return Pointer to the given system or NULL
		//--------------------------------------------
		Core::System* CPlatformSystem::CreateAndAddSystemWithInterface(Core::InterfaceIDType inInterfaceID, std::vector<Core::SystemSPtr>& inaExisitingSystems) const
		{
			//Check if it already exists to prevent multiples
			for(std::vector<Core::SystemSPtr>::const_iterator it = inaExisitingSystems.begin(); it != inaExisitingSystems.end(); ++it)
			{
				if((*it)->IsA(inInterfaceID))
				{
					return (*it).get();
				}
			}

			Core::System* pSystem = NULL;

			if(inInterfaceID == Networking::HttpConnectionSystem::InterfaceID)
			{
				pSystem = new CHttpConnectionSystem();
				inaExisitingSystems.push_back(Core::SystemSPtr(pSystem));
			}

			return pSystem;
		}
		//--------------------------------------------
		/// Can Create Activity With Interface
		///
		/// @param Interface ID
		/// @return Whether activity can be created
		//--------------------------------------------
		bool CPlatformSystem::CanCreateActivityWithInterface(Core::InterfaceIDType inInterfaceID) const
		{
			return false;
		}
		//--------------------------------------------
		/// Create Activity With Interface
		///
		/// @param Interface ID
		/// @return Ownership of activity instance or NULL
		//--------------------------------------------
		Core::Activity* CPlatformSystem::CreateActivityWithInterface(Core::InterfaceIDType inInterfaceID) const
		{
			return NULL;
		}
		//--------------------------------------------
		/// Can Create Information Provider With Interface
		///
		/// @param Interface ID
		/// @return Whether provider can be created
		//--------------------------------------------
		bool CPlatformSystem::CanCreateInformationProviderWithInterface(Core::InterfaceIDType inInterfaceID) const
		{
			return false;
		}
		//--------------------------------------------
		/// Create Information Provider With Interface
		///
		/// @param Interface ID
		/// @return Ownership of provider instance or NULL
		//--------------------------------------------
		Core::IInformationProvider* CPlatformSystem::CreateInformationProviderWithInterface(Core::InterfaceIDType inInterfaceID) const
		{
			return NULL;
		}
		//-----------------------------------------------------------------------------------------------------------
		/// Get Screen Dimensions
		///
		/// Retrieves the screen dimensions. These dimensions are always in the default orientation for the device.
		/// @return A Vector2 containing the screen size in it's x + y components
		//-----------------------------------------------------------------------------------------------------------
		Core::Vector2 CPlatformSystem::GetScreenDimensions() const
		{
			Core::Vector2 Result;

			s32 dwWidth, dwHeight = 0;
			glfwGetWindowSize(&dwWidth, &dwHeight);

			Result.x = (f32)dwWidth;
			Result.y = (f32)dwHeight;

			return Result;
		}
		//--------------------------------------------------------------
		/// Get OS Version
		///
		/// @return  String containing the OS version of the device
		//--------------------------------------------------------------
		std::string CPlatformSystem::GetOSVersion() const
		{
			OSVERSIONINFOEX osvi;
			ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX)); osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
			GetVersionEx((OSVERSIONINFO*) &osvi);
			return std::string(Core::ToString((u32)osvi.dwMajorVersion) + "." + Core::ToString((u32)osvi.dwMinorVersion));
		}
		//--------------------------------------------------------------
		/// Get Locale
		///
		/// Get the active language locale of the device
		/// @return Locale ID
		//--------------------------------------------------------------
		Core::Locale CPlatformSystem::GetLocale() const
		{
			wchar_t localeName[LOCALE_NAME_MAX_LENGTH]={0};

			if(GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH * sizeof(wchar_t)))
			{
				if(wcscmp(localeName, L"en") == 0)	return Core::Locale("en");
				if(wcscmp(localeName, L"fr") == 0)	return Core::Locale("fr");
				if(wcscmp(localeName, L"it") == 0)  return Core::Locale("it");
				if(wcscmp(localeName, L"de") == 0)  return Core::Locale("de");
				if(wcscmp(localeName, L"es") == 0)  return Core::Locale("es");
				if(wcscmp(localeName, L"jp") == 0)  return Core::Locale("jp");
			}

			//Just default to english
			return Core::Locale("en");
		}
		//--------------------------------------------------------------
		/// Get Device Model Name
		///
		/// @return The above information stringified
		//--------------------------------------------------------------
		std::string CPlatformSystem::GetDeviceModelName() const
		{
			return "Windows";
		}
		//--------------------------------------------------------------
		/// Get Device Model Type Name
		///
		/// @return The above information stringified
		//--------------------------------------------------------------
		std::string CPlatformSystem::GetDeviceModelTypeName() const
		{
			return "PC";
		}
		//--------------------------------------------------------------
		/// Get Device Manufacturer Name
		///
		/// @return The above information stringified
		//--------------------------------------------------------------
		std::string CPlatformSystem::GetDeviceManufacturerName() const
		{
			return "Microsoft";
		}
		//--------------------------------------------------------------
		/// Get Langauge
		///
		/// Get the active language of the device in locale format
		/// @return Locale ID
		//--------------------------------------------------------------
		Core::Locale CPlatformSystem::GetLanguage() const
		{
			wchar_t localeName[LOCALE_NAME_MAX_LENGTH]={0};

			if(GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH * sizeof(wchar_t)))
			{
				if(wcscmp(localeName, L"en") == 0)	return Core::Locale("en");
				if(wcscmp(localeName, L"fr") == 0)	return Core::Locale("fr");
				if(wcscmp(localeName, L"it") == 0)  return Core::Locale("it");
				if(wcscmp(localeName, L"de") == 0)  return Core::Locale("de");
				if(wcscmp(localeName, L"es") == 0)  return Core::Locale("es");
				if(wcscmp(localeName, L"jp") == 0)  return Core::Locale("jp");
			}

			//Just default to english
			return Core::Locale("en");
		}
		//-------------------------------------------------
		/// Get Screen Density
		///
		/// @return The density scale factor of the screen
		/// to convert from DIPS to physical pixels
		//-------------------------------------------------
		f32 CPlatformSystem::GetScreenDensity() const
		{
			return 1.0f;
		}
		//-------------------------------------------------
		/// Get Device ID
		///
		/// @return The UDID of the device
		//-------------------------------------------------
		std::string CPlatformSystem::GetDeviceID()
		{
			CS_LOG_ERROR("CPlatformSystem::GetDeviceID() has not been implemented!");
			return "FAKE ID";
		}
		//--------------------------------------------------------------
		/// Get Number Of CPU Cores
		///
		/// @return The number of cores available
		//--------------------------------------------------------------
		u32 CPlatformSystem::GetNumberOfCPUCores() const
		{
			SYSTEM_INFO SysInfo;
			GetSystemInfo(&SysInfo);
			return SysInfo.dwNumberOfProcessors;
		}
		//--------------------------------------------------------------
		/// Get System Time
		///
		/// @return the current time in milliseconds
		//--------------------------------------------------------------
		u64 CPlatformSystem::GetSystemTimeMS() const
		{
			LARGE_INTEGER currentTime;
            QueryPerformanceCounter(&currentTime);
            return (u64)((currentTime.QuadPart) * 1000.0 / gFrequency.QuadPart);
		}
		//-------------------------------------------------
		/// Get Physical Screen Size
		///
		/// @return The physical size of the screen in
		/// inches.
		//-------------------------------------------------
		f32 CPlatformSystem::GetPhysicalScreenSize()
		{
			CS_LOG_WARNING("GetPhysicalScreenSize() is not implemented on windows!");
			return 0.0f;
		}
		//--------------------------------------------------------------------------------------------------
		/// Make Toast
		///
		/// Display a toast notification with the given text
		///
		/// @param Text
		//--------------------------------------------------------------------------------------------------
		void CPlatformSystem::MakeToast(const Core::UTF8String& instrText) const
		{
			CS_LOG_WARNING("Toast not available on Windows");
		}
		//--------------------------------------------------------------------------------------------------
		/// Show System Confirm Dialog
		///
		/// Display a system confirmation dialog with the given ID and delegate
		///
		/// @param ID
		/// @param Title text
		/// @param Message text
		/// @param Confirm text
		/// @param Cancel text
		//--------------------------------------------------------------------------------------------------
		void CPlatformSystem::ShowSystemConfirmDialog(u32 inudwID, const Core::UTF8String& instrTitle, const Core::UTF8String& instrMessage, const Core::UTF8String& instrConfirm, const Core::UTF8String& instrCancel) const
		{
			if(MessageBoxA(NULL, instrTitle.ToASCII().c_str(), instrMessage.ToASCII().c_str(), MB_OKCANCEL) == IDOK)
			{
				Core::Application::OnSystemConfirmDialogResult(inudwID, Core::SystemConfirmDialog::Result::k_confirm);
			}
			else
			{
				Core::Application::OnSystemConfirmDialogResult(inudwID, Core::SystemConfirmDialog::Result::k_cancel);
			} 
		}
		//--------------------------------------------------------------------------------------------------
        /// Show System Dialog
        ///
        /// Display a system confirmation dialog with the given ID and delegate
        ///
        /// @param ID
        /// @param Title text
        /// @param Message text
        /// @param Confirm text
        //--------------------------------------------------------------------------------------------------
		void CPlatformSystem::ShowSystemDialog(u32 inudwID, const Core::UTF8String& instrTitle, const Core::UTF8String& instrMessage, const Core::UTF8String& instrConfirm) const
		{
			MessageBoxA(NULL, instrTitle.ToASCII().c_str(), instrMessage.ToASCII().c_str(), MB_OK);
		}
		//---GLFW Delegates
		//-------------------------------------------------
		/// On Window Resized (GLFW)
		///
		/// Triggered when glfw resizes the window
		///
		/// @param Window width
		/// @param Window Height
		//-------------------------------------------------
		void CPlatformSystem::OnWindowResized(s32 indwWidth, s32 indwHeight)
		{
			Core::Application::OnScreenResized((u32)indwWidth, (u32)indwHeight);
		}
		//-------------------------------------------------
		/// On Window Closed (GLFW)
		///
		/// Triggered when glfw exits the window
		//-------------------------------------------------
		void CPlatformSystem::OnWindowClosed()
		{
			Core::Application::Suspend();
			Core::Application::Quit();
			glfwTerminate();
		}
		//-----------------------------------------
		/// Destructor
		///
		/// 
		//-----------------------------------------
		CPlatformSystem::~CPlatformSystem()
		{
			glfwTerminate();
		}
	}
}
