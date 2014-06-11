//
// CSkeletonBuilder.h
// ColladaToMoModelConverter
//
// Created by Ian Copland on 18/04/2013.
// Copyright 2012 Tag Games. All rights reserved.
//

package com.taggames.momodelconverter;

import java.util.Collections;
import java.util.LinkedList;

import com.taggames.colladaparser.colladadata.ColladaNode;
import com.taggames.colladaparser.colladadata.ColladaNode.COLLADA_NODE_TYPE;
import com.taggames.momodelconverter.momodel.MoModel;
import com.taggames.momodelconverter.momodel.MoModelSkeletonNode;
import com.taggames.momodelconverter.momodel.MoModelSkeleton;
import com.taggames.toolutils.SCLogger;

public class SCSkeletonBuilder 
{
	//-------------------------------------------------------------------
	/// Try Build Skeleton
	///
	/// Tries to build the skeleton if it has not already been built. 
	/// If it has already been built, this then confirms that it is using 
	/// the same skeleton.
	//-------------------------------------------------------------------
	static public void TryBuildSkeleton(LinkedList<ColladaNode> inNodes, String instrSkeletonName, MoModel inModel)
	{
		if (inModel.mSkeleton.mbLocked == false)
		{
			BuildSkeleton(inNodes, instrSkeletonName, inModel);
			OrderSkeleton(inModel);
			inModel.mSkeleton.mbLocked = true;
		}
		else
		{
			MoModel testModel = new MoModel();
			BuildSkeleton(inNodes, instrSkeletonName, testModel);
			OrderSkeleton(testModel);
			
			if (CompareSkeletons(inModel.mSkeleton, testModel.mSkeleton) == false)
			{
				String strRootNode1 = "", strRootNode2 = "";
				if (inModel.mSkeleton.mNodeList.size() > 0)
					strRootNode1 = inModel.mSkeleton.mNodeList.get(0).mstrName;
				if (testModel.mSkeleton.mNodeList.size() > 0)
					strRootNode2 = testModel.mSkeleton.mNodeList.get(0).mstrName;
				SCLogger.LogFatalError("A second skeleton has been found. All meshes must be attached to the same skeleton. Attaching a mesh to a subset of the skeleton counts as a different skeleton. The two different skeleton root nodes are: '" + strRootNode1 + "' and '" + strRootNode2 + "'");
			}
		}
	}
	//-------------------------------------------------------------------
	/// Compare Skeletons
	///
	/// Compares the two skeletons to see if all relevent data in them 
	/// is the same.
	//-------------------------------------------------------------------
	static private boolean CompareSkeletons(MoModelSkeleton inSkeleton1, MoModelSkeleton inSkeleton2)
	{
		//check the list counts are the same
		if (inSkeleton1.mNodeList.size() != inSkeleton2.mNodeList.size())
			return false;
		
		//check all the joints
		for (int i = 0; i < inSkeleton1.mNodeList.size(); i++)
		{
			MoModelSkeletonNode node1 = inSkeleton1.mNodeList.get(i);
			MoModelSkeletonNode node2 = inSkeleton2.mNodeList.get(i);
			
			if (node1.mstrName.equals(node2.mstrName) == false)
				return false;
			if (node1.mdwParentNodeIndex != node2.mdwParentNodeIndex)
				return false;
		}
		
		return true;
	}
	//-------------------------------------------------------------------
	/// Build Skeleton
	///
	/// builds to the skeleton.
	//-------------------------------------------------------------------
	static private boolean BuildSkeleton(LinkedList<ColladaNode> inNodes, String instrSkeletonName, MoModel inModel)
	{
		//find the "root" node that we are using
		for (ColladaNode node : inNodes)
		{
			if (node.mstrId.equals(instrSkeletonName))
			{
				//build skeleton
				AddToSkeleton(node, null, inModel);
				return true;
			}
			else
			{
				if (BuildSkeleton(new LinkedList<ColladaNode>(node.mChildNodes.values()), instrSkeletonName, inModel) == true)
				{
					return true;
				}
			}
		}
		
		return false;
	}
	//-------------------------------------------------------------------
	/// Add To Skeleton
	///
	/// Adds a node tree to the skeleton.
	//-------------------------------------------------------------------
	static private void AddToSkeleton(ColladaNode inNode, ColladaNode inParent, MoModel inModel)
	{
		//if its a skeleton node, then add it to the skeleton node list
		if (inNode.meType == COLLADA_NODE_TYPE.JOINT || inNode.meType == COLLADA_NODE_TYPE.BASE)
		{
			TryAddSkeletonNode(inNode, inParent, inModel);
			for (ColladaNode node: inNode.mChildNodes.values())
			{
				AddToSkeleton(node, inNode, inModel);
			}
		}
	}
	//-------------------------------------------------------------------
	/// Try Add Skeleton Node
	///
	/// Tries to add a joint to the skeleton.
	//-------------------------------------------------------------------
	static private void TryAddSkeletonNode(ColladaNode inNode, ColladaNode inParent, MoModel inModel)
	{
		//check and see if this node already exists
		for (int i = 0; i < inModel.mSkeleton.mNodeList.size(); i++)
		{
			MoModelSkeletonNode node = inModel.mSkeleton.mNodeList.get(i);
			if (node.mstrId.equalsIgnoreCase(inNode.mstrId))
			{
				//if the node does already exist, then make sure the parent is correct
				if (node.mstrParentId.length() == 0 && inParent != null)
				{
					node.mstrParentId = inParent.mstrId;
				}
				return;
			}
		}
		
		//create a new skeleton node
		MoModelSkeletonNode newSkeletonNode = new MoModelSkeletonNode();
		newSkeletonNode.mstrName = inNode.mstrName;
		newSkeletonNode.mstrId = inNode.mstrId;
		if (inParent != null)
		{
			newSkeletonNode.mstrParentId = inParent.mstrId;
		}
		
		//if it's a joint store the SId.
		if (inNode.meType == COLLADA_NODE_TYPE.JOINT)
		{
			newSkeletonNode.mbIsJoint = true;
			newSkeletonNode.mstrSId = inNode.mstrSId;
		}
		
		//add the joint
		inModel.mSkeleton.mNodeList.add(newSkeletonNode);
	}
	//-------------------------------------------------------------------
	/// Order Skeleton
	///
	/// Orders the skeleton based on the joints ordering index. The parent 
	/// indices for each joint, and the joint index for each attachment 
	/// point are then calculated.
	//-------------------------------------------------------------------
	static private void OrderSkeleton(MoModel inModel)
	{
		Collections.sort(inModel.mSkeleton.mNodeList, new SkeletonNodeComparator());
		
		int dwJointCount = 0;
		
		//iterator the list and get the parent indices
		for (MoModelSkeletonNode node : inModel.mSkeleton.mNodeList)
		{
			if (node.mbIsJoint == true)
			{
				node.mdwJointIndex = dwJointCount;
				dwJointCount++;
			}
			node.mdwParentNodeIndex = GetIndexOfSkeletonNodeById(inModel, node.mstrParentId);
		}
	}
	//-------------------------------------------------------------------
	/// Get Index Of Skeleton Node By Id
	///
	/// @return The index of the skeleton node with the passed in id.
	//-------------------------------------------------------------------
	static private int GetIndexOfSkeletonNodeById(MoModel inModel, String instrId)
	{
		for (int i = 0; i < inModel.mSkeleton.mNodeList.size(); i++)
		{
			MoModelSkeletonNode node = inModel.mSkeleton.mNodeList.get(i);
			if (node.mstrId.equalsIgnoreCase(instrId))
			{
				return i;
			}
		}
		return -1;
	}
	//-------------------------------------------------------------------
	/// Get Index Of Joint By SId
	///
	/// @return The index of the joint with the passed in name.
	//-------------------------------------------------------------------
	static public int GetIndexOfJointBySId(MoModel inModel, String instrSId)
	{
		for (int i = 0; i < inModel.mSkeleton.mNodeList.size(); i++)
		{
			MoModelSkeletonNode node = inModel.mSkeleton.mNodeList.get(i);
			if (node.mbIsJoint == true && node.mstrSId.equalsIgnoreCase(instrSId))
			{
				return node.mdwJointIndex;
			}
		}
		
		return -1;
	}
	//-------------------------------------------------------------------
	/// Get Number of Joints
	///
	/// @return The number of joints in the given skeleton
	//-------------------------------------------------------------------
	static public int GetNumberOfJoints(MoModel inModel)
	{
		int dwCount = 0;
		for (int i = 0; i < inModel.mSkeleton.mNodeList.size(); i++)
		{
			MoModelSkeletonNode node = inModel.mSkeleton.mNodeList.get(i);
			if (node.mbIsJoint == true)
			{
				dwCount++;
			}
		}
		return dwCount;
	}
}
