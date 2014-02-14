/** 
* File: ImageLoader.h
* Date: 1 Oct 2010
* Description: iOS 3_X image resource provider
*/

/** 
*
* Author Stuart / Ian Copland
* Version 1.0 - moFlo
* Copyright ©2010 Tag Games Limited - All rights reserved 
*/

#ifndef _MOFLO_PLATFORM_WINDOWS_IMAGELOADER_H_
#define _MOFLO_PLATFORM_WINDOWS_IMAGELOADER_H_

#include <ChilliSource/Core/Image/ImageResourceProvider.h>
#include <ChilliSource/Core/Image/Image.h>

namespace moFlo
{
	namespace WindowsPlatform
	{
		class ImageLoader : public IImageResourceProvider
		{
		public:
			
			virtual ~ImageLoader(){}
			//----------------------------------------------------------------
			/// Is A
			///
			/// Is the object of the given interface type
			/// @param Interface type to query
			/// @return Whether the object is of given type
			//----------------------------------------------------------------
			bool IsA(Core::InterfaceIDType inInterfaceID) const;
			//----------------------------------------------------------------
			/// Can Create Resource Of Kind
			///
			/// @param Resource to compare against
			/// @return Whether the object can load a resource of given type
			//----------------------------------------------------------------
			bool CanCreateResourceOfKind(Core::InterfaceIDType inInterfaceID) const;
			//----------------------------------------------------------------
			/// Can Create Resource From File With Extension
			///
			/// @param Extension to compare against
			/// @return Whether the object can load a resource with that extension
			//----------------------------------------------------------------
			bool CanCreateResourceFromFileWithExtension(const std::string & inExtension) const;
			//----------------------------------------------------------------
			/// Create Resource From File
			///
			/// @param Storage Location
			/// @param File path to resource
			/// @param Out: Resource
			/// @return Success
			//----------------------------------------------------------------
			bool CreateResourceFromFile(Core::STORAGE_LOCATION ineLocation, const std::string & inFilePath, Core::ResourcePtr& outpResource);
			//----------------------------------------------------------------
			/// Create Image From File
			///
			/// @param Storage location to load from
			/// @param File path to resource
			/// @param Image format
			/// @param Out: Resource
			/// @return Sucess
			//----------------------------------------------------------------
			bool CreateImageFromFile(Core::STORAGE_LOCATION ineLocation, const std::string & inFilePath, Core::CImage::Format ineFormat, Core::ResourcePtr& outpResource);

		private:
			//----------------------------------------------------------------
			/// Create UI Image From File
			///
			/// Create a UI image using 
			///
			/// @param Storage 
			/// @param File path to resource
			/// @param Whether the asset is high resolution
			/// @param Image format
			/// @param Out: Image data
			/// @return Success
			//----------------------------------------------------------------
			bool CreatePNGImageFromFile(Core::STORAGE_LOCATION ineLocation, const std::string & inFilePath, Core::CImage::Format ineFormat, Core::CImage* outpImage);
			//----------------------------------------------------------------
			/// RGBA8888 To RGB565
			///
			/// Downsize the image from 32 bits to 16 bits no alpha
			///
			/// @param RGBA8888 data
			/// @param Width x Height
			/// @return RGB565 data
			//-----------------------------------------------------------------
			u8* RGBA8888ToRGB565(u8* inpData, u32 inudwArea);
			//----------------------------------------------------------------
			/// RGBA8888 To RGBA4444
			///
			/// Downsize the image from 32 bits to 16 bits
			///
			/// @param RGBA8888 data
			/// @param Width x Height
			/// @return RGBA4444 data
			//-----------------------------------------------------------------
			u8* RGBA8888ToRGBA4444(u8* inpData, u32 inudwArea);
			//----------------------------------------------------------------
			/// RGBA8888 To LUM88
			///
			/// Downsize the image from 32 bits to 16 bits
			///
			/// @param RGBA8888 data
			/// @param Width x Height
			/// @return LUM88 data
			//-----------------------------------------------------------------
			u8* RGBA8888ToLUM88(u8* inpData, u32 inudwArea);
		};
	}
}



#endif
