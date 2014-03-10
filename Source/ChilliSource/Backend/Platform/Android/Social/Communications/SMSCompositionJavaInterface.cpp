/*
 *  SMSCompositionJavaInterface.cpp
 *  moFlow
 *
 *  Created by Steven Hendrie on 20/01/2012.
 *  Copyright 2012 Tag Games. All rights reserved.
 *
 */

#include <ChilliSource/Backend/Platform/Android/Social/Communications/SMSCompositionJavaInterface.h>

#include <ChilliSource/Backend/Platform/Android/Social/Communications/SMSCompositionActivity.h>

//function definitions
extern "C"
{
	void Java_com_chillisource_social_SMSCompositionNativeInterface_OnSMSClosed(JNIEnv* inpEnv, jobject thiz);
}
//--------------------------------------------------------------------------------------
/// On Update Text
///
/// Interface function called from java. This is called when the keyboard text has been
/// updated.
///
/// @param The jni environment.
/// @param the java object calling the function
/// @param the updated keyboard text
/// @return whether or not the text was accepted.
//--------------------------------------------------------------------------------------
void Java_com_chillisource_social_SMSCompositionNativeInterface_OnSMSClosed(JNIEnv* inpEnv, jobject thiz)
{
	ChilliSource::Android::SMSCompositionActivity::OnSMSClosed();
}
namespace ChilliSource
{
	namespace Android
	{
		void SMSCompositionJavaInterface::SetupJavaInterface(JavaVM* inpJavaVM)
		{
			mspJavaVM = inpJavaVM;

			InitCallableStaticMethod("com/chillisource/social/SMSCompositionNativeInterface","PresentSMS", "([Ljava/lang/String;Ljava/lang/String;)V");
		}

		void SMSCompositionJavaInterface::Present(const std::vector<Core::UTF8String> & inastrRecipientNumbers, //Vector of email addresses
								const Core::UTF8String & instrContents)
		{
			MethodReference sdMethodRef = GetStaticMethodReference("PresentSMS");
			if (sdMethodRef.mMethodID != 0 && sdMethodRef.mClassID != 0)
			{
				JNIEnv* env = GetJNIEnvironmentPtr();
				jobjectArray ajstrRecipients = env->NewObjectArray(inastrRecipientNumbers.size(), env->FindClass("java/lang/String"), env->NewStringUTF(""));
				u32 udwRecipientCount = 0;
				for(std::vector<Core::UTF8String>::const_iterator it = inastrRecipientNumbers.begin(); it != inastrRecipientNumbers.end(); ++it)
				{
					jstring jstrNumbers = CreateJStringFromUTF8String((*it));
					env->SetObjectArrayElement(ajstrRecipients,udwRecipientCount, jstrNumbers);
					env->DeleteLocalRef(jstrNumbers);
					udwRecipientCount++;
				}
				jstring jstrContents = CreateJStringFromUTF8String(instrContents);
				env->CallStaticVoidMethod(sdMethodRef.mClassID, sdMethodRef.mMethodID,ajstrRecipients,jstrContents);

				//delete local reference
				env->DeleteLocalRef(ajstrRecipients);
				env->DeleteLocalRef(jstrContents);
			}
		}

		void SMSCompositionJavaInterface::Dismiss()
		{

		}
	}
}