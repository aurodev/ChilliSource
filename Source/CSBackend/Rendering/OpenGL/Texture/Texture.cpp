//
//  Texture.cpp
//  Chilli Source
//  Created by Scott Downie on 01/10/2010.
//
//  The MIT License (MIT)
//
//  Copyright (c) 2010 Tag Games Limited
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

#include <CSBackend/Rendering/OpenGL/Texture/Texture.h>

#include <CSBackend/Rendering/OpenGL/Base/GLError.h>
#include <CSBackend/Rendering/OpenGL/Base/RenderCapabilities.h>
#include <CSBackend/Rendering/OpenGL/Texture/TextureUnitSystem.h>
#include <ChilliSource/Core/Base/Application.h>
#include <ChilliSource/Core/Image/ImageFormat.h>
#include <ChilliSource/Core/Image/ImageCompression.h>
#include <ChilliSource/Core/Image/ImageFormatConverter.h>
#include <ChilliSource/Rendering/Texture/TextureFilterMode.h>
#include <ChilliSource/Rendering/Texture/TextureWrapMode.h>

namespace CSBackend
{
	namespace OpenGL
	{
        namespace
        {
            //---------------------------------------------------
            /// Apply the currently set wrap mode to the texture.
            /// Requires that the texture is bound.
            ///
            /// @author S Downie
            ///
            /// @param Horizontal wrap mode
            /// @param Vertical wrap mode
            //---------------------------------------------------
            void ApplyWrapMode(ChilliSource::TextureWrapMode in_smode, ChilliSource::TextureWrapMode in_tmode)
            {
                switch(in_smode)
                {
                    case ChilliSource::TextureWrapMode::k_clamp:
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                        break;
                    case ChilliSource::TextureWrapMode::k_repeat:
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                        break;
                };
                switch(in_tmode)
                {
                    case ChilliSource::TextureWrapMode::k_clamp:
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                        break;
                    case ChilliSource::TextureWrapMode::k_repeat:
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                        break;
                };
                
                CS_ASSERT_NOGLERROR("An OpenGL error occurred while applying texture wrap mode.");
            }
            //---------------------------------------------------
            /// Apply the currently set filter mode to the texture.
            /// Requires that the texture is bound.
            ///
            /// @author S Downie
            ///
            /// @param Filter mode
            /// @param Whether mip-mapping is enabled
            //---------------------------------------------------
            void ApplyFilterMode(ChilliSource::TextureFilterMode in_mode, bool in_hasMipMaps)
            {
                if(in_hasMipMaps == false)
                {
                    switch(in_mode)
                    {
                        case ChilliSource::TextureFilterMode::k_nearest:
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                            break;
                        case ChilliSource::TextureFilterMode::k_bilinear:
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                            break;
                    }
                }
                else
                {
                    switch(in_mode)
                    {
                        case ChilliSource::TextureFilterMode::k_nearest:
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                            break;
                        case ChilliSource::TextureFilterMode::k_bilinear:
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                            break;
                    }
                }
                
                CS_ASSERT_NOGLERROR("An OpenGL error occurred while applying texture filter mode.");
            }
            //---------------------------------------------------
            /// Uploads image data with no compression in the
            /// given format
            ///
            /// @author S Downie
            ///
            /// @param Format
            /// @param Image width
            /// @param Image height
            /// @param Image data
            //---------------------------------------------------
            void UploadImageDataNoCompression(ChilliSource::ImageFormat in_format, u32 in_imageWidth, u32 in_imageHeight, const u8* in_imageData)
            {
                switch(in_format)
                {
                    default:
                    case ChilliSource::ImageFormat::k_RGBA8888:
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, in_imageWidth, in_imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, in_imageData);
                        break;
                    case ChilliSource::ImageFormat::k_RGB888:
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, in_imageWidth, in_imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, in_imageData);
                        break;
                    case ChilliSource::ImageFormat::k_RGBA4444:
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, in_imageWidth, in_imageHeight, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, in_imageData);
                        break;
                    case ChilliSource::ImageFormat::k_RGB565:
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, in_imageWidth, in_imageHeight, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, in_imageData);
                        break;
                    case ChilliSource::ImageFormat::k_LumA88:
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, in_imageWidth, in_imageHeight, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, in_imageData);
                        break;
                    case ChilliSource::ImageFormat::k_Lum8:
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, in_imageWidth, in_imageHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, in_imageData);
                        break;
                    case ChilliSource::ImageFormat::k_Depth16:
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, in_imageWidth, in_imageHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, in_imageData);
                        break;
                    case ChilliSource::ImageFormat::k_Depth32:
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, in_imageWidth, in_imageHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, in_imageData);
                        break;
                };
                
                CS_ASSERT_NOGLERROR("An OpenGL error occurred while uploading uncompressed texture data.");
            }
            //---------------------------------------------------
            /// Uploads image data with ETC1 compression. ETC1
            /// only supports RGB images and will aseert on any
            /// other format. It is currently only supported on
            /// Android and will assert on other platforms
            ///
            /// @author S Downie
            ///
            /// @param Format
            /// @param Image width
            /// @param Image height
            /// @param Image data
            /// @param Image data size in bytes
            //---------------------------------------------------
            void UploadImageDataETC1(ChilliSource::ImageFormat in_format, u32 in_imageWidth, u32 in_imageHeight, const u8* in_imageData, u32 in_imageDataSize)
            {
#ifndef CS_TARGETPLATFORM_ANDROID
                CS_LOG_FATAL("ETC1 compression is only supported on Android");
#endif
                CS_ASSERT(in_format == ChilliSource::ImageFormat::k_RGB888, "ETC1 only supports RGB image format");
                
#ifdef CS_TARGETPLATFORM_ANDROID
                glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_ETC1_RGB8_OES, in_imageWidth, in_imageHeight, 0, in_imageDataSize, in_imageData);
