/*
 * File: iOSPlatformSystem.cpp
 * Date: 24/11/2010 2010 
 * Description: 
 */

/*
 * Author: Scott Downie
 * Version: v 1.0
 * Copyright ©2010 Tag Games Limited - All rights reserved 
 */

#include <ChilliSource/Core/File/FileSystem.h>

#include <ChilliSource/Platform/iOS/PlatformSystem.h>
#include <ChilliSource/Platform/iOS/NativeSystem.h>
#include <ChilliSource/Platform/iOS/ImageLoader.h>
#include <ChilliSource/Platform/iOS/Input/InputSystem.h>
#include <ChilliSource/Platform/iOS/Video/VideoPlayerActivity.h>
#include <ChilliSource/Platform/iOS/FMOD/FMODSystem.h>
#include <ChilliSource/Platform/iOS/FMOD/FMODAudioLoader.h>
#include <ChilliSource/Platform/iOS/HttpConnectionSystem.h>
#include <ChilliSource/Platform/iOS/ToastNotification.h>
#include <ChilliSource/Platform/iOS/IAPSystem.h>
#include <ChilliSource/Platform/iOS/WebViewActivity.h>

#include <ChilliSource/Platform/iOS/Social/Communications/SMSCompositionActivity.h>
#include <ChilliSource/Platform/iOS/Social/Communications/EmailCompositionActivity.h>
#include <ChilliSource/Platform/iOS/Social/Communications/ContactInformationProvider.h>

#include <ChilliSource/Platform/iOS/LocalNotificationScheduler.h>
#include <ChilliSource/Platform/iOS/FileIO/FileSystem.h>
#include <ChilliSource/Platform/iOS/NSString+MD5Addition.h>
#include <ChilliSource/Platform/iOS/UIDevice+IdentifierAddition.h>

#include <ChilliSource/Rendering/Renderer.h>
#include <ChilliSource/Rendering/FontLoader.h>
#include <ChilliSource/Rendering/SpriteSheetLoader.h>
#include <ChilliSource/Rendering/XMLSpriteSheetLoader.h>
#include <ChilliSource/Rendering/MaterialLoader.h>
#include <ChilliSource/Rendering/MaterialFactory.h>
#include <ChilliSource/Rendering/ComponentUpdaters/AnimatedMeshComponentUpdater.h>

#include <ChilliSource/Audio/Main/AudioPlayer.h>
#include <ChilliSource/Audio/Main/AudioLoader.h>

#include <ChilliSource/Rendering/GUI/GUIViewFactory.h>

#include <ChilliSource/Core/String/StringUtils.h>
#include <ChilliSource/Core/Image/MoImageProvider.h>

#include <ChilliSource/Backend/Rendering/OpenGL/RenderSystem.h>
#include <ChilliSource/Backend/Rendering/OpenGL/RenderCapabilities.h>

#include <UIKit/UIKit.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <AdSupport/ASIdentifierManager.h>

namespace moFlo 
{
    DEFINE_CREATABLE(IPlatformSystem, iOSPlatform::CPlatformSystem);
    
