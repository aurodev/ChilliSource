//
// CLibraryEffectsParser.h
// Collada Parser
//
// Created by Ian Copland on 16/01/2013.
// Copyright 2012 Tag Games. All rights reserved.
//

package com.taggames.colladaparser;

import org.xml.sax.Attributes;
import org.xml.sax.SAXException;

import com.taggames.colladaparser.colladadata.*;
import com.taggames.toolutils.SCLogger;

public class CLibraryEffectsParser 
{
	//--------------------------------------------------------------
	/// Private Member Data
	//--------------------------------------------------------------
	private int mdwIgnoreStack;
	private Collada mCollada;
	private ColladaEffect mCurrentEffect;
	private ColladaNewParam mCurrentNewParam;
	private ColladaTechnique mCurrentTechnique;
	private ColladaMaterialColour mCurrentMaterialColour;
	private ColladaShininess mCurrentShininess;
	private ColladaColour mCurrentColour;
	private ColladaFloat mCurrentFloat;
	private ColladaSource mCurrentSource;
	private ColladaInitFrom mCurrentInitFrom;
	//--------------------------------------------------------------
	/// Constructor
	//--------------------------------------------------------------
	public CLibraryEffectsParser(Collada inColladaData)
	{
		mCollada = inColladaData;
		mdwIgnoreStack = 0;
	}
	//--------------------------------------------------------------
	/// Start Element
	///
	/// Callback method used by SAX whenever a new element is found.
	//--------------------------------------------------------------
	public boolean StartElement(String instrUri, String instrLocalName,String instrQName, Attributes inAttributes) throws SAXException 
	{	
		//push element type on to the imaginary element stack
		if (instrQName.equalsIgnoreCase("effect"))
			return PushEffect(inAttributes);
		else if (instrQName.equalsIgnoreCase("profile_COMMON"))
			return PushProfileCommon(inAttributes);
		else if (instrQName.equalsIgnoreCase("newparam"))
			return PushNewParam(inAttributes);
		else if (instrQName.equalsIgnoreCase("surface"))
			return PushSurface(inAttributes);
		else if (instrQName.equalsIgnoreCase("sampler2D"))
			return PushSampler2D(inAttributes);
		else if (instrQName.equalsIgnoreCase("technique"))
			return PushTechnique(inAttributes);
		else if (instrQName.equalsIgnoreCase("blinn"))
			return PushBlinn(inAttributes);
		else if (instrQName.equalsIgnoreCase("phong"))
			return PushPhong(inAttributes);
		else if (instrQName.equalsIgnoreCase("emissive"))
			return PushEmissive(inAttributes);
		else if (instrQName.equalsIgnoreCase("ambient"))
			return PushAmbient(inAttributes);
		else if (instrQName.equalsIgnoreCase("diffuse"))
			return PushDiffuse(inAttributes);
		else if (instrQName.equalsIgnoreCase("specular"))
			return PushSpecular(inAttributes);
		else if (instrQName.equalsIgnoreCase("shininess"))
			return PushShininess(inAttributes);
		else if (instrQName.equalsIgnoreCase("color"))
			return PushColour(inAttributes);
		else if (instrQName.equalsIgnoreCase("texture"))
			return PushTexture(inAttributes);
		else if (instrQName.equalsIgnoreCase("float"))
			return PushFloat(inAttributes);
		else if (instrQName.equalsIgnoreCase("source"))
			return PushSource(inAttributes);
		else if (instrQName.equalsIgnoreCase("init_from"))
			return PushInitFrom(inAttributes);
		else
		{
			mdwIgnoreStack++;
			return false;
		}
	}
	//--------------------------------------------------------------
	/// End Element
	///
	/// Callback method used by SAX whenever a new element is found.
	//--------------------------------------------------------------
	public void EndElement(String instrUri, String instrLocalName,String instrQName, String inData) throws SAXException 
	{
		//pop element type off to the imaginary element stack
		if (instrQName.equalsIgnoreCase("effect"))
			PopEffect(inData);
		else if (instrQName.equalsIgnoreCase("profile_COMMON"))
			PopProfileCommon(inData);
		else if (instrQName.equalsIgnoreCase("newparam"))
			PopNewParam(inData);
		else if (instrQName.equalsIgnoreCase("surface"))
			PopSurface(inData);
		else if (instrQName.equalsIgnoreCase("sampler2D"))
			PopSampler2D(inData);
		else if (instrQName.equalsIgnoreCase("technique"))
			PopTechnique(inData);
		else if (instrQName.equalsIgnoreCase("blinn"))
			PopBlinn(inData);
		else if (instrQName.equalsIgnoreCase("phong"))
			PopPhong(inData);
		else if (instrQName.equalsIgnoreCase("emissive"))
			PopEmissive(inData);
		else if (instrQName.equalsIgnoreCase("ambient"))
			PopAmbient(inData);
		else if (instrQName.equalsIgnoreCase("diffuse"))
			PopDiffuse(inData);
		else if (instrQName.equalsIgnoreCase("specular"))
			PopSpecular(inData);
		else if (instrQName.equalsIgnoreCase("shininess"))
			PopShininess(inData);
		else if (instrQName.equalsIgnoreCase("color"))
			PopColour(inData);
		else if (instrQName.equalsIgnoreCase("texture"))
			PopTexture(inData);
		else if (instrQName.equalsIgnoreCase("float"))
			PopFloat(inData);
		else if (instrQName.equalsIgnoreCase("source"))
			PopSource(inData);
		else if (instrQName.equalsIgnoreCase("init_from"))
			PopInitFrom(inData);
		else
			mdwIgnoreStack--;
	}
	//--------------------------------------------------------------
	/// Push Effect
	///
	/// Pushes an effect element type on to the element stack.
	//--------------------------------------------------------------
	private boolean PushEffect(Attributes inAttributes)
	{
		if (mdwIgnoreStack > 0 ) return false;
		
		mCurrentEffect = new ColladaEffect();
		mCurrentEffect.mstrId = inAttributes.getValue("id");
		
		if (mCollada.mLibraryEffects != null)
			mCollada.mLibraryEffects.put(mCurrentEffect.mstrId, mCurrentEffect);
		else
			SCLogger.LogFatalError("mLibraryEffects is null!");
		
		return false;
	}
	//--------------------------------------------------------------
	/// Pop Effect
	///
	/// Pops an effect element type from the element stack.
	//--------------------------------------------------------------
	private void PopEffect(String inData)
	{
		mCurrentEffect = null;
	}
	//--------------------------------------------------------------
	/// Push Profile Common
	///
	/// Pushes an profile common element type on to the element stack.
	//--------------------------------------------------------------
	private boolean PushProfileCommon(Attributes inAttributes)
	{
		if (mdwIgnoreStack > 0 ) return false;
		
		ColladaProfileCommon profileCommon = new ColladaProfileCommon();
		
		if (mCurrentEffect != null)
			mCurrentEffect.mProfileCommon = profileCommon;
		else
			SCLogger.LogFatalError("mCurrentEffect is null!");
		
		return false;
	}
	//--------------------------------------------------------------
	/// Pop Profile Common
	///
	/// Pops an profile common element type from the element stack.
	//--------------------------------------------------------------
	private void PopProfileCommon(String inData)
	{
		if (mdwIgnoreStack > 0 ) return;
	}
	//--------------------------------------------------------------
	/// Push New Param
	///
	/// Pushes an new param element type on to the element stack.
	//--------------------------------------------------------------
	private boolean PushNewParam(Attributes inAttributes)
	{
		if (mdwIgnoreStack > 0 ) return false;
		
		mCurrentNewParam = new ColladaNewParam();
		mCurrentNewParam.mstrSId = inAttributes.getValue("sid");
		
		if (mCurrentEffect != null && mCurrentEffect.mProfileCommon != null)
			mCurrentEffect.mProfileCommon.mNewParams.put(mCurrentNewParam.mstrSId, mCurrentNewParam);
		else
			SCLogger.LogFatalError("mCurrentEffect is null!");
		
		return false;
	}
	//--------------------------------------------------------------
	/// Pop New Param
	///
	/// Pops an new param element type from the element stack.
	//--------------------------------------------------------------
	private void PopNewParam(String inData)
	{
		if (mdwIgnoreStack > 0 ) return;
		mCurrentNewParam = null;
	}
	//--------------------------------------------------------------
	/// Push Surface
	///
	/// Pushes a surface element type on to the element stack.
	//--------------------------------------------------------------
	private boolean PushSurface(Attributes inAttributes)
	{
		if (mdwIgnoreStack > 0 ) return false;
		
		ColladaSurface newSurface = new ColladaSurface();
		newSurface.mstrType = inAttributes.getValue("type");
		
		if (mCurrentNewParam != null)
			mCurrentNewParam.mSurface = newSurface;
		else
			SCLogger.LogFatalError("mCurrentNewParam is null!");
		
		return false;
	}
	//--------------------------------------------------------------
	/// Pop Surface
	///
	/// Pops a surface element type from the element stack.
	//--------------------------------------------------------------
	private void PopSurface(String inData)
	{
		if (mdwIgnoreStack > 0 ) return;
	}
	//--------------------------------------------------------------
	/// Push Sampler 2D
	///
	/// Pushes a sampler 2D element type on to the element stack.
	//--------------------------------------------------------------
	private boolean PushSampler2D(Attributes inAttributes)
	{
		if (mdwIgnoreStack > 0 ) return false;
		
		ColladaSampler2D newSampler2D = new ColladaSampler2D();
		
		if (mCurrentNewParam != null)
			mCurrentNewParam.mSampler2D = newSampler2D;
		else
			SCLogger.LogFatalError("mCurrentNewParam is null!");
		
		return false;
	}
	//--------------------------------------------------------------
	/// Pop Sampler 2D
	///
	/// pops a sampler 2D element type from the element stack.
	//--------------------------------------------------------------
	private void PopSampler2D(String inData)
	{
		if (mdwIgnoreStack > 0 ) return;
	}
	//--------------------------------------------------------------
	/// Push Technique
	///
	/// Pushes a technique element type on to the element stack.
	//--------------------------------------------------------------
	private boolean PushTechnique(Attributes inAttributes)
	{
		if (mdwIgnoreStack > 0 ) return false;
		
		mCurrentTechnique = new ColladaTechnique();
		mCurrentTechnique.mstrSId = inAttributes.getValue("sid");
		
		if (mCurrentEffect != null && mCurrentEffect.mProfileCommon != null)
			mCurrentEffect.mProfileCommon.mTechnique = mCurrentTechnique;
		else
			SCLogger.LogFatalError("mCurrentEffect is null!");
		
		return false;
	}
	//--------------------------------------------------------------
	/// Pop Technique
	///
	/// Pops a technique element type from the element stack.
	//--------------------------------------------------------------
	private void PopTechnique(String inData)
	{
		if (mdwIgnoreStack > 0 ) return;
		mCurrentTechnique = null;
	}
	//--------------------------------------------------------------
	/// Push Blinn
	///
	/// Pushes a blinn element type on to the element stack.
	//--------------------------------------------------------------
	private boolean PushBlinn(Attributes inAttributes)
	{
		if (mdwIgnoreStack > 0 ) return false;
		
		ColladaBlinnAndPhong newBlinnAndPhong = new ColladaBlinnAndPhong();
		
		if (mCurrentTechnique != null)
			mCurrentTechnique.mBlinnAndPhong = newBlinnAndPhong;
		else
			SCLogger.LogFatalError("mCurrentTechnique is null!");
		
		return false;
	}
	//--------------------------------------------------------------
	/// Pop Blinn
	///
	/// Pops a blinn element type from the element stack.
	//--------------------------------------------------------------
	private void PopBlinn(String inData)
	{
		if (mdwIgnoreStack > 0 ) return;
	}
	//--------------------------------------------------------------
	/// Push Phong
	///
	/// Pushes a phong element type on to the element stack.
	//--------------------------------------------------------------
	private boolean PushPhong(Attributes inAttributes)
	{
		if (mdwIgnoreStack > 0 ) return false;
		
		ColladaBlinnAndPhong newBlinnAndPhong = new ColladaBlinnAndPhong();
		
		if (mCurrentTechnique != null)
			mCurrentTechnique.mBlinnAndPhong = newBlinnAndPhong;
		else
			SCLogger.LogFatalError("mCurrentTechnique is null!");
		
		return false;
	}
	//--------------------------------------------------------------
	/// Pop Phong
	///
	/// Pops a phong element type from the element stack.
	//--------------------------------------------------------------
	private void PopPhong(String inData)
	{
		if (mdwIgnoreStack > 0 ) return;
	}
	//--------------------------------------------------------------
	/// Push Emissive
	///
	/// Pushes an emissive element type on to the element stack.
	//--------------------------------------------------------------
	private boolean PushEmissive(Attributes inAttributes)
	{
		if (mdwIgnoreStack > 0 ) return false;
		
		mCurrentMaterialColour = new ColladaMaterialColour();
		
		//Default the emissive
		mCurrentMaterialColour.mColour = ColladaBlinnAndPhong.DEFAULT_EMISSIVE;
		
		if (mCurrentTechnique != null && mCurrentTechnique.mBlinnAndPhong != null)
			mCurrentTechnique.mBlinnAndPhong.mEmissive = mCurrentMaterialColour;
		else
			SCLogger.LogFatalError("mBlinnAndPhong is null!");
		
		return false;
	}
	//--------------------------------------------------------------
	/// Pop Emissive
	///
	/// Pops an emissive element type from the element stack.
	//--------------------------------------------------------------
	private void PopEmissive(String inData)
	{
		if (mdwIgnoreStack > 0 ) return;
		mCurrentMaterialColour = null;
	}
	//--------------------------------------------------------------
	/// Push Ambient
	///
	/// Pushes an ambient element type on to the element stack.
	//--------------------------------------------------------------
	private boolean PushAmbient(Attributes inAttributes)
	{
		if (mdwIgnoreStack > 0 ) return false;
		
		mCurrentMaterialColour = new ColladaMaterialColour();
		
		if (mCurrentTechnique != null && mCurrentTechnique.mBlinnAndPhong != null)
			mCurrentTechnique.mBlinnAndPhong.mAmbient = mCurrentMaterialColour;
		else
			SCLogger.LogFatalError("mBlinnAndPhong is null!");
		
		return false;
	}
	//--------------------------------------------------------------
	/// Pop Ambient
	///
	/// Pops an ambient element type from the element stack.
	//--------------------------------------------------------------
	private void PopAmbient(String inData)
	{
		if (mdwIgnoreStack > 0 ) return;
		mCurrentMaterialColour = null;
	}
	//--------------------------------------------------------------
	/// Push Diffuse
	///
	/// Pushes a diffuse element type on to the element stack.
	//--------------------------------------------------------------
	private boolean PushDiffuse(Attributes inAttributes)
	{
		if (mdwIgnoreStack > 0 ) return false;
		
		mCurrentMaterialColour = new ColladaMaterialColour();
		
		//Default the diffuse
		mCurrentMaterialColour.mColour = ColladaBlinnAndPhong.DEFAULT_DIFFUSE;
		
		if (mCurrentTechnique != null && mCurrentTechnique.mBlinnAndPhong != null)
			mCurrentTechnique.mBlinnAndPhong.mDiffuse = mCurrentMaterialColour;
		else
			SCLogger.LogFatalError("mBlinnAndPhong is null!");
		
		return false;
	}
	//--------------------------------------------------------------
	/// Pop Diffuse
	///
	/// Pops a diffuse element type from the element stack.
	//--------------------------------------------------------------
	private void PopDiffuse(String inData)
	{
		if (mdwIgnoreStack > 0 ) return;
		mCurrentMaterialColour = null;
	}
	//--------------------------------------------------------------
	/// Push Specular
	///
	/// Pushes a specular element type on to the element stack.
	//--------------------------------------------------------------
	private boolean PushSpecular(Attributes inAttributes)
	{
		if (mdwIgnoreStack > 0 ) return false;
		
		mCurrentMaterialColour = new ColladaMaterialColour();
		
		if (mCurrentTechnique != null && mCurrentTechnique.mBlinnAndPhong != null)
			mCurrentTechnique.mBlinnAndPhong.mSpecular = mCurrentMaterialColour;
		else
			SCLogger.LogFatalError("mBlinnAndPhong is null!");
		
		return false;
	}
	//--------------------------------------------------------------
	/// Pop Specular
	///
	/// Pops a specular element type from the element stack.
	//--------------------------------------------------------------
	private void PopSpecular(String inData)
	{
		if (mdwIgnoreStack > 0 ) return;
		mCurrentMaterialColour = null;
	}
	//--------------------------------------------------------------
	/// Push Shininess
	///
	/// Pushes a shininess element type on to the element stack.
	//--------------------------------------------------------------
	private boolean PushShininess(Attributes inAttributes)
	{
		if (mdwIgnoreStack > 0 ) return false;
		
		mCurrentShininess = new ColladaShininess();
		
		if (mCurrentTechnique != null && mCurrentTechnique.mBlinnAndPhong != null)
			mCurrentTechnique.mBlinnAndPhong.mShininess = mCurrentShininess;
		else
			SCLogger.LogFatalError("mBlinnAndPhong is null!");
		
		return false;
	}
	//--------------------------------------------------------------
	/// Pop Shininess
	///
	/// Pops a shininess element type from the element stack.
	//--------------------------------------------------------------
	private void PopShininess(String inData)
	{
		if (mdwIgnoreStack > 0 ) return;
		mCurrentShininess = null;
	}
	//--------------------------------------------------------------
	/// Push Colour
	///
	/// Pushes a colour element type on to the element stack.
	//--------------------------------------------------------------
	private boolean PushColour(Attributes inAttributes)
	{
		if (mdwIgnoreStack > 0 ) return false;
		
		mCurrentColour = new ColladaColour();
		
		if (mCurrentMaterialColour != null)
			mCurrentMaterialColour.mColour = mCurrentColour;
		else
			SCLogger.LogFatalError("mBlinnAndPhong is null!");

		return true;
	}
	//--------------------------------------------------------------
	/// Pop Colour
	///
	/// Pops a colour element type from the element stack.
	//--------------------------------------------------------------
	private void PopColour(String inData)
	{
		if (mdwIgnoreStack > 0 ) return;
		String[] splitColourString = CColladaUtils.SplitOnWhitespace(inData);
		
		if (splitColourString.length != 4)
		{
			SCLogger.LogFatalError("ColladaColour does not contain 4 values!");
		}
		
		mCurrentColour.mfR = Float.parseFloat(splitColourString[0]);
		mCurrentColour.mfG = Float.parseFloat(splitColourString[1]);
		mCurrentColour.mfB = Float.parseFloat(splitColourString[2]);
		mCurrentColour.mfA = Float.parseFloat(splitColourString[3]);
		
		mCurrentColour = null;
	}
	//--------------------------------------------------------------
	/// Push Texture
	///
	/// Pushes a texture element type on to the element stack.
	//--------------------------------------------------------------
	private boolean PushTexture(Attributes inAttributes)
	{
		if (mdwIgnoreStack > 0 ) return false;
		
		ColladaTexture texture = new ColladaTexture();
		texture.mstrTexCoord = inAttributes.getValue("texcoord");
		texture.mstrTexture = inAttributes.getValue("texture");
		
		if (mCurrentMaterialColour != null)
			mCurrentMaterialColour.mTexture = texture;
		else
			SCLogger.LogFatalError("mCurrentMaterialColour is null!");
		
		return false;
	}
	//--------------------------------------------------------------
	/// Pop Texture
	///
	/// Pops a texture element type from the element stack.
	//--------------------------------------------------------------
	private void PopTexture(String inData)
	{
		if (mdwIgnoreStack > 0 ) return;
	}
	//--------------------------------------------------------------
	/// Push Float
	///
	/// Pushes a float element type on to the element stack.
	//--------------------------------------------------------------
	private boolean PushFloat(Attributes inAttributes)
	{
		if (mdwIgnoreStack > 0 ) return false;
		
		mCurrentFloat = new ColladaFloat();
		
		if (mCurrentShininess != null)
			mCurrentShininess.mFloat = mCurrentFloat;
		else
			SCLogger.LogFatalError("mCurrentShininess is null!");
		
		return true;
	}
	//--------------------------------------------------------------
	/// Pop Float
	///
	/// Pops a float element type from the element stack.
	//--------------------------------------------------------------
	private void PopFloat(String inData)
	{
		if (mdwIgnoreStack > 0 ) return;
		mCurrentFloat.mfValue = Float.parseFloat(inData);
		
		mCurrentFloat = null;
	}
	//--------------------------------------------------------------
	/// Push Source
	///
	/// Pushes a source element type on to the element stack.
	//--------------------------------------------------------------
	private boolean PushSource(Attributes inAttributes)
	{
		if (mdwIgnoreStack > 0 ) return false;
		
		mCurrentSource = new ColladaSource();
		
		if (mCurrentNewParam != null && mCurrentNewParam.mSampler2D != null)
			mCurrentNewParam.mSampler2D.mSource = mCurrentSource;
		else
			SCLogger.LogFatalError("mSampler2D is null!");
		
		return true;
	}
	//--------------------------------------------------------------
	/// Pop Source
	///
	/// Pops a source element type from the element stack.
	//--------------------------------------------------------------
	private void PopSource(String inData)
	{
		if (mdwIgnoreStack > 0 ) return;
		
		mCurrentSource.mstrContents = inData;
		
		mCurrentSource = null;
	}
	//--------------------------------------------------------------
	/// Push Init From
	///
	/// Pushes an init from element type on to the element stack.
	//--------------------------------------------------------------
	private boolean PushInitFrom(Attributes inAttributes)
	{
		if (mdwIgnoreStack > 0 ) return false;
		
		mCurrentInitFrom = new ColladaInitFrom();
		
		if (mCurrentNewParam != null && mCurrentNewParam.mSurface != null)
			mCurrentNewParam.mSurface.mInitFrom = mCurrentInitFrom;
		else
			SCLogger.LogFatalError("mSurface is null!");
		
		return true;
	}
	//--------------------------------------------------------------
	/// Pop Init From
	///
	/// Pops an init from element type from the element stack.
	//--------------------------------------------------------------
	private void PopInitFrom(String inData)
	{
		if (mdwIgnoreStack > 0 ) return;
		
		mCurrentInitFrom.mstrValue = inData;
		
		mCurrentSource = null;
	}
}
