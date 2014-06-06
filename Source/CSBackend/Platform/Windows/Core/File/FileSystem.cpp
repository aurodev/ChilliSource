//
//  FileSystem.cpp
//  Chilli Source
//
//  Created by I Copland on 25/03/2011.
//  Copyright 2011 Tag Games Ltd. All rights reserved.
//

#ifdef CS_TARGETPLATFORM_WINDOWS

#include <CSBackend/Platform/Windows/Core/File/FileSystem.h>

#include <CSBackend/Platform/Windows/Core/String/WindowsStringUtils.h>
#include <ChilliSource/Core/Base/Utils.h>
#include <ChilliSource/Core/File/FileStream.h>
#include <ChilliSource/Core/String/StringUtils.h>

#include <iostream>
#include <shlobj.h>
#include <stack>

//This includes windows so needs to come last, else it might cause problems with other includes
#include <CSBackend/Platform/Windows/Core/File/WindowsFileUtils.h>

//Undefine the windows file system functions that share names with ours.
#undef CopyFile
#undef DeleteFile

namespace ChilliSource
{
	namespace Windows 
	{
		namespace
		{
			const std::string k_saveDataPath = "SaveData/";
			const std::string k_cachePath = "Cache/";
			const std::string k_dlcPath = "DLC/";

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
				default:
					return false;

				}
			}
			//--------------------------------------------------------------
			/// @author I Copland
			///
			/// @param The file path.
			///
			/// @return Whether or not the given file path exists.
			//--------------------------------------------------------------
			bool DoesFileExist(const std::string& in_filePath)
			{
				std::wstring filePath = WindowsStringUtils::ConvertStandardPathToWindows(in_filePath);
				DWORD attributes = GetFileAttributes(filePath.c_str());
				return (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY));
			}
			//--------------------------------------------------------------
			/// @author I Copland
			///
			/// @param The directory path.
			///
			/// @return Whether or not the given directory path exists.
			//--------------------------------------------------------------
			bool DoesDirectoryExist(const std::string& in_directoryPath)
			{
				std::wstring directoryPath = WindowsStringUtils::ConvertStandardPathToWindows(in_directoryPath);
				DWORD attributes = GetFileAttributes(directoryPath.c_str());
				return (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY));
			}
			//--------------------------------------------------------------
			/// @author I Copland
			///
			/// @param The directory path.
			///
			/// @return Whether or not the given directory path exists.
			//--------------------------------------------------------------
			bool DeleteDirectory(const std::string& in_directoryPath)
			{
				std::wstring directoryQuery = WindowsStringUtils::ConvertStandardPathToWindows(in_directoryPath) + L"\\*";

				WIN32_FIND_DATA fileData;
				HANDLE fileHandle = WindowsFileUtils::WindowsFindFirstFile(directoryQuery.c_str(), &fileData);
				if (fileHandle == INVALID_HANDLE_VALUE || GetLastError() == ERROR_FILE_NOT_FOUND)
				{
					return false;
				}

				do
				{
					if (wcscmp(fileData.cFileName, L".") != 0 && wcscmp(fileData.cFileName, L"..") != 0)
					{
						if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
						{
							std::string directoryName = WindowsStringUtils::UTF16ToUTF8(fileData.cFileName);
							
							if (DeleteDirectory(Core::StringUtils::StandardisePath(in_directoryPath + directoryName)) == false)
							{
								return false;
							}
						}
						else
						{
							std::string fileName = WindowsStringUtils::UTF16ToUTF8(fileData.cFileName);
							std::string filePath = Core::StringUtils::StandardisePath(in_directoryPath + fileName);
							if (WindowsFileUtils::WindowsDeleteFile(WindowsStringUtils::ConvertStandardPathToWindows(filePath).c_str()) == FALSE)
							{
								return false;
							}
						}
					}
				} while (WindowsFileUtils::WindowsFindNextFile(fileHandle, &fileData) == TRUE);

				bool success = (GetLastError() == ERROR_NO_MORE_FILES);
				FindClose(fileHandle);

				if (success == false)
				{
					return false;
				}

				if (WindowsFileUtils::WindowsRemoveDirectory(WindowsStringUtils::ConvertStandardPathToWindows(in_directoryPath).c_str()) == FALSE)
				{
					return false;
				}

				return true;
			}
			//--------------------------------------------------------------
			/// Lists all files and sub-directories inside the given directory.
			/// All paths will be relative to the given directory.
			///
			/// @author I Copland
			///
			/// @param The directory.
			/// @param Whether or not to recurse into sub directories.
			/// @param [Out] The sub directories.
			/// @param [Out] The files.
			/// @param [Optional] The relative directory path. This is used
			/// in recursion and shouldn't be set outside of this function.
			/// @return Whether or not this succeeded.
			//--------------------------------------------------------------
			bool ListDirectoryContents(const std::string& in_directoryPath, bool in_recursive, std::vector<std::string>& out_directoryPaths, std::vector<std::string>& out_filePaths, 
				const std::string& in_relativeDirectoryPath = "")
			{
				std::wstring directoryQuery = WindowsStringUtils::ConvertStandardPathToWindows(in_directoryPath) + L"\\*";

				WIN32_FIND_DATA fileData;
				HANDLE fileHandle = WindowsFileUtils::WindowsFindFirstFile(directoryQuery.c_str(), &fileData);
				if (fileHandle == INVALID_HANDLE_VALUE || GetLastError() == ERROR_FILE_NOT_FOUND)
				{
					return false;
				}

				do
				{
					if (wcscmp(fileData.cFileName, L".") != 0 && wcscmp(fileData.cFileName, L"..") != 0)
					{
						if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
						{
							std::string directoryName = WindowsStringUtils::UTF16ToUTF8(fileData.cFileName);
							std::string relativeDirectoryPath = Core::StringUtils::StandardisePath(in_relativeDirectoryPath + directoryName);
							out_directoryPaths.push_back(relativeDirectoryPath);

							if (in_recursive == true)
							{
								std::string absoluteDirectoryPath = Core::StringUtils::StandardisePath(in_directoryPath + directoryName);
								if (ListDirectoryContents(absoluteDirectoryPath, true, out_directoryPaths, out_filePaths, relativeDirectoryPath) == false)
								{
									return false;
								}
							}
						}
						else
						{
							std::string fileName = WindowsStringUtils::UTF16ToUTF8(fileData.cFileName);
							std::string relativeFilePath = Core::StringUtils::StandardisePath(in_relativeDirectoryPath + fileName);
							out_filePaths.push_back(relativeFilePath);
						}
					}
				} 
				while (WindowsFileUtils::WindowsFindNextFile(fileHandle, &fileData) == TRUE);

				bool success = (GetLastError() == ERROR_NO_MORE_FILES);
				FindClose(fileHandle);

				return success;
			}
		}
		CS_DEFINE_NAMEDTYPE(FileSystem);
		//--------------------------------------------------------------
		//--------------------------------------------------------------
		FileSystem::FileSystem()
		{
			wchar_t pathChars[MAX_PATH];
			GetModuleFileName(nullptr, pathChars, MAX_PATH);
			std::string path = WindowsStringUtils::ConvertWindowsPathToStandard(std::wstring(pathChars));
			std::string::size_type pos = path.find_last_of("/");
			std::string strWorkingDir = Core::StringUtils::StandardisePath(path.substr(0, pos));

			m_packagePath = strWorkingDir + "assets/";
			m_documentsPath = strWorkingDir + "Documents/";

			CreateDirectoryPath(Core::StorageLocation::k_saveData, "");
			CreateDirectoryPath(Core::StorageLocation::k_cache, "");
			CreateDirectoryPath(Core::StorageLocation::k_DLC, "");
		}
		//----------------------------------------------------------
		//----------------------------------------------------------
		bool FileSystem::IsA(Core::InterfaceIDType in_interfaceId) const
		{
			return (Core::FileSystem::InterfaceID == in_interfaceId || FileSystem::InterfaceID == in_interfaceId);
		}
		//--------------------------------------------------------------
		//--------------------------------------------------------------
		Core::FileStreamUPtr FileSystem::CreateFileStream(Core::StorageLocation in_storageLocation, const std::string& in_filePath, Core::FileMode in_fileMode) const
		{
			Core::FileStreamUPtr fileStream = Core::FileStreamUPtr(new Core::FileStream());

			if (IsWriteMode(in_fileMode) == true)
			{
				CS_ASSERT(IsStorageLocationWritable(in_storageLocation), "File System: Trying to write to read only storage location.");

				std::string filePath = GetAbsolutePathToStorageLocation(in_storageLocation) + in_filePath;
				fileStream->Open(filePath, in_fileMode);
			}
			else
			{
				std::string filePath = GetAbsolutePathToFile(in_storageLocation, in_filePath);
				fileStream->Open(filePath, in_fileMode);
			}

			return fileStream;
		}
		//--------------------------------------------------------------
		//--------------------------------------------------------------
		bool FileSystem::CreateDirectoryPath(Core::StorageLocation in_storageLocation, const std::string& in_directoryPath) const
		{
			CS_ASSERT(IsStorageLocationWritable(in_storageLocation), "File System: Trying to write to read only storage location.");

			std::string directoryPath = GetAbsolutePathToStorageLocation(in_storageLocation) + in_directoryPath;
			if (Windows::DoesDirectoryExist(directoryPath) == false)
			{
				if (WindowsFileUtils::WindowsCreateDirectory(WindowsStringUtils::ConvertStandardPathToWindows(directoryPath).c_str(), NULL) == FALSE)
				{
					CS_LOG_ERROR("File System: Failed to create directory '" + in_directoryPath + "'");
					return false;
				}
			}

			return true;
		}
		//--------------------------------------------------------------
		//--------------------------------------------------------------
		bool FileSystem::CopyFile(Core::StorageLocation in_sourceStorageLocation, const std::string& in_sourceFilePath, 
			Core::StorageLocation in_destinationStorageLocation, const std::string& in_destinationFilePath) const
		{
			CS_ASSERT(IsStorageLocationWritable(in_destinationStorageLocation), "File System: Trying to write to read only storage location.");
            
			std::string sourceFilePath = GetAbsolutePathToFile(in_sourceStorageLocation, in_sourceFilePath);
			if (sourceFilePath.empty() == true)
            {
				CS_LOG_ERROR("File System: Trying to copy file '" + in_sourceFilePath + "' but it does not exist.");
                return false;
            }

            //get the path to the file
			std::string destinationFileName, destinationDirectoryPath;
			ChilliSource::Core::StringUtils::SplitFilename(in_destinationFilePath, destinationFileName, destinationDirectoryPath);
            
            //create the output directory
			CreateDirectoryPath(in_destinationStorageLocation, destinationDirectoryPath);
            
            //try and copy the files
			std::wstring sourceWPath = WindowsStringUtils::ConvertStandardPathToWindows(sourceFilePath);
			std::wstring destWPath = WindowsStringUtils::ConvertStandardPathToWindows(GetAbsolutePathToStorageLocation(in_destinationStorageLocation) + in_destinationFilePath);
			if (WindowsFileUtils::WindowsCopyFile(sourceWPath.c_str(), destWPath.c_str(), FALSE) == FALSE)
			{
				CS_LOG_ERROR("File System: Failed to copy file '" + in_sourceFilePath + "'");
				return false;
			}

			return true;
		}
		//--------------------------------------------------------------
		//--------------------------------------------------------------
		bool FileSystem::CopyDirectory(Core::StorageLocation in_sourceStorageLocation, const std::string& in_sourceDirectoryPath, 
			Core::StorageLocation in_destinationStorageLocation, const std::string& in_destinationDirectoryPath) const
		{
			CS_ASSERT(IsStorageLocationWritable(in_destinationStorageLocation), "File System: Trying to write to read only storage location.");

			if (DoesDirectoryExist(in_sourceStorageLocation, in_sourceDirectoryPath) == false)
			{
				CS_LOG_ERROR("File System: Trying to copy directory '" + in_sourceDirectoryPath + "' but it doesn't exist.");
				return false;
			}

			std::vector<std::string> filePaths = GetFilePaths(in_sourceStorageLocation, in_sourceDirectoryPath, true);

			//if the source directory is empty, just create the equivelent directory in the destination
			if (filePaths.size() == 0)
			{
				CreateDirectoryPath(in_destinationStorageLocation, in_destinationDirectoryPath);
			}
			else
			{
				std::string sourceDirectoryPath = Core::StringUtils::StandardisePath(in_sourceDirectoryPath);
				std::string destinationDirectoryPath = Core::StringUtils::StandardisePath(in_destinationDirectoryPath);
				for (const std::string& filePath : filePaths)
				{
					if (CopyFile(in_sourceStorageLocation, sourceDirectoryPath + filePath, in_destinationStorageLocation, destinationDirectoryPath + filePath) == false)
					{
						CS_LOG_ERROR("File System: Failed to copy directory '" + in_sourceDirectoryPath + "'");
						return false;
					}
				}
			}

			return true;
		}
		//--------------------------------------------------------------
		//--------------------------------------------------------------
		bool FileSystem::DeleteFile(Core::StorageLocation in_storageLocation, const std::string& in_filePath) const
		{
			CS_ASSERT(IsStorageLocationWritable(in_storageLocation), "File System: Trying to delete from a read only storage location.");

			std::wstring filePath = WindowsStringUtils::ConvertStandardPathToWindows(GetAbsolutePathToStorageLocation(in_storageLocation) + in_filePath);
			if (WindowsFileUtils::WindowsDeleteFile(filePath.c_str()) == FALSE)
			{
				CS_LOG_ERROR("File System: Failed to delete file '" + in_filePath + "'");
				return false;
			}

			return true;
		}
		//--------------------------------------------------------------
		//--------------------------------------------------------------
		bool FileSystem::DeleteDirectory(Core::StorageLocation in_storageLocation, const std::string& in_directoryPath) const
		{
			CS_ASSERT(IsStorageLocationWritable(in_storageLocation), "File System: Trying to delete from a read only storage location.");

			std::string directoryPath = GetAbsolutePathToDirectory(in_storageLocation, in_directoryPath);
			if (directoryPath != "")
			{
				if (Windows::DeleteDirectory(directoryPath) == false)
				{
					CS_LOG_ERROR("File System: Failed to delete directory '" + in_directoryPath + "'");
					return false;
				}
				return true;
			}

			return false;
		}
		//--------------------------------------------------------------
		//--------------------------------------------------------------
		std::vector<std::string> FileSystem::GetFilePaths(Core::StorageLocation in_storageLocation, const std::string& in_directoryPath, bool in_recursive) const
		{
			std::vector<std::string> possibleDirectories = GetPossibleAbsoluteDirectoryPaths(in_storageLocation, in_directoryPath);
            
			std::vector<std::string> output;
			std::vector<std::string> filePaths;
			std::vector<std::string> directoryPaths;
			for (const std::string& possibleDirectory : possibleDirectories)
			{
				filePaths.clear();
				directoryPaths.clear();

				ListDirectoryContents(possibleDirectory, in_recursive, directoryPaths, filePaths);
				output.insert(output.end(), filePaths.begin(), filePaths.end());
			}

			std::sort(output.begin(), output.end());
			std::vector<std::string>::iterator it = std::unique(output.begin(), output.end());
			output.resize(it - output.begin());
			return output;
		}
		//--------------------------------------------------------------
		//--------------------------------------------------------------
		std::vector<std::string> FileSystem::GetDirectoryPaths(Core::StorageLocation in_storageLocation, const std::string& in_directoryPath, bool in_recursive) const
		{
			std::vector<std::string> possibleDirectories = GetPossibleAbsoluteDirectoryPaths(in_storageLocation, in_directoryPath);

			std::vector<std::string> output;
			std::vector<std::string> filePaths;
			std::vector<std::string> directoryPaths;
			for (const std::string& possibleDirectory : possibleDirectories)
			{
				filePaths.clear();
				directoryPaths.clear();

				ListDirectoryContents(possibleDirectory, in_recursive, directoryPaths, filePaths);
				output.insert(output.end(), directoryPaths.begin(), directoryPaths.end());
			}

			std::sort(output.begin(), output.end());
			std::vector<std::string>::iterator it = std::unique(output.begin(), output.end());
			output.resize(it - output.begin());
			return output;
		}
		//--------------------------------------------------------------
		//--------------------------------------------------------------
		bool FileSystem::DoesFileExist(Core::StorageLocation in_storageLocation, const std::string& in_filePath) const
		{
			switch (in_storageLocation)
			{
				case Core::StorageLocation::k_package:
				{
					const std::string* resourceDirectories = GetResourceDirectories();
					for (u32 i = 0; i < 3; ++i)
					{
						if (Windows::DoesFileExist(GetAbsolutePathToStorageLocation(Core::StorageLocation::k_package) + resourceDirectories[i] + in_filePath) == true)
						{
							return true;
						}
					}
					return false;
				}
				case Core::StorageLocation::k_DLC:
				{
					if (DoesItemExistInDLCCache(in_filePath, false) == true)
					{
						return true;
					}

					return DoesFileExist(Core::StorageLocation::k_package, GetPackageDLCPath() + in_filePath);
				}
				default:
				{
					std::string path = Core::StringUtils::StandardisePath(GetAbsolutePathToStorageLocation(in_storageLocation) + in_filePath);
					return Windows::DoesFileExist(path);
				}
			}
		}
		//--------------------------------------------------------------
		//--------------------------------------------------------------
		bool FileSystem::DoesDirectoryExist(Core::StorageLocation in_storageLocation, const std::string& in_directoryPath) const
		{
			switch (in_storageLocation)
			{
				case Core::StorageLocation::k_package:
				{
					const std::string* resourceDirectories = GetResourceDirectories();
					for (u32 i = 0; i < 3; ++i)
					{
						if (Windows::DoesDirectoryExist(GetAbsolutePathToStorageLocation(Core::StorageLocation::k_package) + resourceDirectories[i] + in_directoryPath) == true)
						{
							return true;
						}
					}
					return false;
				}
				case Core::StorageLocation::k_DLC:
				{
					if (DoesItemExistInDLCCache(in_directoryPath, true) == true)
					{
						return true;
					}

					return DoesDirectoryExist(Core::StorageLocation::k_package, GetPackageDLCPath() + in_directoryPath);
				}
				default:
				{
					std::string path = Core::StringUtils::StandardisePath(GetAbsolutePathToStorageLocation(in_storageLocation) + in_directoryPath);
					return Windows::DoesDirectoryExist(path);
				}
			}
		}
		//--------------------------------------------------------------
		//--------------------------------------------------------------
		bool FileSystem::DoesFileExistInCachedDLC(const std::string& in_filePath) const
		{
			return DoesItemExistInDLCCache(in_filePath, false);
		}
		//--------------------------------------------------------------
		//--------------------------------------------------------------
		bool FileSystem::DoesFileExistInPackageDLC(const std::string& in_filePath) const
		{
			return DoesFileExist(Core::StorageLocation::k_package, GetPackageDLCPath() + in_filePath);
		}
		//--------------------------------------------------------------
		//--------------------------------------------------------------
		std::string FileSystem::GetAbsolutePathToStorageLocation(Core::StorageLocation in_storageLocation) const
		{
			switch (in_storageLocation)
			{
			case Core::StorageLocation::k_package:
				return m_packagePath;
			case Core::StorageLocation::k_saveData:
				return m_documentsPath + k_saveDataPath;
			case Core::StorageLocation::k_cache:
				return m_documentsPath + k_cachePath;
			case Core::StorageLocation::k_DLC:
				return m_documentsPath + k_dlcPath;
			case Core::StorageLocation::k_root:
				return "";
				break;
			default:
				CS_LOG_ERROR("Storage Location not available on this platform!");
				return "";
			}
		}
		//--------------------------------------------------------------
		//--------------------------------------------------------------
		std::string FileSystem::GetAbsolutePathToFile(Core::StorageLocation in_storageLocation, const std::string& in_filePath) const
		{
			if (DoesFileExist(in_storageLocation, in_filePath) == true)
			{
				switch (in_storageLocation)
				{
					case Core::StorageLocation::k_package:
					{
						const std::string* resourceDirectories = GetResourceDirectories();
						std::string absoluteFilePath;
						for (u32 i = 0; i < 3; ++i)
						{
							absoluteFilePath = Core::StringUtils::StandardisePath(GetAbsolutePathToStorageLocation(Core::StorageLocation::k_package) + resourceDirectories[i] + in_filePath);
							if (Windows::DoesFileExist(absoluteFilePath) == true)
							{
								break;
							}
						}

						return absoluteFilePath;
					}
					case Core::StorageLocation::k_DLC:
					{
						std::string filePath = Core::StringUtils::StandardisePath(GetAbsolutePathToStorageLocation(Core::StorageLocation::k_DLC) + in_filePath);
						if (Windows::DoesFileExist(filePath) == true)
						{
							return filePath;
						}

						return GetAbsolutePathToFile(Core::StorageLocation::k_package, GetPackageDLCPath() + in_filePath);
					}
					default:
					{
						return GetAbsolutePathToStorageLocation(in_storageLocation) + in_filePath;
					}
				}
			}

			return "";
		}
		//--------------------------------------------------------------
		//--------------------------------------------------------------
		std::string FileSystem::GetAbsolutePathToDirectory(Core::StorageLocation in_storageLocation, const std::string& in_directoryPath) const
		{
			if (DoesDirectoryExist(in_storageLocation, in_directoryPath) == true)
			{
				switch (in_storageLocation)
				{
					case Core::StorageLocation::k_package:
					{
						const std::string* resourceDirectories = GetResourceDirectories();
						std::string absoluteDirectoryPath;
						for (u32 i = 0; i < 3; ++i)
						{
							absoluteDirectoryPath = Core::StringUtils::StandardisePath(GetAbsolutePathToStorageLocation(Core::StorageLocation::k_package) + resourceDirectories[i] + in_directoryPath);
							if (Windows::DoesDirectoryExist(absoluteDirectoryPath) == true)
							{
								break;
							}
						}

						return absoluteDirectoryPath;
					}
					case Core::StorageLocation::k_DLC:
					{
						std::string filePath = Core::StringUtils::StandardisePath(GetAbsolutePathToStorageLocation(Core::StorageLocation::k_DLC) + in_directoryPath);
						if (Windows::DoesDirectoryExist(filePath) == true)
						{
							return filePath;
						}

						return GetAbsolutePathToDirectory(Core::StorageLocation::k_package, GetPackageDLCPath() + in_directoryPath);
					}
					default:
					{
						return GetAbsolutePathToStorageLocation(in_storageLocation) + in_directoryPath;
					}
				}
			}

			return "";
		}
		//--------------------------------------------------------------
		//--------------------------------------------------------------
		bool FileSystem::DoesItemExistInDLCCache(const std::string& in_path, bool in_isDirectory) const
		{
			std::string path = Core::StringUtils::StandardisePath(GetAbsolutePathToStorageLocation(Core::StorageLocation::k_DLC) + in_path);
			if (in_isDirectory == true)
			{
				return Windows::DoesDirectoryExist(path);
			}
			else
			{
				return Windows::DoesFileExist(path);
			}
		}
		//------------------------------------------------------------
		//------------------------------------------------------------
		std::vector<std::string> FileSystem::GetPossibleAbsoluteDirectoryPaths(Core::StorageLocation in_storageLocation, const std::string& in_path) const
		{
			std::vector<std::string> output;

			switch (in_storageLocation)
			{
				case Core::StorageLocation::k_package:
				{
					const std::string* resourceDirectories = GetResourceDirectories();
					for (u32 i = 0; i < 3; ++i)
					{
						output.push_back(GetAbsolutePathToStorageLocation(in_storageLocation) + resourceDirectories[i] + in_path);
					}
					break;
				}
				case Core::StorageLocation::k_DLC:
				{
					const std::string* resourceDirectories = GetResourceDirectories();
					for (u32 i = 0; i < 3; ++i)
					{
						output.push_back(GetAbsolutePathToStorageLocation(Core::StorageLocation::k_package) + resourceDirectories[i] + GetPackageDLCPath() + in_path);
					}
					output.push_back(GetAbsolutePathToStorageLocation(Core::StorageLocation::k_DLC) + in_path);
					break;
				}
				default:
				{
					output.push_back(GetAbsolutePathToStorageLocation(in_storageLocation) + in_path);
					break;
				}
			}

			return output;
		}
	}
}

#endif