#endif
                CS_ASSERT_NOGLERROR("An OpenGL error occurred while uploading ETC1 texture data.");
            }
            //---------------------------------------------------
            /// Uploads image data with PVR 2 bits per pixel compression.
            /// PVR only supports RGB and RGBA images and will aseert on any
            /// other format. It is currently only supported on
            /// iOS and will assert on other platforms
            ///
            /// @author S Downie
            ///
            /// @param Format
            /// @param Image width
            /// @param Image height
            /// @param Image data
            /// @param Image data size in bytes
            //---------------------------------------------------
            void UploadImageDataPVR2(ChilliSource::ImageFormat in_format, u32 in_imageWidth, u32 in_imageHeight, const u8* in_imageData, u32 in_imageDataSize)
            {
#ifndef CS_TARGETPLATFORM_IOS
                CS_LOG_FATAL("PVR compression is only supported on iOS");
#endif
                
#ifdef CS_TARGETPLATFORM_IOS
                switch(in_format)
                {
                    default:
                        CS_LOG_FATAL("PVR compression only supports RGB and RGBA image formats");
                        break;
                    case ChilliSource::ImageFormat::k_RGBA8888:
                        glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG, in_imageWidth, in_imageHeight, 0, in_imageDataSize, in_imageData);
                        break;
                    case ChilliSource::ImageFormat::k_RGB888:
                        glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG, in_imageWidth, in_imageHeight, 0, in_imageDataSize, in_imageData);
                        break;
                };