	namespace iOSPlatform
	{   
		//-----------------------------------------
		/// Constructor
		///
		/// Default
		//-----------------------------------------
		CPlatformSystem::CPlatformSystem()
            : mfPhysicalScreenSize(-1.0f)
		{
            //---Systems
			AddSystemFunc(Networking::IHttpConnectionSystem::InterfaceID,SystemCreationFunction(this, &CPlatformSystem::CreateHttpConnectionSystem));

            AddActivityFunc(Social::IEmailCompositionActivity::InterfaceID, ActivityCreationFunction(this, &CPlatformSystem::CreateEmailCompositionActivity));
 
            //---Activities
            AddActivityFunc(Video::IVideoPlayerActivity::InterfaceID, ActivityCreationFunction(this, &CPlatformSystem::CreateDefaultVideoPlayerActivity));
			AddActivityFunc(IWebViewActivity::InterfaceID, ActivityCreationFunction(this, &CPlatformSystem::CreateWebViewActivity));
			
			if(CSMSCompositionActivity::SupportedByDevice())
            {
				AddActivityFunc(Social::ISMSCompositionActivity::InterfaceID, ActivityCreationFunction(this, &CPlatformSystem::CreateSMSCompositionActivity));
            }
			
            //---Info providers
			AddInfoProviderFunc(Social::IContactInformationProvider::InterfaceID, InfoProviderCreationFunction(this, &CPlatformSystem::CreateContactInformationProvider));

			CNotificationScheduler::Initialise(new CLocalNotificationScheduler());
			Core::CApplication::SetFileSystem(new iOSPlatform::CFileSystem());

			moFlo::CLogging::Init();
		}
        //--------------------------------------------
        /// Add System Function
        ///
        /// Map the creation function with the
        /// system type
        ///
        /// @param System interface ID
        /// @param Creation delegate
        //-------------------------------------------
		void CPlatformSystem::AddSystemFunc(Core::InterfaceIDType inInterfaceID, SystemCreationFunction inFunction)
        {
			mmapInterfaceIDToSystemFunc.insert(std::make_pair(inInterfaceID,inFunction));
		}
        //--------------------------------------------
        /// Add Activity Function
        ///
        /// Map the creation function with the
        /// activity type
        ///
        /// @param Activity interface ID
        /// @param Creation delegate
        //-------------------------------------------
		void CPlatformSystem::AddActivityFunc(Core::InterfaceIDType inInterfaceID, ActivityCreationFunction inFunction)
        {
			mmapInterfaceIDToActivityFunc.insert(std::make_pair(inInterfaceID,inFunction));
		}
        //--------------------------------------------
        /// Add Info Provider Function
        ///
        /// Map the creation function with the
        /// system type
        ///
        /// @param Info provider interface ID
        /// @param Creation delegate
        //-------------------------------------------
		void CPlatformSystem::AddInfoProviderFunc(Core::InterfaceIDType inInterfaceID, InfoProviderCreationFunction inFunction)
        {
			mmapInterfaceIDToInfoProviderFunc.insert(std::make_pair(inInterfaceID,inFunction));
		}
        //-------------------------------------------
        /// Find System Implementing
        ///
        /// Identify if the system already exists
        /// to prevent creation of duplicate 
        /// systems
        ///
        /// @param Interface ID
        /// @param Exisiting systems
        /// @return Pointer to system
        //-------------------------------------------
		Core::ISystem* CPlatformSystem::FindSystemImplementing(Core::InterfaceIDType inInterfaceID, const DYNAMIC_ARRAY<Core::SystemPtr>& inSystems) const
        {
			for(u32 nSystem = 0; nSystem < inSystems.size(); nSystem++)
            {
				if (inSystems[nSystem]->IsA(inInterfaceID))
                {
					return inSystems[nSystem].get();
				}
			}
			
			return NULL;
		}
		//-----------------------------------------
		/// Init
		//-----------------------------------------
		void CPlatformSystem::Init()
		{
            //Initialise GUI factory
            GUI::CGUIViewFactory::RegisterDefaults();
		}
		//-------------------------------------------------
		/// Create Default Systems
		///
		/// Adds default systems to the applications system
		/// list.
		///
		/// @param the system list
		//-------------------------------------------------
		void CPlatformSystem::CreateDefaultSystems(DYNAMIC_ARRAY<Core::SystemPtr> & inaSystems)
		{
			//create the main systems
            Rendering::IRenderSystem* pRenderSystem = new OpenGL::CRenderSystem();
            inaSystems.push_back(Core::SystemPtr(pRenderSystem));
			Core::CApplication::SetRenderSystem(pRenderSystem);
            
            Input::IInputSystem* pInputSystem = new iOSPlatform::CInputSystem();
            inaSystems.push_back(Core::SystemPtr(pInputSystem));
            Core::CApplication::SetInputSystem(pInputSystem);
            
            Audio::IAudioSystem * pAudioSystem = new iOSPlatform::CFMODSystem();
			inaSystems.push_back(Core::SystemPtr(pAudioSystem));
			inaSystems.push_back(Core::SystemPtr(new iOSPlatform::CFMODAudioLoader(pAudioSystem)));
			Core::CApplication::SetAudioSystem(pAudioSystem);
            
			//create other important systems
			OpenGL::CRenderCapabilities* pRenderCapabilities = new OpenGL::CRenderCapabilities();
            inaSystems.push_back(Core::SystemPtr(pRenderCapabilities));
            inaSystems.push_back(Core::SystemPtr(new iOSPlatform::ImageLoader()));
            inaSystems.push_back(Core::SystemPtr(new CMoImageProvider()));
			inaSystems.push_back(Core::SystemPtr(new Rendering::CSpriteSheetLoader()));
			inaSystems.push_back(Core::SystemPtr(new Rendering::CXMLSpriteSheetLoader()));
			inaSystems.push_back(Core::SystemPtr(new Rendering::CMaterialLoader(pRenderCapabilities)));
			inaSystems.push_back(Core::SystemPtr(new Rendering::CFontLoader()));
            inaSystems.push_back(Core::SystemPtr(new Rendering::CAnimatedMeshComponentUpdater()));
            inaSystems.push_back(Core::SystemPtr(new Rendering::CMaterialFactory()));
            
			//Initialise the render system
			Core::CApplication::GetRenderSystemPtr()->Init((u32)Core::CScreen::GetRawDimensions().x, (u32)Core::CScreen::GetRawDimensions().y);
            
			//Create the renderer
			Core::CApplication::SetRenderer(new Rendering::CRenderer(Core::CApplication::GetRenderSystemPtr()));
            
			//Initialise the input system
			if(Core::CApplication::GetInputSystemPtr() != NULL)
			{
				Core::CApplication::SetHasTouchInput((Core::CApplication::GetInputSystemPtr()->GetTouchScreenPtr() != NULL));
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
            if(Core::CApplication::GetAudioSystemPtr() != NULL)
			{
				Audio::CAudioPlayer::Init();
			}
		}
        //-----------------------------------------
        /// Run
        ///
        /// Begin the game loop
        //-----------------------------------------
        void CPlatformSystem::Run()
        {
            iOSInit();
        }
        //-----------------------------------------
        /// Set Max FPS
        ///
        /// @param The maximum frames per second
        /// to clamp to. This should be in multiples
        /// of 15 (15, 30, 60)
        //-----------------------------------------
        void CPlatformSystem::SetMaxFPS(u32 inudwFPS)
        {
            iOSSetMaxFPS(inudwFPS);
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
			iOSSetUpdaterActive(inbIsActive);
		}
        //-----------------------------------------
        /// Terminate Updater
        ///
        /// Stops the update loop causing
        /// the application to terminate
        //-----------------------------------------
        void CPlatformSystem::TerminateUpdater() 
        {
            iOSInvalidateUpdater();
        }
        //-----------------------------------------
        /// Can Create System With Interface
        ///
        /// @param Interface ID
        /// @return Whether system can be created
        //----------------------------------------
		bool CPlatformSystem::CanCreateSystemWithInterface(Core::InterfaceIDType inInterfaceID) const
        {
			MapInterfaceIDToSystemFunc::const_iterator pFunc(mmapInterfaceIDToSystemFunc.find(inInterfaceID));

			return pFunc != mmapInterfaceIDToSystemFunc.end();
		}
        //-----------------------------------------
        /// Create and Add System With Interface
        ///
        /// Convenience template method of the above returning the needed interface type.
        ///
        /// @param InterfaceID to generate
        /// @param Vector of existing systems
        /// @return A handle to the given system or NULL if the platform cannot support it
        //-----------------------------------------
		Core::ISystem* CPlatformSystem::CreateAndAddSystemWithInterface(Core::InterfaceIDType inInterfaceID, DYNAMIC_ARRAY<Core::SystemPtr> & inaExistingSystems) const
        {
			Core::ISystem * pResult = NULL;
			
			MapInterfaceIDToSystemFunc::const_iterator pFunc(mmapInterfaceIDToSystemFunc.find(inInterfaceID));
			
			if (pFunc != mmapInterfaceIDToSystemFunc.end())
            {
				pResult = pFunc->second(inaExistingSystems);
			}
			
			if (pResult)
            {
				inaExistingSystems.push_back(Core::SystemPtr(pResult));
			}
			
			return pResult;
		}
		//-----------------------------------------
        /// Can Create Activity With Interface
        ///
		/// @param Interface ID
		/// @return Whether activity can be created
		//----------------------------------------
		bool CPlatformSystem::CanCreateActivityWithInterface(Core::InterfaceIDType inInterfaceID) const
        {
			MapInterfaceIDToActivityFunc::const_iterator pFunc(mmapInterfaceIDToActivityFunc.find(inInterfaceID));
			
			return pFunc != mmapInterfaceIDToActivityFunc.end();
		}
		//-----------------------------------------
        /// Create Activity With Interface
        ///
		/// Tries to create a platform specific implementation with the given interface
		///
		/// @param InterfaceID to generate
		/// @return A handle to the given activity or NULL if the platform cannot support it
		//-----------------------------------------
		IActivity* CPlatformSystem::CreateActivityWithInterface(Core::InterfaceIDType inInterfaceID) const
        {
			MapInterfaceIDToActivityFunc::const_iterator pFunc(mmapInterfaceIDToActivityFunc.find(inInterfaceID));
			
			if (pFunc != mmapInterfaceIDToActivityFunc.end())
            {
				return pFunc->second();
			}
			
			return NULL;
		}
		//-----------------------------------------
        /// Can Create Information Provider With Interface
        ///
		/// @param Interface ID
		/// @return Whether system can be created
		//----------------------------------------
		bool CPlatformSystem::CanCreateInformationProviderWithInterface(Core::InterfaceIDType inInterfaceID) const
        {
			MapInterfaceIDToInfoProviderFunc::const_iterator pFunc(mmapInterfaceIDToInfoProviderFunc.find(inInterfaceID));
			
			return pFunc != mmapInterfaceIDToInfoProviderFunc.end();
		}
		//-----------------------------------------
        /// Create Information Provider With Interface
        ///
		/// Tries to create a platform specific implementation with the given interface
		///
		/// @param InterfaceID to generate
		/// @return A handle to the given system or NULL if the platform cannot support it
		//-----------------------------------------
		IInformationProvider* CPlatformSystem::CreateInformationProviderWithInterface(Core::InterfaceIDType inInterfaceID) const
        {
			MapInterfaceIDToInfoProviderFunc::const_iterator pFunc(mmapInterfaceIDToInfoProviderFunc.find(inInterfaceID));
			if (pFunc != mmapInterfaceIDToInfoProviderFunc.end())
            {
				return pFunc->second();
			}
			return NULL;
		}
		//--------------------------------------------
		/// Create Systems
		///
		/// Methods that create concrete systems 
        /// for this platform
		///
        /// @param System list
		/// @return A pointer to the system
		//--------------------------------------------
		Core::ISystem * CPlatformSystem::CreateHttpConnectionSystem(DYNAMIC_ARRAY<Core::SystemPtr>& inSystems) const
        {
			return new CHttpConnectionSystem();
		}
        //--------------------------------------------
        /// Create Activities
        ///
        /// Creates an instance of the activity
        ///
        /// @return Ownership of the activity
        //--------------------------------------------
		IActivity* CPlatformSystem::CreateSMSCompositionActivity() const
        {
			return new CSMSCompositionActivity();
		}
		IActivity* CPlatformSystem::CreateEmailCompositionActivity() const
        {
			return new CEmailCompositionActivity();
		}
		IActivity * CPlatformSystem::CreateDefaultVideoPlayerActivity() const
        {
            return new CVideoPlayerActivity();
        }
		IActivity * CPlatformSystem::CreateWebViewActivity() const
        {
            return new CWebViewActivity();
        }
        //--------------------------------------------
        /// Create Information Providers
        ///
        /// Creates an instance of the info provider
        ///
        /// @return Ownership of the info provider
        //--------------------------------------------
		IInformationProvider* CPlatformSystem::CreateContactInformationProvider() const
        {
			return new CContactInformationProvider();
		}
		//-----------------------------------------------------------------------------------------------------------
		/// Get Screen Dimensions
		///
		/// Retrieves the screen dimensions. These dimensions are always in the default orientation for the device.
		/// @return A CVector2 containing the screen size in it's x + y components
        ///
        /// The dimensions are always for the base screen size and the density scale factor is use to scale the screen
        /// to the correct resolution. i.e. the Retina screen will be 320x480 and a density of 2.0
		//-----------------------------------------------------------------------------------------------------------
		Core::CVector2 CPlatformSystem::GetScreenDimensions() const
		{
			Core::CVector2 Result;
			CGSize Size = [[UIScreen mainScreen] bounds].size;
            
            f32 fScale = 1.0f;
            
            bool bIsiOS4_0 = ([[[UIDevice currentDevice] systemVersion] floatValue] >= 4.0f);
            
            if(bIsiOS4_0)
            {
                fScale = [UIScreen mainScreen].scale;
            }
            
			Result.x = Size.width * fScale;
			Result.y = Size.height * fScale;
            
			return Result;
		}
		//--------------------------------------------------------------
		/// Get Device Model Name
		///
		/// @return The above information stringified
		//--------------------------------------------------------------
		std::string CPlatformSystem::GetDeviceModelName() const
		{
			NSString * nsType = [[UIDevice currentDevice] model];

			return (moFlo::Core::CStringUtils::NSStringToString(nsType));
		}
		//--------------------------------------------------------------
		/// Get Device Model Type Name
		///
		/// @return The above information stringified
		//--------------------------------------------------------------
		std::string CPlatformSystem::GetDeviceModelTypeName() const
		{
			size_t size = 0;
			sysctlbyname("hw.machine", NULL, &size, NULL, 0);
			char* machine = (char*)malloc(size);
			sysctlbyname("hw.machine", machine, &size, NULL, 0);
			NSString *platform = [NSString stringWithCString:machine encoding:NSASCIIStringEncoding];
			free(machine);

			std::string strOutput;
			std::string strModelType = moFlo::Core::CStringUtils::NSStringToString(platform);
			bool bRecord = false;
			for(std::string::const_iterator it = strModelType.begin(); it != strModelType.end(); ++it)
			{
				if(isdigit(*it))
				{
					bRecord = true;
				}
				
				if(bRecord)
				{
					strOutput += (*it);
				}
			}
			return strOutput;
		}
		//--------------------------------------------------------------
		/// Get Device Manufacturer Name
		///
		/// @return The above information stringified
		//--------------------------------------------------------------
		std::string CPlatformSystem::GetDeviceManufacturerName() const
		{
			return std::string("Apple");
		}
        //--------------------------------------------------------------
        /// Get OS Version
        ///
        /// @return String containing the OS version of the device
        //--------------------------------------------------------------
        std::string CPlatformSystem::GetOSVersion() const
        {
            NSString* NSVersion = [[UIDevice currentDevice] systemVersion];
			return moFlo::Core::CStringUtils::NSStringToString(NSVersion);
        }
        //--------------------------------------------------------------
		/// Get Locale
		///
		/// Get the active locale of the device
		/// @return Locale ID
		//--------------------------------------------------------------
		Core::CLocale CPlatformSystem::GetLocale() const
		{
			NSLocale *pcLocale = [NSLocale currentLocale];
			NSString *pcCountryCode = [pcLocale objectForKey:NSLocaleCountryCode];
			std::string strCountryCode = [pcCountryCode UTF8String];
			NSString *pcLanguageCode = [pcLocale objectForKey:NSLocaleLanguageCode];
			std::string strLanguageCode = [pcLanguageCode UTF8String];

			//Just default to english
			return moFlo::Core::CLocale(strLanguageCode, strCountryCode);
		}
        //--------------------------------------------------------------
		/// Get Language
		///
		/// Get the active language of the device in locale format
		/// @return Locale ID
		//--------------------------------------------------------------
		Core::CLocale CPlatformSystem::GetLanguage() const
		{
			NSUserDefaults* UserDefaults = [NSUserDefaults standardUserDefaults];
			NSArray* SupportedLanguages = [UserDefaults objectForKey:@"AppleLanguages"];
			NSString* NSUserLocale = [SupportedLanguages objectAtIndex:0];
			std::string strLocaleCode = [NSUserLocale UTF8String];

			//break this locale into parts(language/country code/extra)
			DYNAMIC_ARRAY<std::string> strLocaleBrokenUp = moFlo::Core::CStringUtils::Split(strLocaleCode, "-", 0);

			if (strLocaleBrokenUp.size() > 1)
			{
				return Core::CLocale(strLocaleBrokenUp[0],strLocaleBrokenUp[1]);
			}
			else if (strLocaleBrokenUp.size() == 1)
			{
				return Core::CLocale(strLocaleBrokenUp[0]);
			}
			else
				return Core::kUnknownLocale;
		}
        //-------------------------------------------------
        /// Get Screen Density
        ///
        /// @return The density scale factor of the screen
        /// to convert from DIPS to physical pixels
        //-------------------------------------------------
        f32 CPlatformSystem::GetScreenDensity() const
        {
            if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 4.0f) 
            {
				CGFloat fScale = [UIScreen mainScreen].scale;
                
                return fScale;
			}
            
            return 1.0f;
        }
        //-------------------------------------------------
        /// Get Device ID
        ///
        /// @return The UDID of the device
        //-------------------------------------------------
        std::string CPlatformSystem::GetDeviceID()
        {
            if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 6.0f)
            {
                NSUUID* uid = nil;
                
                if([ASIdentifierManager sharedManager].advertisingTrackingEnabled)
                {
                    uid = [ASIdentifierManager sharedManager].advertisingIdentifier;
                }
                else
                {
                    uid = [UIDevice currentDevice].identifierForVendor;
                }
                
                return moFlo::Core::CStringUtils::NSStringToString([uid UUIDString]);
            }
            else
            {
                NSString* strUDID = [[UIDevice currentDevice] uniqueGlobalDeviceIdentifier];
                return moFlo::Core::CStringUtils::NSStringToString(strUDID);
            }
        }
        //-------------------------------------------------
        /// Get App Version
        ///
        /// @return The bundle version as found in the plist
        //-------------------------------------------------
        std::string CPlatformSystem::GetAppVersion() const
        {
            NSString* strVersion = [[NSBundle mainBundle] objectForInfoDictionaryKey:(NSString *)kCFBundleVersionKey];
            return moFlo::Core::CStringUtils::NSStringToString(strVersion);
        }
		//--------------------------------------------------------------
		/// Get Number Of CPU Cores
		///
		/// @return The number of cores available
		//--------------------------------------------------------------
		u32 CPlatformSystem::GetNumberOfCPUCores() const
		{
			u32 udwNumCores = 1;
			size_t udwSize = sizeof(udwNumCores);
			
			if(sysctlbyname("hw.ncpu", &udwNumCores, &udwSize, NULL, 0)) 
			{
				return 1;
			}
			else
			{
				return udwNumCores;
			}
		}
        //--------------------------------------------------------------------------------------------------
        /// Make Toast
        ///
        /// Display a toast notification with the given text
        ///
        /// @param Text
        //--------------------------------------------------------------------------------------------------
        void CPlatformSystem::MakeToast(const UTF8String& instrText) const
        {
            ToastNotification* pToast = [[ToastNotification alloc] initWithMessage:Core::CStringUtils::UTF8StringToNSString(instrText)];
            [[EAGLView sharedInstance] addSubview:pToast];
            [pToast animateIn];
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
        void CPlatformSystem::ShowSystemConfirmDialog(u32 inudwID, const UTF8String& instrTitle, const UTF8String& instrMessage, const UTF8String& instrConfirm, const UTF8String& instrCancel) const
        {
            iOSShowSystemConfirmDialog(inudwID, 
                                       Core::CStringUtils::UTF8StringToNSString(instrTitle), Core::CStringUtils::UTF8StringToNSString(instrMessage), 
                                       Core::CStringUtils::UTF8StringToNSString(instrConfirm), Core::CStringUtils::UTF8StringToNSString(instrCancel));
        }
        //--------------------------------------------------------------------------------------------------
        /// Show System Dialog
        ///
        /// Display a system dialog with the given ID and delegate
        ///
        /// @param ID
        /// @param Title text
        /// @param Message text
        /// @param Confirm text
        //--------------------------------------------------------------------------------------------------
        void CPlatformSystem::ShowSystemDialog(u32 inudwID, const UTF8String& instrTitle, const UTF8String& instrMessage, const UTF8String& instrConfirm) const
        {
            iOSShowSystemDialog(inudwID,
                                Core::CStringUtils::UTF8StringToNSString(instrTitle), Core::CStringUtils::UTF8StringToNSString(instrMessage),
                                Core::CStringUtils::UTF8StringToNSString(instrConfirm));
        }
		//--------------------------------------------------------------
		/// Get System Time
		///
		/// @return The current time in milliseconds
		//--------------------------------------------------------------
		TimeIntervalMs CPlatformSystem::GetSystemTimeMS() const
		{
			return GetSystemTimeInNanoSeconds() / 1000000;
		}
		//-------------------------------------------------
		/// Get Physical Screen Size
		///
		/// @return The physical size of the screen in
		/// inches.
		//-------------------------------------------------
		f32 CPlatformSystem::GetPhysicalScreenSize()
		{
            if (mfPhysicalScreenSize < 0.0f)
            {
                size_t size = 0;
                sysctlbyname("hw.machine", NULL, &size, NULL, 0);
                char* machine = (char*)malloc(size);
                sysctlbyname("hw.machine", machine, &size, NULL, 0);
                NSString *platform = [NSString stringWithCString:machine encoding:NSASCIIStringEncoding];
                free(machine);
                
                std::string strDeviceName = moFlo::Core::CStringUtils::NSStringToString(platform);
                
                //3.5 inch screens
                if (strDeviceName == "iPhone1,1" || strDeviceName == "iPhone1,2" || strDeviceName == "iPhone2,1" || strDeviceName == "iPhone3,1" || strDeviceName == "iPhone3,2" ||
                    strDeviceName == "iPhone3,3" || strDeviceName == "iPhone4,1" || strDeviceName == "iPod1,1" || strDeviceName == "iPod2,1" || strDeviceName == "iPod3,1" || strDeviceName == "iPod4,1")
                {
                    mfPhysicalScreenSize = 3.5f;
                }
                
                //4 inch screens
                else if (strDeviceName == "iPhone5,1" || strDeviceName == "iPhone5,2" || strDeviceName == "iPod5,1")
                {
                    mfPhysicalScreenSize = 4.0f;
                }
                
                //7.9 inch screens
                else if (strDeviceName == "iPad2,5" || strDeviceName == "iPad2,6" || strDeviceName == "iPad2,7")
                {
                    mfPhysicalScreenSize = 7.9f;
                }
                
                //9.7 inch screens
                else if (strDeviceName == "iPad1,1" || strDeviceName == "iPad2,1" || strDeviceName == "iPad2,2" || strDeviceName == "iPad2,3" || strDeviceName == "iPad2,4" || strDeviceName == "iPad3,1" ||
                         strDeviceName == "iPad3,2" || strDeviceName == "iPad3,3" || strDeviceName == "iPad3,4" || strDeviceName == "iPad3,5" || strDeviceName == "iPad3,6")
                {
                    mfPhysicalScreenSize = 9.7f;
                }
                
                //Simulator
                else if (strDeviceName == "x86_64" || strDeviceName == "x86_32")
                {
                    std::string strSimulatorName = GetDeviceModelName();
                    
                    //iphone simulator
                    if (strSimulatorName == "iPhone Simulator")
                    {
                        const u32 kudwIPhone5Width = 640;
                        
                        //3.5 inch screens
                        if (GetScreenDimensions().x < kudwIPhone5Width)
                        {
                            mfPhysicalScreenSize = 3.5f;
                        }
                        
                        //4.0 inch screens
                        else if (GetScreenDimensions().x >= kudwIPhone5Width)
                        {
                            mfPhysicalScreenSize = 4.0f;
                        }
                        
                        //unknown
                        else
                        {
                            mfPhysicalScreenSize = 0.0f;
                        }
                    }
                    
                    //iPad simulator
                    else if (strSimulatorName == "iPad Simulator")
                    {
                        mfPhysicalScreenSize = 9.7f;
                    }
                }
                
                //unknown
                else
                {
                    mfPhysicalScreenSize = 0.0f;
                }
            }
            
            return mfPhysicalScreenSize;
		}
		//-----------------------------------------
		/// Destructor
		///
		/// 
		//-----------------------------------------
		CPlatformSystem::~CPlatformSystem()
		{
			
		}
	}
}
