package com.chillisource.toolutils;

import java.nio.charset.Charset;

public class StringUtils 
{
	//-------------------------------------------------------
	/// Constants
	//-------------------------------------------------------
	private static final Charset kUTF8Charset = Charset.forName("UTF-8");
	//-------------------------------------------------------
	/// String To UTF8 Bytes
	///
	/// Converts a string in UTF-16 format to a UTF8 byte array. 
	///
	/// @param The string to convert.
	/// @return The output byte array in UTF-8 format.
	//-------------------------------------------------------
	public static byte[] stringToUTF8Bytes(final String instrString)
	{
		return instrString.getBytes(kUTF8Charset);
	}
	//-------------------------------------------------------
	/// UTF8 Bytes To String
	///
	/// Converts UTF-8 byte array to a string in UTF-16 format.
	///
	/// @param The byte array to convert.
	/// @return The output string.
	//-------------------------------------------------------
	public static String utf8BytesToString(final byte[] inabyUTF8)
	{
		return new String(inabyUTF8, kUTF8Charset);
	}
	/**
	 * Standardised the path to a file.
	 * 
	 * @author Ian Copland
	 * 
	 * @param The file path.
	 * 
	 * @return The standardised file path.
	 */
	public static String standardiseFilePath(String in_filePath)
	{
		String standardisedPath = in_filePath.replace("\\", "/");
		standardisedPath = in_filePath.replace("//", "/");
		if (standardisedPath.endsWith("/") == true)
		{
			standardisedPath = standardisedPath.substring(0, standardisedPath.length() - 1);
		}
		return standardisedPath;
	}
	/**
	 * Standardised the path to a file.
	 * 
	 * @author Ian Copland
	 * 
	 * @param The file path.
	 * 
	 * @return The standardised file path.
	 */
	public static String standardiseDirectoryPath(String in_directoryPath)
	{
		String standardisedPath = in_directoryPath.replace("\\", "/");
		standardisedPath = in_directoryPath.replace("//", "/");
		if (standardisedPath.endsWith("/") == false)
		{
			standardisedPath += "/";
		}
		return standardisedPath;
	}
	//-------------------------------------------------
	/// Get Extension
	///
	/// returns the extension of a filepath string
	///
	/// @param the directory
	/// @output the standardised path.
	//-------------------------------------------------
	public static String getExtension(String instrFilepath)
	{
		int dwIndex = instrFilepath.lastIndexOf(".");
		if (dwIndex != -1)
			return instrFilepath.substring(dwIndex + 1);
		return "";
	}
	//-------------------------------------------------
	/// Remove Extension
	///
	/// removes the extension of a filepath string
	///
	/// @param the directory
	/// @output the standardised path.
	//-------------------------------------------------
	public static String removeExtension(String instrFilepath)
	{
		int dwIndex = instrFilepath.lastIndexOf(".");
		if (dwIndex != -1)
			return instrFilepath.substring(0, dwIndex);
		return instrFilepath;
	}
	//-------------------------------------------------
	/// Get Directory
	///
	/// removes the filename from a filepath.
	///
	/// @param the filepath
	/// @output the standardised directory path.
	//-------------------------------------------------
	public static String getDirectory(String instrFilepath)
	{
		String strStandardisedFilepath = StringUtils.standardiseFilePath(instrFilepath);
		int dwIndex = strStandardisedFilepath.lastIndexOf("/");
		if (dwIndex != -1)
			return strStandardisedFilepath.substring(0, dwIndex + 1);
		return "";
	}
	//-------------------------------------------------
	/// Remove Whitespace
	///
	/// Removes all spaces, line returns and tabs from
	/// the given string.
	///
	/// @param the string
	/// @output the string with no whitespace
	//-------------------------------------------------
	public static String removeWhitespace(String instrFilepath)
	{
		String strOutput = new String(instrFilepath);
		strOutput = strOutput.replace(" ", "");
		strOutput = strOutput.replace("\n", "");
		strOutput = strOutput.replace("\r", "");
		strOutput = strOutput.replace("\t", "");
		return strOutput;
	}
	//-------------------------------------------------
	/// Convert Exception To String
	///
	/// Returns the contents of a stack trace as a string.
	///
	/// @param the exception
	/// @output the string.
	//-------------------------------------------------
	public static String convertExceptionToString(Exception inException)
	{
		String strOutput = inException.getLocalizedMessage();
		for (StackTraceElement element: inException.getStackTrace())
		{
			strOutput += "\n" + element.toString();
		}
		
		return strOutput;
	}
}