#endif
                CS_ASSERT_NOGLERROR("An OpenGL error occurred while uploading PVR2 texture data.");
            }
            //---------------------------------------------------
            /// Uploads image data with PVR 4 bits per pixel compression.
            /// PVR only supports RGB and RGBA images and will aseert on any
            /// other format. It is currently only supported on
            /// iOS and will assert on other platforms
            ///
            /// @author S Downie
            ///
            /// @param Format
            /// @param Image width
            /// @param Image height
            /// @param Image data
            /// @param Image data size in bytes
            //---------------------------------------------------
            void UploadImageDataPVR4(ChilliSource::ImageFormat in_format, u32 in_imageWidth, u32 in_imageHeight, const u8* in_imageData, u32 in_imageDataSize)
            {
#ifndef CS_TARGETPLATFORM_IOS
                CS_LOG_FATAL("PVR compression is only supported on iOS");
#endif
                
#ifdef CS_TARGETPLATFORM_IOS
                switch(in_format)
                {
                    default:
                        CS_LOG_FATAL("PVR compression only supports RGB and RGBA image formats");
                        break;
                    case ChilliSource::ImageFormat::k_RGBA8888:
                        glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG, in_imageWidth, in_imageHeight, 0, in_imageDataSize, in_imageData);
                        break;
                    case ChilliSource::ImageFormat::k_RGB888:
                        glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG, in_imageWidth, in_imageHeight, 0, in_imageDataSize, in_imageData);
                        break;
                };
#endif
                CS_ASSERT_NOGLERROR("An OpenGL error occurred while uploading PVR4 texture data.");
            }
        }
        
        CS_DEFINE_NAMEDTYPE(Texture);
		//---------------------------------------------------------------
		//---------------------------------------------------------------
		Texture::Texture()
        : m_format(ChilliSource::ImageFormat::k_RGBA8888)
		{
            m_renderCapabilities = ChilliSource::Application::Get()->GetSystem<ChilliSource::RenderCapabilities>();
            CS_ASSERT(m_renderCapabilities, "Cannot find required system: Render Capabilities.");
            
            m_texUnitSystem = ChilliSource::Application::Get()->GetSystem<TextureUnitSystem>();
            CS_ASSERT(m_texUnitSystem, "Cannot find required system: Texture Unit System.");
		}
        //--------------------------------------------------
		//--------------------------------------------------
		bool Texture::IsA(ChilliSource::InterfaceIDType in_interfaceId) const
		{
			return in_interfaceId == Texture::InterfaceID || in_interfaceId == ChilliSource::Texture::InterfaceID;
		}
		//--------------------------------------------------
		//--------------------------------------------------
		void Texture::Bind(u32 in_texUnit)
		{
			CS_ASSERT(in_texUnit < m_renderCapabilities->GetNumTextureUnits(), "Attempting to bind to texture unit not supported on this device: " + ChilliSource::ToString(in_texUnit));
			
            m_texUnitSystem->Bind(this, in_texUnit);
            
            if(m_hasFilterModeChanged == true)
            {
                ApplyFilterMode(m_filterMode, m_hasMipMaps);
                m_hasFilterModeChanged = false;
            }
            
            if(m_hasWrapModeChanged == true)
            {
                ApplyWrapMode(m_sWrapMode, m_tWrapMode);
                m_hasWrapModeChanged = false;
            }
		}
        //--------------------------------------------------
        /// GL makes a copy of the data so we can just
        /// let the incoming data delete itself
        //--------------------------------------------------
        void Texture::Build(const Descriptor& in_desc, TextureDataUPtr in_data, bool in_mipMap, bool in_restoreTextureDataEnabled)
        {
            //TODO: Re-add building of textures
            
//            Destroy();
//            
//            m_width = in_desc.m_width;
//            m_height = in_desc.m_height;
//            m_format = in_desc.m_format;
//            m_compression = in_desc.m_compression;
//            
//            CS_ASSERT(m_width <= m_renderCapabilities->GetMaxTextureSize() && m_height <= m_renderCapabilities->GetMaxTextureSize(),
//                      "OpenGL does not support textures of this size on this device (" + ChilliSource::ToString(m_width) + ", " + ChilliSource::ToString(m_height) + ")");
//            
//            glGenTextures(1, &m_texHandle);
//            Bind();
//            
//            u8* data = in_data.get();
//            
//			switch(m_compression)
//			{
//				case ChilliSource::ImageCompression::k_none:
//                    UploadImageDataNoCompression(m_format, m_width, m_height, data);
//					break;
//				case ChilliSource::ImageCompression::k_ETC1:
//                    UploadImageDataETC1(m_format, m_width, m_height, data, in_desc.m_dataSize);
//					break;
//				case ChilliSource::ImageCompression::k_PVR2Bpp:
//                    UploadImageDataPVR2(m_format, m_width, m_height, data, in_desc.m_dataSize);
//					break;
//				case ChilliSource::ImageCompression::k_PVR4Bpp:
//                    UploadImageDataPVR4(m_format, m_width, m_height, data, in_desc.m_dataSize);
//					break;
//			};
//            
//            if(in_mipMap == true)
//            {
//                glGenerateMipmap(GL_TEXTURE_2D);
//            }
//            
//            m_hasMipMaps = in_mipMap;
//            
//#ifdef CS_TARGETPLATFORM_ANDROID
//            if (GetStorageLocation() == ChilliSource::StorageLocation::k_none && in_restoreTextureDataEnabled == true)
//            {
//            	m_restoreTextureDataEnabled = true;
//                m_restorationDataSize = in_desc.m_dataSize;
//                m_restorationData = std::move(in_data);
//            }
//#endif
//            
//            CS_ASSERT_NOGLERROR("An OpenGL error occurred while building texture.");
        }
        //--------------------------------------------------
		//--------------------------------------------------
		void Texture::Unbind()
		{
            m_texUnitSystem->Unbind(this);
		}
		//--------------------------------------------------
		//--------------------------------------------------
		GLuint Texture::GetTextureHandle() const
		{
			return m_texHandle;
		}
        //--------------------------------------------------
		//--------------------------------------------------
		bool Texture::HasMipMaps() const
		{
			return m_hasMipMaps;
		}
		//--------------------------------------------------
		//--------------------------------------------------
		ChilliSource::ImageFormat Texture::GetFormat() const
		{
			return m_format;
		}
        //--------------------------------------------------------------
        //--------------------------------------------------------------
        u32 Texture::GetWidth() const
        {
            return m_width;
        }
        //--------------------------------------------------------------
        //--------------------------------------------------------------
        u32 Texture::GetHeight() const
        {
            return m_height;
        }
		//--------------------------------------------------
		//--------------------------------------------------
		void Texture::SetFilterMode(TextureFilterMode in_mode)
		{
            m_filterMode = in_mode;

            m_hasFilterModeChanged = true;
		}
		//--------------------------------------------------
		//--------------------------------------------------
		void Texture::SetWrapMode(TextureWrapMode in_smode, TextureWrapMode in_tmode)
		{
			m_sWrapMode = in_smode;
			m_tWrapMode = in_tmode;
            
            m_hasWrapModeChanged = true;
		}
