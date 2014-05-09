//
//  VideoPlayer.cpp
//  Chilli Source
//  Created by Ian Copland on 10/08/2012.
//
//  The MIT License (MIT)
//
//  Copyright (c) 2012 Tag Games Limited
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

#include <ChilliSource/Backend/Platform/Android/Video/Base/VideoPlayer.h>

#include <ChilliSource/Backend/Platform/Android/Core/JNI/JavaInterfaceManager.h>
#include <ChilliSource/Backend/Platform/Android/Video/Base/VideoPlayerJavaInterface.h>
#include <ChilliSource/Core/Base/Application.h>
#include <ChilliSource/Core/Base/MakeDelegate.h>
#include <ChilliSource/Core/Base/Screen.h>
#include <ChilliSource/Core/Math/MathUtils.h>
#include <ChilliSource/Core/Localisation/LocalisedText.h>
#include <ChilliSource/Core/String/StringUtils.h>
#include <ChilliSource/Core/Threading/TaskScheduler.h>
#include <ChilliSource/Video/Base/Subtitles.h>

namespace ChilliSource
{
    namespace Android
    {
    	CS_DEFINE_NAMEDTYPE(VideoPlayer);
        //--------------------------------------------------------------
        //--------------------------------------------------------------
        VideoPlayer::VideoPlayer()
        	: m_currentSubtitleTimeMS(0), m_isPlaying(false)
        {
        }
		//--------------------------------------------------------------
		//--------------------------------------------------------------
		bool VideoPlayer::IsA(Core::InterfaceIDType in_interfaceId) const
		{
			return (in_interfaceId == Video::VideoPlayer::InterfaceID || in_interfaceId == VideoPlayer::InterfaceID);
		}
        //--------------------------------------------------------------
        //--------------------------------------------------------------
        void VideoPlayer::Present(Core::StorageLocation in_storageLocation, const std::string& in_fileName, const VideoCompleteDelegate& in_delegate, bool in_dismissWithTap,
        		const Core::Colour& in_backgroundColour)
        {
        	CS_ASSERT(m_isPlaying == false, "Cannot present a video while one is already playing.");
        	m_isPlaying = true;
        	m_completionDelegate = in_delegate;

        	//calculate the storage location and full filename.
        	bool isPackage = false;
        	std::string fileName;
        	if (in_storageLocation == Core::StorageLocation::k_package)
        	{
        		isPackage = true;
        		fileName = Core::Application::Get()->GetFileSystem()->GetAbsolutePathToFile(Core::StorageLocation::k_package, in_fileName);
        	}
        	else if (in_storageLocation == Core::StorageLocation::k_DLC)
			{
        		if (Core::Application::Get()->GetFileSystem()->DoesFileExistInCachedDLC(in_fileName) == true)
        		{
        			isPackage = false;
        			fileName = Core::Application::Get()->GetFileSystem()->GetAbsolutePathToStorageLocation(Core::StorageLocation::k_DLC) + in_fileName;
        		}
        		else
        		{
        			isPackage = true;
        			fileName = Core::Application::Get()->GetFileSystem()->GetAbsolutePathToFile(Core::StorageLocation::k_package, Core::Application::Get()->GetFileSystem()->GetPackageDLCPath() + in_fileName);
        		}
			}
        	else
        	{
        		isPackage = false;
        		fileName = Core::Application::Get()->GetFileSystem()->GetAbsolutePathToStorageLocation(in_storageLocation) + in_fileName;
        	}

        	//start the video
        	m_javaInterface->Present(isPackage, fileName, in_dismissWithTap, in_backgroundColour, Core::MakeDelegate(this, &VideoPlayer::OnVideoComplete));
        }
        //--------------------------------------------------------------
		//--------------------------------------------------------------
		void VideoPlayer::PresentWithSubtitles(Core::StorageLocation in_storageLocation, const std::string& in_fileName, const Video::SubtitlesCSPtr& in_subtitles, const VideoCompleteDelegate& in_delegate,
                bool in_dismissWithTap, const Core::Colour& in_backgroundColour)
		{
			m_subtitles = in_subtitles;
			m_javaInterface->SetUpdateSubtitlesDelegate(Core::MakeDelegate(this, &VideoPlayer::OnUpdateSubtitles));
			Present(in_storageLocation, in_fileName, in_delegate, in_dismissWithTap, in_backgroundColour);
		}
        //-------------------------------------------------------
        //-------------------------------------------------------
        void VideoPlayer::OnInit()
        {
        	//get the media player java interface or create it if it doesn't yet exist.
			m_javaInterface = JavaInterfaceManager::GetSingletonPtr()->GetJavaInterface<VideoPlayerJavaInterface>();
			if (m_javaInterface == nullptr)
			{
				m_javaInterface = VideoPlayerJavaInterfaceSPtr(new VideoPlayerJavaInterface());
				JavaInterfaceManager::GetSingletonPtr()->AddJavaInterface(m_javaInterface);
			}
        }
        //---------------------------------------------------------------
        //---------------------------------------------------------------
        void VideoPlayer::OnVideoComplete()
        {
        	m_javaInterface->SetUpdateSubtitlesDelegate(nullptr);
        	m_subtitles.reset();
        	m_isPlaying = false;

        	if (m_completionDelegate != nullptr)
        	{
        		m_completionDelegate();
        	}
        }
		//---------------------------------------------------------------
		//---------------------------------------------------------------
		void VideoPlayer::OnUpdateSubtitles()
		{
			//only update if the position in the video has changed.
			f32 position = m_javaInterface->GetTime();
			TimeIntervalMs currentTimeMS = (TimeIntervalMs)(position * 1000.0f);
			if (m_currentSubtitleTimeMS != currentTimeMS)
			{
				m_currentSubtitleTimeMS = currentTimeMS;

				//get the current subtitles
				auto subtitleArray = m_subtitles->GetSubtitlesAtTime(m_currentSubtitleTimeMS);
				auto localisedText = m_subtitles->GetLocalisedText().get();

				//add any new subtitles
				for (auto it = subtitleArray.begin(); it != subtitleArray.end(); ++it)
				{
					auto mapEntry = m_subtitleMap.find(*it);
					if (mapEntry == m_subtitleMap.end())
					{
						const Core::UTF8String& text = localisedText->GetText((*it)->m_localisedTextId);
						const Video::Subtitles::Style* style = m_subtitles->GetStyleWithName((*it)->m_styleName);
						s64 subtitleID = m_javaInterface->CreateSubtitle(text, style->m_fontName, style->m_fontSize, Rendering::StringFromAlignmentAnchor(style->m_alignment), style->m_bounds.vOrigin.x, style->m_bounds.vOrigin.y, style->m_bounds.vSize.x, style->m_bounds.vSize.y);
						m_javaInterface->SetSubtitleColour(subtitleID, 0.0f, 0.0f, 0.0f, 0.0f);
						m_subtitleMap.insert(std::make_pair(*it, subtitleID));
					}
				}

				//update the current text views
				for (auto it = m_subtitleMap.begin(); it != m_subtitleMap.end(); ++it)
				{
					UpdateSubtitle(it->first, it->second, m_currentSubtitleTimeMS);
				}

				//removes any text views that are no longer needed.
				for (auto it = m_subtitlesToRemove.begin(); it != m_subtitlesToRemove.end(); ++it)
				{
					auto mapEntry = m_subtitleMap.find(*it);
					if (mapEntry != m_subtitleMap.end())
					{
						m_javaInterface->RemoveSubtitle(mapEntry->second);
						m_subtitleMap.erase(mapEntry);
					}
				}
				m_subtitlesToRemove.clear();
			}
		}
		//---------------------------------------------------------------
		//---------------------------------------------------------------
		void VideoPlayer::UpdateSubtitle(const Video::Subtitles::Subtitle* in_subtitle, s64 in_subtitleID, TimeIntervalMs in_timeMS)
		{
			const Video::Subtitles::Style* style = m_subtitles->GetStyleWithName(in_subtitle->m_styleName);

			f32 fade = 0.0f;
			s64 relativeTime = ((s64)in_timeMS) - ((s64)in_subtitle->m_startTimeMS);
			s64 displayTime = ((s64)in_subtitle->m_endTimeMS) - ((s64)in_subtitle->m_startTimeMS);

			//subtitle should not be displayed yet so remove
			if (relativeTime < 0)
			{
				m_subtitlesToRemove.push_back(in_subtitle);
			}

			//fading in
			else if (relativeTime < style->m_fadeTimeMS)
			{
				fade = ((f32)relativeTime) / ((f32)style->m_fadeTimeMS);
			}

			//active
			else if (relativeTime < displayTime - style->m_fadeTimeMS)
			{
				fade = 1.0f;
			}

			//fading out
			else if (relativeTime < displayTime)
			{
				fade = 1.0f - (((f32)relativeTime - (displayTime - style->m_fadeTimeMS)) / ((f32)style->m_fadeTimeMS));
			}

			//should no longer be displayed so remove
			else if (relativeTime >= displayTime)
			{
				m_subtitlesToRemove.push_back(in_subtitle);
			}

			m_javaInterface->SetSubtitleColour(in_subtitleID, style->m_colour.r, style->m_colour.g, style->m_colour.b, fade * style->m_colour.a);
		}
        //-------------------------------------------------------
        //-------------------------------------------------------
        void VideoPlayer::OnDestroy()
        {
        	m_javaInterface.reset();
        }
    }
}