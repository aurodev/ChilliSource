/*
 *  FileStream.h
 *  iOSTemplate
 *
 *  Created by Ian Copland on 25/03/2011.
 *  Copyright 2011 Tag Games Ltd. All rights reserved.
 *
 */

#ifndef _MOFLO_PLATFORM_ANDROID_FILEIO_FILE_STREAM_ANDROID_APK_
#define _MOFLO_PLATFORM_ANDROID_FILEIO_FILE_STREAM_ANDROID_APK_

#include <ChilliSource/Core/File/FileStream.h>
#include <fstream>
#include <ios>
#include <ChilliSource/Core/Minizip/unzip.h>
#include <sstream>
#include <ChilliSource/Core/Threading/Thread.h>

namespace moFlo
{
	namespace AndroidPlatform
	{
		//======================================================================================================
		/// CFileStreamAPK
		///
		/// A filestream is used for all reading and writing of files. This will emulate the functionality
		/// of fstream and allows for cross platform filereading.
		//======================================================================================================
		class CFileStreamAPK : public Core::IFileStream
		{
		public:
			//--------------------------------------------------------------------------------------------------
			/// Destructor
			//--------------------------------------------------------------------------------------------------
			virtual ~CFileStreamAPK();
			//--------------------------------------------------------------------------------------------------
			/// Is Open
			///
			/// @return whether or not the stream is open
			//--------------------------------------------------------------------------------------------------
			bool IsOpen();
			//--------------------------------------------------------------------------------------------------
			/// Is Bad
			///
			/// @return Checks that the file stream is not corrupt. this calls both fstream::fail() and
			///			fstream::bad()
			//--------------------------------------------------------------------------------------------------
			bool IsBad();
			//--------------------------------------------------------------------------------------------------
			/// End Of File
			///
			/// @return Checks whether the end of the file has been reached.
			//--------------------------------------------------------------------------------------------------
			bool EndOfFile();
			//--------------------------------------------------------------------------------------------------
			/// Close
			///
			/// Closes the filestream.
			//--------------------------------------------------------------------------------------------------
			void Close();
			//--------------------------------------------------------------------------------------------------
			/// Get
			///
			/// @return the next character in the file stream as an integer
			//--------------------------------------------------------------------------------------------------
			s32 Get();
			//--------------------------------------------------------------------------------------------------
			/// Get
			///
			/// Reads the next character into outbyChar
			///
			/// @param Container for the output value.
			//--------------------------------------------------------------------------------------------------
			void Get(s8 & outbyChar);
			//--------------------------------------------------------------------------------------------------
			/// Get
			///
			/// Reads indwStreamSize bytes from the stream unless an EOF is encountered.
			///
			/// @param buffer for the output string.
			/// @param The amount of bytes to read into the buffer.
			//--------------------------------------------------------------------------------------------------
			void Get(s8 * outpbyString, s32 indwStreamSize);
			//--------------------------------------------------------------------------------------------------
			/// Get
			///
			/// Reads indwStreamSize bytes from the stream unless the character specifed by inbyDelim is 
			/// encountered
			///
			/// @param buffer for the output string.
			/// @param The amount of bytes to read into the buffer.
			/// @param The delimiting character.
			//--------------------------------------------------------------------------------------------------
			void Get(s8 * outpbyString, s32 indwStreamSize, s8 inbyDelim);
			//--------------------------------------------------------------------------------------------------
			/// Get Line
			///
			/// Reads a single line into a std string.
			///
			/// @param OUT: output string.
			//--------------------------------------------------------------------------------------------------
			void GetLine(std::string &outstrString);
			//--------------------------------------------------------------------------------------------------
			/// Get All
			///
			/// Reads the entire file into a std string.
			///
			/// @param OUT: output string.
			//--------------------------------------------------------------------------------------------------
			void GetAll(std::string &outstrString);
			//--------------------------------------------------------------------------------------------------
			/// Get
			///
			/// Reads into a std string stream.
			///
			/// @param OUT: output string.
			//--------------------------------------------------------------------------------------------------
			void Get(std::stringstream &outstrStringStream);
			//--------------------------------------------------------------------------------------------------
			/// Get Line
			///
			/// Reads indwStreamSize bytes from the stream unless a return character is encountered.
			///
			/// @param buffer for the output string.
			/// @param The amount of bytes to read into the buffer.
			//--------------------------------------------------------------------------------------------------
			void GetLine(s8 * outpbyString, s32 indwStreamSize);
			//--------------------------------------------------------------------------------------------------
			/// Get Line
			///
			/// Reads indwStreamSize bytes from the stream unless the character specifed by inbyDelim is 
			/// encountered
			///
			/// @param buffer for the output string.
			/// @param The amount of bytes to read into the buffer.
			/// @param The delimiting character.
			//--------------------------------------------------------------------------------------------------
			void GetLine(s8 * outpbyString, s32 indwStreamSize, s8 inbyDelim);
			//--------------------------------------------------------------------------------------------------
			/// Ignore
			///
			/// Reads indwStreamSize bytes from the stream unless the character specifed by inbyDelim is 
			/// encountered and the discards them.
			///
			/// @param The amount of bytes to read into the buffer.
			/// @param The delimiting character.
			//--------------------------------------------------------------------------------------------------
			void Ignore(s32 indwStreamSize = 1, s8 inbyDelim = EOF);
			//--------------------------------------------------------------------------------------------------
			/// Peek
			///
			/// Reads the next byte in the stream and returns it as an integer, but doesn't seek to the next
			/// byte, leaving it remaining as the next byte.
			///
			/// @return the next byte as an integer.
			//--------------------------------------------------------------------------------------------------
			s32 Peek();
			//--------------------------------------------------------------------------------------------------
			/// Read
			///
			/// Reads a block of data into the buffer on size specified by indwStreamSize
			///
			/// @param The buffer.
			/// @param the stream size.
			//--------------------------------------------------------------------------------------------------
			void Read(s8* inpbyBuffer, s32 indwStreamSize);
			//--------------------------------------------------------------------------------------------------
			/// Read Some
			///
			/// Very similar to Read(), but this will stop if there are no more characters in the steam, even
			/// without hitting EOF.
			///
			/// @param the buffer.
			/// @param the stream size.
			//--------------------------------------------------------------------------------------------------
			s32 ReadSome(s8* inpbyBuffer, s32 indwStreamSize);
			//--------------------------------------------------------------------------------------------------
			/// Put Back
			///
			/// Decriments the get pointer by one, and sets inbyChar as the next char to be read.
			///
			/// @param the next char.
			//--------------------------------------------------------------------------------------------------
			void PutBack(s8 inbyChar);
			//--------------------------------------------------------------------------------------------------
			/// Unget
			///
			/// Decriments the get pointer by one.
			//--------------------------------------------------------------------------------------------------
			void Unget();
			//--------------------------------------------------------------------------------------------------
			/// TellG
			///
			/// @return the absolute position of the get pointer.
			//--------------------------------------------------------------------------------------------------
			s32 TellG();
			//--------------------------------------------------------------------------------------------------
			/// SeekG
			///
			/// Sets the position of the get pointer
			///
			/// @param the new position.
			//--------------------------------------------------------------------------------------------------
			void SeekG(s32 indwPosition);
			//--------------------------------------------------------------------------------------------------
			/// SeekG
			///
			/// Sets the position of the get pointer, using the specified SEEK_DIR
			///
			/// @param the new position.
			/// @param the direction from which to seek.
			//--------------------------------------------------------------------------------------------------
			void SeekG(s32 indwPosition, Core::SEEK_DIR ineDir);
			//--------------------------------------------------------------------------------------------------
			/// Sync
			///
			/// Syncronises with the associated buffer. This effectively means that unread bytes are discarded.
			//--------------------------------------------------------------------------------------------------
			s32 Sync();
			//--------------------------------------------------------------------------------------------------
			/// Put
			///
			/// Writes the specified character to the output.
			///
			/// @param the char to write.
			//--------------------------------------------------------------------------------------------------
			void Put(s8 inbyChar);
			//--------------------------------------------------------------------------------------------------
			/// Write
			///
			/// Writes a block of data to the output.
			///
			/// @param the data buffer.
			/// @param the stream size.
			//--------------------------------------------------------------------------------------------------
			void Write(s8* inpbyChar, s32 indwStreamSize);
			//--------------------------------------------------------------------------------------------------
			/// Write
			///
			/// Writes using a std:string
			///
			/// @param the string.
			//--------------------------------------------------------------------------------------------------
			void Write(const std::string& _instrString);
			//--------------------------------------------------------------------------------------------------
			/// TellP
			///
			/// @return the currently position of the put pointer
			//--------------------------------------------------------------------------------------------------
			s32 TellP();
			//--------------------------------------------------------------------------------------------------
			/// SeekP
			///
			/// Sets the position of the put pointer
			///
			/// @param the new position.
			//--------------------------------------------------------------------------------------------------
			void SeekP(s32 indwPosition);
			//--------------------------------------------------------------------------------------------------
			/// SeekP
			///
			/// Sets the position of the put pointer, using the specified SEEK_DIR
			///
			/// @param the new position.
			/// @param the direction from which to seek.
			//--------------------------------------------------------------------------------------------------
			void SeekP(s32 indwPosition, Core::SEEK_DIR ineDir);
			//--------------------------------------------------------------------------------------------------
			/// Flush
			///
			/// Synchronises the associated buffer with the stream.
			//--------------------------------------------------------------------------------------------------
			void Flush();
		protected:
			//--------------------------------------------------------------------------------------------------
			/// Constructor
			///
			/// This is defined protected so that only the FileSystem can create it.
			//--------------------------------------------------------------------------------------------------
			CFileStreamAPK(CThread::Mutex* inpMinizipMutex);
			//--------------------------------------------------------------------------------------------------
			/// OpenFromAPK
			///
			/// Opens the apk using minizip and opens a filestream.
			///
			/// @param The path to the zip file
			/// @param The zip file position.
			/// @param File mode
			//--------------------------------------------------------------------------------------------------
			void OpenFromAPK(const std::string& instrApkPath, const unz_file_pos& inFilePos, Core::FILE_MODE ineMode);
			//--------------------------------------------------------------------------------------------------
			/// Open
			///
			/// Opens the filestream with the specified file mode.
			///
			/// @param The zip file position.
			/// @param The file mode with which the file should be opened.
			//--------------------------------------------------------------------------------------------------
			void Open(const unz_file_pos& inFilePos, Core::FILE_MODE ineMode);
			//--------------------------------------------------------------------------------------------------
			/// Get File Mode
			///
			/// Converts the FILE_MODE enum into a ios_base::openmode for opening the file.
			///
			/// @return the STL openmode.
			//--------------------------------------------------------------------------------------------------
			std::ios_base::openmode GetFileMode();
			
			friend class CFileSystem;
		private:
			CThread::Mutex* mpMinizipMutex;
			unzFile mUnzipper;
			bool mbError;
			bool mbOpen;

			s8 * mpDataBuffer;
			Core::FILE_MODE meFileMode;
			std::stringstream mStringStream;
		};
	}
}


#endif