#ifdef CS_TARGETPLATFORM_ANDROID
        //--------------------------------------------------
        //--------------------------------------------------
        void Texture::UpdateRestorationData()
        {
            CS_ASSERT(GetStorageLocation() == ChilliSource::StorageLocation::k_none, "Cannot update restoration data on texture that was loaded from file.");
            
            if (m_restoreTextureDataEnabled == true)
            {
            	Unbind();

            	//create an bind a new frame buffer.
            	GLuint frameBufferHandle = 0;
                glGenFramebuffers(1, &frameBufferHandle);
				glBindFramebuffer(GL_FRAMEBUFFER, frameBufferHandle);

				//attach the texture
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texHandle, 0);
				GLuint glCheck = glCheckFramebufferStatus(GL_FRAMEBUFFER);
				if(glCheck != GL_FRAMEBUFFER_COMPLETE)
				{
					CS_LOG_FATAL("Framebuffer incomplete while updating texture restoration data.");
				}

				//read the data from the texture. This can only be read back in RGBA8888 format so it will need
				//to be converted back to the format of the texture.
				u32 unconvertedDataSize = GetWidth() * GetHeight() * 4;
				std::unique_ptr<u8[]> unconvertedData(new u8[unconvertedDataSize]);
				glReadPixels(0, 0, GetWidth(), GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, unconvertedData.get());

				//Convert to the format of this texture
				ChilliSource::ImageFormatConverter::ImageBuffer convertedData;
				switch (m_format)
				{
				case ChilliSource::ImageFormat::k_RGBA8888:
					convertedData.m_size = unconvertedDataSize;
					convertedData.m_data = std::move(unconvertedData);
					break;
				case ChilliSource::ImageFormat::k_RGB888:
					convertedData = ChilliSource::ImageFormatConverter::RGBA8888ToRGB888(unconvertedData.get(), unconvertedDataSize);
					unconvertedData.reset();
					break;
				case ChilliSource::ImageFormat::k_RGBA4444:
					convertedData = ChilliSource::ImageFormatConverter::RGBA8888ToRGBA4444(unconvertedData.get(), unconvertedDataSize);
					unconvertedData.reset();
					break;
				case ChilliSource::ImageFormat::k_RGB565:
					convertedData = ChilliSource::ImageFormatConverter::RGBA8888ToRGB565(unconvertedData.get(), unconvertedDataSize);
					unconvertedData.reset();
					break;
				default:
					CS_LOG_FATAL("Texture is not in a restorable format. The restorable texture data option must be disabled for this texture.");
					break;
				}

				m_restorationDataSize = convertedData.m_size;
				m_restorationData = std::move(convertedData.m_data);

				//clean up the frame buffer.
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glDeleteFramebuffers(1, &frameBufferHandle);

				CS_ASSERT_NOGLERROR("An OpenGL error occurred while updating texture restoration data.");
            }
        }
        //--------------------------------------------------
        //--------------------------------------------------
        void Texture::Restore()
        {
            CS_ASSERT(GetStorageLocation() == ChilliSource::StorageLocation::k_none, "Cannot restore texture that was loaded from file. This should be handled using RefreshResource().");

            Texture::Descriptor desc;
            desc.m_width = m_width;
            desc.m_height = m_height;
            desc.m_format = m_format;
            desc.m_compression = m_compression;
            desc.m_dataSize = m_restorationDataSize;
            
            WrapMode sWrap = m_sWrapMode;
            WrapMode tWrap = m_tWrapMode;
            FilterMode filterMode = m_filterMode;
            
            Build(desc, std::move(m_restorationData), m_hasMipMaps, m_restoreTextureDataEnabled);
            SetWrapMode(sWrap, tWrap);
            SetFilterMode(filterMode);
        }
