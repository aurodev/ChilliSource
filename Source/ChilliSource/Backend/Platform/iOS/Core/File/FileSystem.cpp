//
//  FileSystem.cpp
//  Chilli Source
//
//  Created by I Copland on 25/03/2011.
//  Copyright 2011 Tag Games Ltd. All rights reserved.
//

#include <ChilliSource/Backend/Platform/iOS/Core/File/FileSystem.h>

#include <ChilliSource/Core/Base/Utils.h>
#include <ChilliSource/Core/File/FileStream.h>
#include <ChilliSource/Core/String/StringUtils.h>

#include <iostream>
#include <UIKit/UIKit.h>
#include <sys/types.h>
#include <sys/sysctl.h>

namespace ChilliSource
{
	namespace iOS 
	{
        namespace
        {
            const std::string k_saveDataPath  = "SaveData/";
            const std::string k_cachePath  = "Caches/Cache/";
            const std::string k_dlcPath  = "Caches/DLC/";
            
            //--------------------------------------------------------------
            /// @author I Copland
            ///
            /// @return whether or not the given file mode is a write mode
            //--------------------------------------------------------------
            bool IsWriteMode(Core::FileMode in_fileMode)
            {
                switch (in_fileMode)
                {
                    case Core::FileMode::k_write:
                    case Core::FileMode::k_writeAppend:
                    case Core::FileMode::k_writeAtEnd:
                    case Core::FileMode::k_writeBinary:
                    case Core::FileMode::k_writeBinaryAppend:
                    case Core::FileMode::k_writeBinaryAtEnd:
                    case Core::FileMode::k_writeBinaryTruncate:
                    case Core::FileMode::k_writeTruncate:
                        return true;
                    case Core::FileMode::k_read:
                    case Core::FileMode::k_readBinary:
                        return false;
                        
                }
            }
            //--------------------------------------------------------------
            /// @author S Downie
            ///
            /// @return The device bundle path as returned by iOS
            //--------------------------------------------------------------
            std::string RetrieveBundlePath()
            {
                CFBundleRef mainBundle = CFBundleGetMainBundle();
                CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
                char path[PATH_MAX];
                if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX))
                {
                    CS_LOG_ERROR("Changing working directory to resource folder");
                }
                CFRelease(resourcesURL);
                return std::string(path)  + "/";
            }
            //--------------------------------------------------------------
            /// @author S Downie
            ///
            /// @return The device documents path as returned by iOS
            //--------------------------------------------------------------
            std::string RetrieveDocumentsPath()
            {
                NSArray* documentDir = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
                if (documentDir)
                {
                    NSString*	nsDocumentPath = nil;
                    nsDocumentPath = [documentDir objectAtIndex:0];
                    if (nsDocumentPath != nil)
                    {
                        const char* pPath = [nsDocumentPath fileSystemRepresentation];
                        return std::string(pPath) + "/";
                    }
                }
                
                return "";
            }
            //--------------------------------------------------------------
            /// @author S Downie
            ///
            /// @return The device library path as returned by iOS
            //--------------------------------------------------------------
            std::string RetrieveLibraryPath()
            {
                NSArray* libraryDir = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES);
                if (libraryDir)
                {
                    NSString*	nsLibraryPath = nil;
                    nsLibraryPath = [libraryDir objectAtIndex:0];
                    if (nsLibraryPath != nil)
                    {
                        const char* pPath = [nsLibraryPath fileSystemRepresentation];
                        return std::string(pPath) + "/";
                    }
                }
                
