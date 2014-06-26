/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* 
 * Modified by Hugh McLaughlin on 3 July 2012
 */

package com.chillisource.googleplay;

import com.google.android.vending.expansion.downloader.impl.DownloaderService;

/**
 * This class demonstrates the minimal client implementation of the
 * DownloaderService from the Downloader library.
 */
public class GooglePlayDownloaderService extends DownloaderService 
{
	// stuff for LVL -- MODIFY FOR YOUR APPLICATION!
	private static String BASE64_PUBLIC_KEY = "";
	// used by the preference obfuscater
	private static final byte[] SALT = new byte[] 
	{
		1, 43, -12, -1, 34, 68, -16, -12, 43, 2, -8, -4, 9, 5, -106, 108, -36, 45, -10, 32
	};

	/**
	 * This public key comes from your Android Market publisher account, and it
	 * used by the LVL to validate responses from Market on your behalf.
	 */
	@Override
	public String getPublicKey() {
		return BASE64_PUBLIC_KEY;
	}

	/**
	 * This is used by the preference obfuscater to make sure that your
	 * obfuscated preferences are different than the ones used by other
	 * applications.
	 */
	@Override
	public byte[] getSALT() {
		return SALT;
	}

	/**
	 * Fill this in with the class name for your alarm receiver. We do this
	 * because receivers must be unique across all of Android (it's a good idea
	 * to make sure that your receiver is in your unique package)
	 */
	@Override
	public String getAlarmReceiverClassName() {
		return GooglePlayAlarmReceiver.class.getName();
	}


	//Sets the marketPublicKey
	public static void SetMarketPublicKey(String instrMarketKey)
	{
		BASE64_PUBLIC_KEY = instrMarketKey;
	}

}