#endif
        //--------------------------------------------------
        //--------------------------------------------------
        void Texture::Destroy()
        {
            //TODO: Re-implement in new system
//            m_width = 0;
//            m_height = 0;
//            
//            m_hasFilterModeChanged = true;
//            m_hasWrapModeChanged = true;
//            m_hasMipMaps = false;
//            
//            m_filterMode = FilterMode::k_bilinear;
//            m_sWrapMode = WrapMode::k_clamp;
//            m_tWrapMode = WrapMode::k_clamp;
//            
//            //If the context has already been destroyed then the cubemap has already been destroyed
//            bool hasContext = static_cast<RenderSystem*>(ChilliSource::Application::Get()->GetRenderSystem())->HasContext();
//            if(hasContext == true && m_texHandle > 0)
//            {
//                Unbind();
//                glDeleteTextures(1, &m_texHandle);
//            }
//            
//            m_texHandle = 0;
//            
//#ifdef CS_TARGETPLATFORM_ANDROID
//            m_restoreTextureDataEnabled = false;
//            m_restorationDataSize = 0;
//            m_restorationData.reset();
//#endif
//            
//            CS_ASSERT_NOGLERROR("An OpenGL error occurred while destroying texture.");
        }
		//--------------------------------------------------
		//--------------------------------------------------
		Texture::~Texture()
		{
            //TODO: Re-add texture cleanup
            //Destroy();
		}
	}
}
