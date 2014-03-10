//
//  CGooglePlayExpansionSystem.h
//  MoFlow
//
//  Created by Hugh McLaughlin on 25/06/2012.
//	Edited by Scott Downie on 12/04/2013
//  Copyright (c) 2012 Tag Games. All rights reserved.
//

#ifndef _MOFLOW_PLATFORM_ANDROID_GOOGLEPLAYEXPANSION_GOOGLEPLAYEXPANSIONSYSTEM_H
#define _MOFLOW_PLATFORM_ANDROID_GOOGLEPLAYEXPANSION_GOOGLEPLAYEXPANSIONSYSTEM_H

#include <ChilliSource/Backend/Platform/Android/Core/JNI/JavaInterface.h>
#include <ChilliSource/Core/JSON/json.h>
#include <ChilliSource/Core/System/System.h>

namespace ChilliSource
{
    namespace Android
    {
    	class GooglePlayExpansionJavaInterface;

    	enum class DownloadStatus
    	{
    		k_downloading,
    		k_installing,
    		k_complete,
    		k_pausedNoWiFi,
    		k_paused,
    		k_failed,
    		k_failedInsufficientStorage
    	};

        class GooglePlayExpansionSystem : public Core::System
        {
        public:
        	CS_DECLARE_NAMEDTYPE(GooglePlayExpansionSystem);

        	typedef std::function<void(DownloadStatus)> DownloadStatusDelegate;

        	GooglePlayExpansionSystem();
        	~GooglePlayExpansionSystem();

            //-------------------------------------------------------------
            /// Is A
            ///
            /// @param Interface ID 
            /// @return Whether system is off the given type
            //-------------------------------------------------------------
            bool IsA(Core::InterfaceIDType inID) const;
            //-------------------------------------------------------------
            /// Init
            ///
            //-------------------------------------------------------------
            void Init();
            //-------------------------------------------------------------
            /// Is Download Required
            ///
            /// Check to see if the files have already been downloaded
            /// and if they are complete
            ///
            /// @return Whether a download is required
            //-------------------------------------------------------------
            bool IsDownloadRequired();
            //-------------------------------------------------------------
            /// Download
            ///
            /// Attempts to download the package and install to device
            /// from the google store.
            ///
            /// @param Delegate gets called whenever the download status
            /// changes - such as complete, paused, failed, etc
            //-------------------------------------------------------------
            void Download(const DownloadStatusDelegate& inDelegate);
            //-------------------------------------------------------------
            /// Pause
            ///
            /// Suspend the currently in-progress download
            //-------------------------------------------------------------
            void Pause();
            //-------------------------------------------------------------
            /// Resume
            ///
            /// Resume the currently in-progress download
            //-------------------------------------------------------------
            void Resume();
            //-------------------------------------------------------------
            /// Get Download Progress
            ///
            /// @return Get the percentage progress of the current download
            //-------------------------------------------------------------
            f32 GetDownloadProgress();
            //-------------------------------------------------------------
            /// Get Install Progress
            ///
            /// @return Get the percentage progress of the current install
            //-------------------------------------------------------------
            f32 GetInstallProgress() const;
            //-------------------------------------------------------------
            /// Get Required Storage Space in Bytes
            ///
            /// @return Storage requirements
            //-------------------------------------------------------------
            u64 GetRequiredStorageSpaceInBytes();

            //----Called from Java
            void OnDownloadStatusChanged(DownloadStatus ineStatus);

        private:

            //-------------------------------------------------------------
            /// On Application Resume
            ///
            /// Resume listening for the current download
            //-------------------------------------------------------------
            void OnApplicationResume();
            //-------------------------------------------------------------
            /// On Application Suspend
            ///
            /// Stop listening for the current download
            //-------------------------------------------------------------
            void OnApplicationSuspend();
            //-------------------------------------------------------------
            /// Install Expansions
            ///
            /// Unzip the expansions into the DLC directory ready for
            /// use by the file system
            //-------------------------------------------------------------
            void Install();
            //-------------------------------------------------------------
            /// Do Installed Files Exist
            ///
            /// @return Whether the downloaded files require unzipping
            //-------------------------------------------------------------
            bool DoInstalledFilesExist() const;
            //-------------------------------------------------------------
            /// Get Uncompressed Zip Size
            ///
            /// @param Path to zip file
            /// @return Unzipped package size
            //-------------------------------------------------------------
            u64 GetUncompressedZipSize(const std::string& instrFilePath) const;
            //-------------------------------------------------------------
            /// Has Enough Storage Space
            ///
            /// @param Required space in bytes
            /// @return Whether enough space is available
            //-------------------------------------------------------------
            bool HasEnoughStorageSpace(u64 uddwRequired);
            //-------------------------------------------------------------
            /// Get Required Install Space in Bytes
            ///
            /// @return Storage requirements for installation
            //-------------------------------------------------------------
            u64 GetRequiredInstallSpaceInBytes();
            //-------------------------------------------------------------
            /// Get Required Download Space in Bytes
            ///
            /// @return Storage requirements for download
            //-------------------------------------------------------------
            u64 GetRequiredDownloadSpaceInBytes();
            //-------------------------------------------------------------
            /// Unzip Task
            ///
            /// A task for unzipping the expansions.
            //-------------------------------------------------------------
            void UnzipTask();
            //-------------------------------------------------------------
            /// Unzip
            ///
            /// Unzipping the expansion to a directory.
            //-------------------------------------------------------------
            void Unzip(const std::string& instrZipPath, Json::Value& outjManifest);
    		//-------------------------------------------------------------
    		/// Cache Package Descriptions
            ///
            /// Save the package version that will allow us to check
            /// whether we have the latest package
    		//-------------------------------------------------------------
            void CachePackageDescriptions();
    		//-------------------------------------------------------------
    		/// Unzip Finished Task
            ///
            /// @param Status
    		//-------------------------------------------------------------
            void UnzipCompleteTask(DownloadStatus ineStatus);
            //-------------------------------------------------------------
            /// Do Expansion Download Files Exist
            ///
            /// @param Whether the zip file exists on device
            //-------------------------------------------------------------
            bool DoExpansionDownloadFilesExist() const;
            //-------------------------------------------------------------
            /// Remove Installed Files
            ///
            /// Attempt to load the manifest and remove all the files
            /// it contains from the installed location
            //-------------------------------------------------------------
            void RemoveInstalledFiles();

        private:

            DownloadStatusDelegate mDownloadStatusDelegate;

            Core::ConnectionUPtr m_appResumeConnection;
            Core::ConnectionUPtr m_appSuspendConnection;

            u32 mudwNumExpansions;
            f32 mfInstallProgress;

            GooglePlayExpansionJavaInterface* mpJavaInterface;
        };

        class GooglePlayExpansionJavaInterface : public ChilliSource::Android::IJavaInterface
        {
        public:

        	CS_DECLARE_NAMEDTYPE(GooglePlayExpansionJavaInterface);

        	GooglePlayExpansionJavaInterface();

        	bool IsA(Core::InterfaceIDType inInterfaceID) const;

        	void Init();

        	void Download();
        	void PauseDownload();
        	void ResumeDownload();
        	f32 GetDownloadProgress();

        	bool DoExpansionFilesExist();
        	u64 GetExternalFreeStorageInBytes();

        	void KeepAppAwake();
        	void AllowAppToSleep();

        	u32 GetNumExpansions();
        	u32 GetExpansionVersionCode(u32 inudwIndex);
        	u64 GetExpansionFileSizeInBytes(u32 inudwIndex);
        	std::string GetExpansionPath(u32 inudwIndex);
        };
    }
}

#endif