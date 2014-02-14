/*
 *  FacebookPostSystem
 *  moFlow
 *
 *  Created by Stuart McGaw on 06/06/2011.
 *  Copyright 2011 Tag Games. All rights reserved.
 *
 */

#ifndef _MOFLO_SOCIAL_FACEBOOK_FACEBOOKPOSTSYSTEM_H_
#define _MOFLO_SOCIAL_FACEBOOK_FACEBOOKPOSTSYSTEM_H_

#include <ChilliSource/Core/Main/GenericEvent.h>
#include <ChilliSource/Core/System/System.h>
#include <ChilliSource/Core/JSON/json.h>

#include <ChilliSource/Social/Facebook/FacebookAuthentication.h>

namespace moFlo
{
	namespace Social
	{
		struct FacebookPostDesc
		{
			std::string strTo; //ID of the page this post is for (Friend ID perhaps), if blank will default to the user's page
			
			std::string strName; //Name in bold of this linky thing
			std::string strURL; //This must be set for your post to show
			
			std::string strCaption; //Subline under name
			std::string strDescription; //Body of the post
            
			std::string strPictureURL; //URL of an image to accompany this. If this isn't set Facebook will scrape the URL page to find a picture.
			
			void ToJSON(Json::Value& outsRoot) const;
		};
		
		class IFacebookPostSystem : public moFlo::Core::ISystem
		{
		public:
			
			DECLARE_NAMED_INTERFACE(IFacebookPostSystem);
			
			enum PostResult
			{
				PR_SUCCESS,
				PR_CANCELLED,
				PR_FAILED
			};
			
			typedef fastdelegate::FastDelegate1<const PostResult&> PostResultDelegate;
            
            static IFacebookPostSystem* CreatePostSystem(moFlo::Social::IFacebookAuthenticationSystem* inpAuthSystem);
            
			virtual void TryPost(const FacebookPostDesc& insDesc, const PostResultDelegate& insResultCallback) = 0;
            
            virtual void TrySendRequest(const Social::FacebookPostDesc& insDesc, const PostResultDelegate& insResultCallback, std::vector<std::string>& inastrRecommendedFriends) = 0;
			
			PostResultDelegate mCompletionDelegate;
            
            PostResultDelegate mRequestCompleteDelegate;
		};
	}
}

#endif