                return "";
            }
            //------------------------------------------------------------
            /// @author S Downie
            ///
            /// @param Directories
            /// @param Recurse into sub directories
            /// @param Out: Content file names
            //------------------------------------------------------------
            void GetDirectoryContents(const std::vector<std::string>& inastrDirs, bool inbRecursive, NSMutableArray* outpContents)
            {
                for(std::vector<std::string>::const_iterator it = inastrDirs.begin(); it != inastrDirs.end(); ++it)
                {
                    std::string path = ChilliSource::Core::StringUtils::StandardisePath(*it);
                    NSString* Dir = [NSString stringWithCString:path.c_str() encoding:NSASCIIStringEncoding];
                    
                    if (inbRecursive == true)
                    {
                        [outpContents addObjectsFromArray:[[NSFileManager defaultManager] subpathsOfDirectoryAtPath:Dir error:nil]];
                    }
                    else
                    {
                        [outpContents addObjectsFromArray:[[NSFileManager defaultManager] contentsOfDirectoryAtPath:Dir error:nil]];
                    }
                }
            }
            //--------------------------------------------------------------
            /// @author S Downie
            ///
            /// @param Unfiltered names
            /// @param Extension
            /// @return Filtered names
            //--------------------------------------------------------------
            NSArray* FilterFileNamesByExtension(NSArray* inpFilenames, const std::string& instrExtension)
            {
                //Filter out the files we don't want
                NSString* Extension = [NSString stringWithCString:instrExtension.c_str() encoding:NSASCIIStringEncoding];
                NSString* Predicate = [NSString stringWithFormat:@"self ENDSWITH '.%@'", Extension];
                NSArray* Filtered = [inpFilenames filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:Predicate]];
                return Filtered;
            }
            //--------------------------------------------------------------
            /// @author S Downie
            ///
            /// @param Unfiltered names
            /// @param Name
            /// @return Filtered names
            //--------------------------------------------------------------
            NSArray* FilterFileNamesByName(NSArray* inpFilenames, const std::string& instrName)
            {
                //Filter out the files we don't want
                NSString* Name = [NSString stringWithCString:instrName.c_str() encoding:NSASCIIStringEncoding];
                NSString* Predicate = [NSString stringWithFormat:@"self ENDSWITH '%@'", Name];
                NSArray* Filtered = [inpFilenames filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:Predicate]];
                return Filtered;
            }
            //--------------------------------------------------------------
            /// @author S Downie
            ///
            /// @param Unfiltered names
            /// @return Filtered names
            //--------------------------------------------------------------
            NSArray* FilterFileNamesByFile(NSArray* inpFilenames)
            {
                //Filter out the files we don't want
                NSString* Predicate = [NSString stringWithFormat:@"self contains '.'"];
                NSArray* Filtered = [inpFilenames filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:Predicate]];
                return Filtered;
            }
            //--------------------------------------------------------------
            /// @author S Downie
            ///
            /// @param Filenames ObjC
            /// @param Directory to append
            /// @param Out: Filenames
            //--------------------------------------------------------------
            void ConvertObjCToPath(NSArray* inpFilenames, const std::string& instrDirectory, std::vector<std::string> &outstrFileNames)
            {
                std::string strDir;
                if(instrDirectory.empty() == false)
                {
                    strDir = ChilliSource::Core::StringUtils::StandardisePath(instrDirectory);
                }
                
                for(NSString* FileNames in inpFilenames)
                {
                    outstrFileNames.push_back(strDir + ChilliSource::Core::StringUtils::NSStringToString(FileNames));
                }
            }
        }
        //--------------------------------------------------------------
        //--------------------------------------------------------------
		FileSystem::FileSystem()
		{
            m_bundlePath = RetrieveBundlePath();
            m_documentsPath = RetrieveDocumentsPath();
            m_libraryPath = RetrieveLibraryPath();
            
            CreateDirectory(Core::StorageLocation::k_saveData, "");
            CreateDirectory(Core::StorageLocation::k_cache, "");
            CreateDirectory(Core::StorageLocation::k_DLC, "");
            
            CreateHashedBundleFileList();
		}
        //--------------------------------------------------------------
        //--------------------------------------------------------------
        void FileSystem::CreateHashedBundleFileList()
        {
            NSAutoreleasePool* pPool = [[NSAutoreleasePool alloc] init];
            NSMutableArray* Contents = [[NSMutableArray alloc] init];
            NSString* Dir = [NSString stringWithCString:m_bundlePath.c_str() encoding:NSASCIIStringEncoding];
            
            [Contents addObjectsFromArray:[[NSFileManager defaultManager] subpathsOfDirectoryAtPath:Dir error:nil]];
            
            for(NSString* file in Contents)
            {
                std::string strFile([file UTF8String]);
                m_hashedPackageFileNames.push_back(Core::HashCRC32::GenerateHashCode(strFile));
            }
            
            std::sort(m_hashedPackageFileNames.begin(), m_hashedPackageFileNames.end());
            
            [Contents release];
            [pPool release];
        }
        //--------------------------------------------------------------
        //--------------------------------------------------------------
        Core::FileStreamUPtr FileSystem::CreateFileStream(Core::StorageLocation in_storageLocation, const std::string& in_filePath, Core::FileMode in_fileMode) const
        {
            Core::FileStreamUPtr fileStream = Core::FileStreamUPtr(new Core::FileStream());
            if (IsWriteMode(in_fileMode) == true)
            {
                CS_ASSERT(IsStorageLocationWritable(in_storageLocation), "Trying to write to read only storage location!");
 
                std::string filePath = GetPathToStorageLocation(in_storageLocation) + in_filePath;
                fileStream->Open(filePath, in_fileMode);
            }
            else
            {
                std::string filePath = GetAbsolutePathForFile(in_storageLocation, in_filePath);
                fileStream->Open(filePath, in_fileMode);
            }
            
			return fileStream;
        }
        //--------------------------------------------------------------
        //--------------------------------------------------------------
        bool FileSystem::CreateDirectory(Core::StorageLocation in_storageLocation, const std::string& in_directoryPath) const //<--- Continue here!
        {
            CS_ASSERT(IsStorageLocationWritable(in_storageLocation), "Trying to write to read only storage location!");
            
            //create the directory
            NSAutoreleasePool* pPool = [[NSAutoreleasePool alloc] init];
            std::string path = GetPathToStorageLocation(in_storageLocation) + in_directoryPath;
			NSFileManager* fileManager = [NSFileManager defaultManager];
            if (![fileManager fileExistsAtPath:[NSString stringWithUTF8String:path.c_str()]])
            {
                if (![fileManager createDirectoryAtPath:[NSString stringWithUTF8String:path.c_str()] withIntermediateDirectories:YES attributes:nil error:nil])
                {
                    CS_LOG_ERROR("Error creating directory.");
                    [pPool release];
                    return false;
                }
            }
            [pPool release];
            
            //return successful
			return true;
        }
        //--------------------------------------------------------------
        //--------------------------------------------------------------
        bool FileSystem::CopyFile(Core::StorageLocation ineSourceStorageLocation, const std::string& instrSourceFilepath, 
                                   Core::StorageLocation ineDestinationStorageLocation, const std::string& instrDestinationFilepath) const <------- COntinue here!!
        {
            
            //insure that the destination location is writable.
            if (IsStorageLocationWritable(ineDestinationStorageLocation) == false)
            {
                CS_LOG_ERROR("Cannot write to the destination Storage Location!");
                return false;
            }
            
            if(!DoesFileExist(ineSourceStorageLocation, instrSourceFilepath))
            {
                CS_LOG_ERROR("Source file does not exist -  " + instrSourceFilepath);
                return false;
            }
            
            std::string strSrcPath;
            GetBestPathToFile(ineSourceStorageLocation, instrSourceFilepath, strSrcPath);
            
            //get the path to the file
            std::string strPath, strName;
            Core::StringUtils::SplitFilename(instrDestinationFilepath, strName, strPath);
            
            //create the output directory
            CreateDirectory(ineDestinationStorageLocation, strPath);
            
            std::string strDstPath = GetStorageLocationDirectory(ineDestinationStorageLocation) + instrDestinationFilepath;
            std::string strDstAtomicPath = strDstPath + ".tmp";
            
            NSAutoreleasePool* pPool = [[NSAutoreleasePool alloc] init];
            
			NSString* pSrcPath = [[NSString alloc] initWithCString:strSrcPath.c_str() encoding:NSUTF8StringEncoding];
            NSString* pDstPath = [[NSString alloc] initWithCString:strDstPath.c_str() encoding:NSUTF8StringEncoding];
            NSString* pDstAtomicPath = [[NSString alloc] initWithCString:strDstAtomicPath.c_str() encoding:NSUTF8StringEncoding];
            
            NSURL* pDstURL = [NSURL URLWithString:[pDstPath stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding]];
            NSURL* pDstAtomicURL = [NSURL URLWithString:[pDstAtomicPath stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding]];
            
            NSError* pCopyError = nil;
            [[NSFileManager defaultManager] copyItemAtPath:pSrcPath toPath:pDstAtomicPath error:&pCopyError];
    
            NSError* pReplaceError = nil;
            [[NSFileManager defaultManager] replaceItemAtURL:pDstURL withItemAtURL:pDstAtomicURL backupItemName:nil options:NSFileManagerItemReplacementUsingNewMetadataOnly resultingItemURL:nil error:&pReplaceError];
            
            [pSrcPath release];
            [pDstPath release];
            [pDstAtomicPath release];
			[pPool release];
            
            return pCopyError == nil && pReplaceError == nil;
        }
        //--------------------------------------------------------------
        //--------------------------------------------------------------
        bool FileSystem::CopyDirectory(Core::StorageLocation ineSourceStorageLocation, const std::string& instrSourceDirectory, 
                                        Core::StorageLocation ineDestinationStorageLocation, const std::string& instrDestinationDirectory) const
        {
            //check the requested source storage location is available
            if (IsStorageLocationAvailable(ineSourceStorageLocation) == false)
            {
                CS_LOG_ERROR("Requested source Storage Location is not available on this platform!");
                return false;
            }
            
            //check the requested destination storage location is available
            if (IsStorageLocationAvailable(ineDestinationStorageLocation) == false)
            {
                CS_LOG_ERROR("Requested destination Storage Location is not available on this platform!");
                return false;
            }
            
            //insure that the destination location is writable.
            if (IsStorageLocationWritable(ineDestinationStorageLocation) == false)
            {
                CS_LOG_ERROR("Cannot write to the destination Storage Location!");
                return false;
            }
            
            //get all the files in the directory
            std::vector<std::string> astrFilenames;
            GetFileNamesInDirectory(ineSourceStorageLocation, instrSourceDirectory, true, astrFilenames);
            
            //error if there are no files
            if (astrFilenames.size() == 0)
            {
                CS_LOG_ERROR("Cannot copy contents of directory as there are no files: " + instrSourceDirectory);
                return false;
            }
            
            //copy each of these files individually
            std::string strSourceProperPath = Core::StringUtils::StandardisePath(instrSourceDirectory);
            std::string strDestProperPath = Core::StringUtils::StandardisePath(instrDestinationDirectory);
            for (std::vector<std::string>::iterator it = astrFilenames.begin(); it != astrFilenames.end(); ++it)
            {
                if (CopyFile(ineSourceStorageLocation, strSourceProperPath + *it, 
                         ineDestinationStorageLocation, strDestProperPath + *it) == false)
                {
                    return false;
                }
            }
            
            return true;
        }
        //--------------------------------------------------------------
        //--------------------------------------------------------------
        bool FileSystem::DeleteFile(Core::StorageLocation ineStorageLocation, const std::string& instrFilepath) const
        {
            //check the requested storage location is available
            if (IsStorageLocationAvailable(ineStorageLocation) == false)
            {
                CS_LOG_ERROR("Requested Storage Location is not available on this platform!");
                return false;
            }
            
            //insure that the storage location is writable.
            if (IsStorageLocationWritable(ineStorageLocation) == false)
            {
                CS_LOG_ERROR("Cannot write to the requested Storage Location!");
                return false;
            }
            
            //get the filepath
            std::string strPath = GetStorageLocationDirectory(ineStorageLocation) + instrFilepath;
            
            //remove the file
            NSAutoreleasePool* pPool = [[NSAutoreleasePool alloc] init];
			CFStringRef cfStringRef = CFStringCreateWithCString(kCFAllocatorDefault, strPath.c_str(), kCFStringEncodingMacRoman);
            [[NSFileManager defaultManager] removeItemAtPath:(NSString*)cfStringRef error:nil];
			CFRelease(cfStringRef); 
			[pPool release];
            
            //return successful
            return true;
        }
        //--------------------------------------------------------------
        //--------------------------------------------------------------
        bool FileSystem::DeleteDirectory(Core::StorageLocation ineStorageLocation, const std::string& instrDirectory) const
        {
            //check the requested storage location is available
            if (IsStorageLocationAvailable(ineStorageLocation) == false)
            {
                CS_LOG_ERROR("Requested Storage Location is not available on this platform!");
                return false;
            }
            
            //insure that the storage location is writable.
            if (IsStorageLocationWritable(ineStorageLocation) == false)
            {
                CS_LOG_ERROR("Cannot write to the requested Storage Location!");
                return false;
            }
            
            //get the directory
            std::string strDirectory = GetStorageLocationDirectory(ineStorageLocation) + instrDirectory;
            
            //remove the directory
            NSAutoreleasePool* pPool = [[NSAutoreleasePool alloc] init];
			CFStringRef cfStringRef = CFStringCreateWithCString(kCFAllocatorDefault, strDirectory.c_str(), kCFStringEncodingMacRoman);
            [[NSFileManager defaultManager] removeItemAtPath:(NSString*)cfStringRef error:nil];
			CFRelease(cfStringRef); 
			[pPool release];
            
            //return successful
            return true;
        }
        //--------------------------------------------------------------
        //--------------------------------------------------------------
        void FileSystem::GetFileNamesWithExtensionInDirectory(Core::StorageLocation ineStorageLocation, const std::string& instrDirectory, bool inbRecurseIntoSubDirectories,
                                                  const std::string& instrExtension, std::vector<std::string> &outstrFileNames, bool inbAppendFullPath) const
        {
            //Check that this storage location is available
            if (IsStorageLocationAvailable(ineStorageLocation) == false)
            {
                CS_LOG_ERROR("Requested Storage Location is not available!");
                return;
            }
            
            std::vector<std::string> astrDirectoriesToCheck;
            GetPathsForStorageLocation(ineStorageLocation, instrDirectory, astrDirectoriesToCheck);
            
            NSAutoreleasePool* pPool = [[NSAutoreleasePool alloc] init];
            NSMutableArray* Contents = [[NSMutableArray alloc] init];
            
            GetDirectoryContents(astrDirectoriesToCheck, inbRecurseIntoSubDirectories, Contents);
            
            if([Contents count] > 0)
            {
                NSArray* Filtered = FilterFileNamesByExtension(Contents, instrExtension);

                if(inbAppendFullPath)
                {
                    ConvertObjCToPath(Filtered, instrDirectory, outstrFileNames);
                }
                else
                {
                    ConvertObjCToPath(Filtered, "", outstrFileNames);
                }
            }
            
            [Contents release];
            [pPool release];
            
            std::sort(outstrFileNames.begin(), outstrFileNames.end());
            std::vector<std::string>::iterator it = std::unique(outstrFileNames.begin(), outstrFileNames.end());
            outstrFileNames.resize(it - outstrFileNames.begin()); 
        }
        //--------------------------------------------------------------
        //--------------------------------------------------------------
        void FileSystem::GetPathForFilesWithNameInDirectory(Core::StorageLocation ineStorageLocation, const std::string& instrDirectory,  bool inbRecurseIntoSubDirectories,
                                                        const std::string& instrName, std::vector<std::string> &outstrFileNames, bool inbAppendFullPath) const
        {
            //Check that this storage location is available
            if (IsStorageLocationAvailable(ineStorageLocation) == false)
            {
                CS_LOG_ERROR("Requested Storage Location is not available!");
                return;
            }
            
            std::vector<std::string> astrDirectoriesToCheck;
            GetPathsForStorageLocation(ineStorageLocation, instrDirectory, astrDirectoriesToCheck);
            
            NSAutoreleasePool* pPool = [[NSAutoreleasePool alloc] init];
            NSMutableArray* Contents = [[NSMutableArray alloc] init];
            
            GetDirectoryContents(astrDirectoriesToCheck, inbRecurseIntoSubDirectories, Contents);
            
            if([Contents count] > 0)
            {
                NSArray* Filtered = FilterFileNamesByName(Contents, instrName);
                
                if(inbAppendFullPath)
                {
                    ConvertObjCToPath(Filtered, instrDirectory, outstrFileNames);
                }
                else
                {
                    ConvertObjCToPath(Filtered, "", outstrFileNames);
                }
            }
            
            [Contents release];
            [pPool release];
            
            std::sort(outstrFileNames.begin(), outstrFileNames.end());
            std::vector<std::string>::iterator it = std::unique(outstrFileNames.begin(), outstrFileNames.end());
            outstrFileNames.resize(it - outstrFileNames.begin()); 
        }
        //--------------------------------------------------------------
        //--------------------------------------------------------------
        void FileSystem::GetFileNamesInDirectory(Core::StorageLocation ineStorageLocation, const std::string& instrDirectory, bool inbRecurseIntoSubDirectories, 
                                     std::vector<std::string> &outstrFileNames, bool inbAppendFullPath) const
        {
            //Check that this storage location is available
            if (IsStorageLocationAvailable(ineStorageLocation) == false)
            {
                CS_LOG_ERROR("Requested Storage Location is not available!");
                return;
            }
            
            std::vector<std::string> astrDirectoriesToCheck;
            GetPathsForStorageLocation(ineStorageLocation, instrDirectory, astrDirectoriesToCheck);
            
            NSAutoreleasePool* pPool = [[NSAutoreleasePool alloc] init];
            NSMutableArray* Contents = [[NSMutableArray alloc] init];
            
            GetDirectoryContents(astrDirectoriesToCheck, inbRecurseIntoSubDirectories, Contents);
            
            if([Contents count] > 0)
            {
                NSArray* Filtered = FilterFileNamesByFile(Contents);
                
                if(inbAppendFullPath)
                {
                    ConvertObjCToPath(Filtered, instrDirectory, outstrFileNames);
                }
                else
                {
                    ConvertObjCToPath(Filtered, "", outstrFileNames);
                }
            }
            
            [Contents release];
            [pPool release];
            
            std::sort(outstrFileNames.begin(), outstrFileNames.end());
            std::vector<std::string>::iterator it = std::unique(outstrFileNames.begin(), outstrFileNames.end());
            outstrFileNames.resize(it - outstrFileNames.begin()); 
        }
        //--------------------------------------------------------------
        //--------------------------------------------------------------
        void FileSystem::GetDirectoriesInDirectory(Core::StorageLocation ineStorageLocation, const std::string& instrDirectory, bool inbRecurseIntoSubDirectories,
                                                    std::vector<std::string> &outstrDirectories, bool inbAppendFullPath) const
        {
            //Check that this storage location is available
            if (IsStorageLocationAvailable(ineStorageLocation) == false)
            {
                CS_LOG_ERROR("Requested Storage Location is not available!");
                return;
            }
            
            std::vector<std::string> astrDirectoriesToCheck;
            GetPathsForStorageLocation(ineStorageLocation, instrDirectory, astrDirectoriesToCheck);
            
            NSAutoreleasePool* pPool = [[NSAutoreleasePool alloc] init];
            NSMutableArray* Contents = [[NSMutableArray alloc] init];
            
            GetDirectoryContents(astrDirectoriesToCheck, inbRecurseIntoSubDirectories, Contents);
            
            if([Contents count] > 0)
            {
                NSArray* Filtered = FilterFileNamesByFile(Contents);
                
                if(inbAppendFullPath)
                {
                    ConvertObjCToPath(Filtered, instrDirectory, outstrDirectories);
                }
                else
                {
                    ConvertObjCToPath(Filtered, "", outstrDirectories);
                }
            }
            
            [Contents release];
            [pPool release];
            
            std::sort(outstrDirectories.begin(), outstrDirectories.end());
            std::vector<std::string>::iterator it = std::unique(outstrDirectories.begin(), outstrDirectories.end());
            outstrDirectories.resize(it - outstrDirectories.begin()); 
        }
        //--------------------------------------------------------------
        //--------------------------------------------------------------
        void FileSystem::GetPathsForStorageLocation(Core::StorageLocation ineStorageLocation, const std::string& instrFileName, std::vector<std::string>& outaPaths) const
        {
            switch(ineStorageLocation)
            {
                case Core::StorageLocation::k_package:
                    for(u32 i=0; i<3; ++i)
                    {
                        outaPaths.push_back(GetStorageLocationDirectory(ineStorageLocation) + mastrResourceDirectory[i] + instrFileName);
                    }
                    break;
                case Core::StorageLocation::k_DLC:
                    for(u32 i=0; i<3; ++i)
                    {
                        outaPaths.push_back(GetStorageLocationDirectory(Core::StorageLocation::k_package) + mastrResourceDirectory[i] + mstrPackageDLCPath + instrFileName);
                    }
                    outaPaths.push_back(GetStorageLocationDirectory(Core::StorageLocation::k_DLC) + instrFileName);
                    break;
                default:
                    outaPaths.push_back(GetStorageLocationDirectory(ineStorageLocation) + instrFileName);
                    break;
            }
        }
        //--------------------------------------------------------------
        //--------------------------------------------------------------
        bool FileSystem::DoesFileExist(Core::StorageLocation ineStorageLocation, const std::string& instrFilepath) const
        {
            //Check that this storage location is available
            if (IsStorageLocationAvailable(ineStorageLocation) == false)
            {
                CS_LOG_ERROR("Requested Storage Location is not available!");
                return false;
            }
            
            if(ineStorageLocation == Core::StorageLocation::k_package)
            {
                for(u32 i=0; i<3; ++i)
                {
                    if(DoesFileExistInHashedStore(mastrResourceDirectory[i] + instrFilepath))
                    {
                        return true;
                    }
                }
                
                return false;
            }
            
            //get the filepath
            std::string path = GetStorageLocationDirectory(ineStorageLocation) + instrFilepath;
            
            //if its a DLC stream, make sure that it exists in the DLC cache, if not fall back on the package
            if (ineStorageLocation == Core::StorageLocation::k_DLC)
            {
                if (DoesItemExistInDLCCache(instrFilepath, false) == true)
                {
                    return true;
                }
                
                return DoesFileExist(Core::StorageLocation::k_package, mstrPackageDLCPath + instrFilepath);
            }
            
            //return whether or not the file exists
			return DoesFileExist(ChilliSource::Core::StringUtils::StandardisePath(path));
        }
        //--------------------------------------------------------------
        //--------------------------------------------------------------
        bool FileSystem::DoesFileExistInCachedDLC(const std::string& instrFilepath) const
        {
            return DoesItemExistInDLCCache(instrFilepath, false);
        }
        //--------------------------------------------------------------
        //--------------------------------------------------------------
        bool FileSystem::DoesFileExistInPackageDLC(const std::string& instrFilepath) const
        {
            return DoesFileExist(ChilliSource::Core::StringUtils::StandardisePath(GetStorageLocationDirectory(Core::StorageLocation::k_package) + mstrPackageDLCPath + instrFilepath));
        }
        //--------------------------------------------------------------
        //--------------------------------------------------------------
        bool FileSystem::DoesDirectoryExist(Core::StorageLocation ineStorageLocation, const std::string& instrDirectory) const
        {
            //Check that this storage location is available
            if (IsStorageLocationAvailable(ineStorageLocation) == false)
            {
                CS_LOG_ERROR("Requested Storage Location is not available!");
                return false;
            }
            
            if(ineStorageLocation == Core::StorageLocation::k_package)
            {
                for(u32 i=0; i<3; ++i)
                {
                    if(DoesFolderExist(mstrBundlePath + mastrResourceDirectory[i] + instrDirectory))
                    {
                        return true;
                    }
                }
                
                return false;
            }
            
            //get the filepath
            std::string path = GetStorageLocationDirectory(ineStorageLocation) + instrDirectory;
            
            //if its a DLC stream, make sure that it exists in the DLC cache, if not fall back on the package
            if (ineStorageLocation == Core::StorageLocation::k_DLC)
            {
                if (DoesItemExistInDLCCache(instrDirectory, true) == true)
                {
                    return true;
                }
                
                return DoesDirectoryExist(Core::StorageLocation::k_package, mstrPackageDLCPath + instrDirectory);
            }
            
            //return whether or not the dir exists
			return DoesFolderExist(ChilliSource::Core::StringUtils::StandardisePath(path));
        }
        //--------------------------------------------------------------
        //--------------------------------------------------------------
        bool FileSystem::IsStorageLocationAvailable(Core::StorageLocation ineStorageLocation) const
        {
            switch (ineStorageLocation) 
            {
                case Core::StorageLocation::k_package:
                case Core::StorageLocation::k_saveData:
                case Core::StorageLocation::k_cache:
                case Core::StorageLocation::k_DLC:
                    return true;
                default:
                    return false;
            }
        }
        //--------------------------------------------------------------
        //--------------------------------------------------------------
        std::string FileSystem::GetStorageLocationDirectory(Core::StorageLocation ineStorageLocation) const
        {
            //get the storage location path
            std::string strStorageLocationPath;
            switch (ineStorageLocation) 
            {
                case Core::StorageLocation::k_package:
                    strStorageLocationPath = mstrBundlePath;
                    break;
                case Core::StorageLocation::k_saveData:
                    strStorageLocationPath = mstrDocumentsPath + kstrSaveDataPath;
                    break;
                case Core::StorageLocation::k_cache:
                    strStorageLocationPath = mstrLibraryPath + kstrCachePath;
                    break;
                case Core::StorageLocation::k_DLC:
                    strStorageLocationPath = mstrLibraryPath + kstrDLCPath;
                    break;
                default:
                    CS_LOG_ERROR("Storage Location not available on this platform!");
                    break;
            }
            
            return strStorageLocationPath;
        }
        //--------------------------------------------------------------
        //--------------------------------------------------------------
        bool FileSystem::DoesItemExistInDLCCache(const std::string& instrPath, bool inbFolder) const
        {
            //Check that this storage location is available
            if (IsStorageLocationAvailable(Core::StorageLocation::k_DLC) == false)
            {
                CS_LOG_ERROR("Requested Storage Location is not available!");
                return false;
            }
            
            //return whether or not the file exists
            if(inbFolder)
            {
                return DoesFolderExist(ChilliSource::Core::StringUtils::StandardisePath(GetStorageLocationDirectory(Core::StorageLocation::k_DLC) + instrPath));
            }
            else
            {
                return DoesFileExist(ChilliSource::Core::StringUtils::StandardisePath(GetStorageLocationDirectory(Core::StorageLocation::k_DLC) + instrPath));
            }
        }
        //--------------------------------------------------------------
        //--------------------------------------------------------------
        std::string FileSystem::GetDirectoryForDLCFile(const std::string& instrFilePath) const
        {
            std::string strResult;
            std::string strPath = ChilliSource::Core::StringUtils::StandardisePath(GetStorageLocationDirectory(Core::StorageLocation::k_DLC) + instrFilePath);
            
            if(DoesFileExist(strPath))
            {
                strResult = strPath;
            }
            else
            {
                strResult = GetDirectoryForPackageFile(mstrPackageDLCPath + instrFilePath);
            }
            
            return strResult;
        }
        //--------------------------------------------------------------
        //--------------------------------------------------------------
        std::string FileSystem::GetDirectoryForPackageFile(const std::string& instrFilePath) const
        {
            std::string strResult = GetStorageLocationDirectory(Core::StorageLocation::k_package) + instrFilePath;
            
            for(u32 i=0; i<3; ++i)
            {
                std::string strPath = ChilliSource::Core::StringUtils::StandardisePath(mastrResourceDirectory[i] + instrFilePath);
                if(DoesFileExistInHashedStore(strPath))
                {
                    strResult = GetStorageLocationDirectory(Core::StorageLocation::k_package) + strPath;
                    break;
                }
            }
            
            return strResult;
        }
        //--------------------------------------------------------------
        //--------------------------------------------------------------
        bool FileSystem::DoesFileExistInHashedStore(const std::string& instrPath) const
        {
            u32 udwHashedFile = Core::HashCRC32::GenerateHashCode(instrPath);
            
            std::vector<u32>::const_iterator it = std::lower_bound(mHashedPackageFileNames.begin(), mHashedPackageFileNames.end(), udwHashedFile);
            
            if(it!= mHashedPackageFileNames.end() && *it == udwHashedFile)
                return true;

            return false;
        }
        //--------------------------------------------------------------
        //--------------------------------------------------------------
        bool FileSystem::DoesFileExist(const std::string& instrPath) const
        {
            BOOL bDirectory = NO;
            bool bExists = false;
            NSFileManager *fileManager= [NSFileManager defaultManager];
            NSAutoreleasePool* pPool = [[NSAutoreleasePool alloc] init];
            
            if ([fileManager fileExistsAtPath:[NSString stringWithUTF8String:instrPath.c_str()] isDirectory:&bDirectory])
            {
                bExists = true;
            }
            
            [pPool release];
            return (bExists && !bDirectory);
        }
        //--------------------------------------------------------------
        //--------------------------------------------------------------
        bool FileSystem::DoesFolderExist(const std::string& instrPath) const
        {
            BOOL bDirectory = NO;
            bool bExists = false;
            NSFileManager *fileManager= [NSFileManager defaultManager];
            NSAutoreleasePool* pPool = [[NSAutoreleasePool alloc] init];
            
            if ([fileManager fileExistsAtPath:[NSString stringWithUTF8String:instrPath.c_str()] isDirectory:&bDirectory])
            {
                bExists = true;
            }
            
            [pPool release];
            return (bExists && bDirectory);
        }
	}
}
