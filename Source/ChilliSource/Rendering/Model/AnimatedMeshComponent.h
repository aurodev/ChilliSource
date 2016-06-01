//
//  AnimatedMeshComponent.h
//  Chilli Source
//  Created by Ian Copland on 17/10/2011.
//
//  The MIT License (MIT)
//
//  Copyright (c) 2011 Tag Games Limited
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

#ifndef _CHILLISOURCE_RENDERING_ANIMATED_MESH_COMPONENT_H_
#define _CHILLISOURCE_RENDERING_ANIMATED_MESH_COMPONENT_H_

#include <ChilliSource/ChilliSource.h>
#include <ChilliSource/Core/Event/Event.h>
#include <ChilliSource/Core/File/FileSystem.h>
#include <ChilliSource/Core/Volume/VolumeComponent.h>
#include <ChilliSource/Rendering/Model/SkinnedAnimationGroup.h>
#include <ChilliSource/Rendering/Model/Model.h>

#include <functional>

namespace ChilliSource
{
    //===============================================================
    /// Animation Playback Type
    ///
    /// Describes the different types of animation playback.
    //===============================================================
    enum class AnimationPlaybackType
    {
        k_once,
        k_looping
    };
    //===============================================================
    /// Events
    //===============================================================
    typedef std::function<void(AnimatedMeshComponent*)> AnimationChangedDelegate;
    typedef Event<AnimationChangedDelegate> AnimationChangedEvent;
    typedef std::function<void(AnimatedMeshComponent*)> AnimationCompletionDelegate;
    typedef Event<AnimationCompletionDelegate> AnimationCompletionEvent;
    typedef std::function<void(AnimatedMeshComponent*)> AnimationLoopedDelegate;
    typedef Event<AnimationLoopedDelegate> AnimationLoopedEvent;
    //===============================================================
    /// Animated Model component
    ///
    /// An animated mesh component. This defines a 3D mesh that can
    /// be manipulated, textured and animated.
    //===============================================================
    class AnimatedMeshComponent : public VolumeComponent
    {
    public:
        CS_DECLARE_NAMEDTYPE(AnimatedMeshComponent);
        
        AnimatedMeshComponent();
        ~AnimatedMeshComponent();
        //----------------------------------------------------------
        /// Is A
        ///
        /// Returns if it is of the type given
        /// @param Comparison Type
        /// @return Whether the class matches the comparison type
        //----------------------------------------------------------
        bool IsA(InterfaceIDType inInterfaceID) const override;
        //----------------------------------------------------
        /// Get Axis Aligned Bounding Box
        ///
        /// All render components have a box used for culling
        /// and coarse intersections. This is cached and 
        /// recomputed when required.
        /// @return Axis aligned bounding box
        //----------------------------------------------------
        const AABB& GetAABB() override;
        //----------------------------------------------------
        /// Get Object Oriented Bounding Box
        ///
        /// All render objects have an OOBB for
        /// picking. This is cached and 
        /// recomputed when required.
        /// @return OOBB
        //----------------------------------------------------
        const OOBB& GetOOBB() override;
        //----------------------------------------------------
        /// Get Bounding Sphere
        ///
        /// All render objects have an bounding sphere for
        /// culling. This is cached and 
        /// recomputed when required.
        /// @return bounding sphere
        //----------------------------------------------------
        const Sphere& GetBoundingSphere() override;
        //----------------------------------------------------
        /// Is Visible
        ///
        /// @return Whether or not to render
        //----------------------------------------------------
        bool IsVisible() const override { return m_isVisible; }
        //----------------------------------------------------
        /// Is Visible
        ///
        /// @param in_isVisible - Whether or not to render
        //----------------------------------------------------
        void SetVisible(bool in_isVisible) { m_isVisible = in_isVisible; }
        //-----------------------------------------------------------
        /// Set Material
        ///
        /// Set the material that the mesh will use. Applies the material
        /// To all submeshes
        ///
        /// @param Handle to material
        //-----------------------------------------------------------
        void SetMaterial(const MaterialCSPtr& inpMaterial);
        //-----------------------------------------------------------
        /// Set Material For Sub Model
        ///
        /// Set the material that one sub mesh will use.
        ///
        /// @param Handle to material
        /// @Param Index to the submesh
        //-----------------------------------------------------------
        void SetMaterialForSubMesh(const MaterialCSPtr& inpMaterial, u32 indwSubMeshIndex);
        //-----------------------------------------------------------
        /// Set Material For Sub Model
        ///
        /// Set the material that one sub mesh will use.
        ///
        /// @param Handle to material
        /// @param The name of the submesh.
        //-----------------------------------------------------------
        void SetMaterialForSubMesh(const MaterialCSPtr& inpMaterial, const std::string& instrSubMeshName);
        //-----------------------------------------------------------
        /// Get Material Of Sub Model
        ///
        /// Get the material of a single sub mesh.
        ///
        /// @param Index to the sub mesh
        /// @return Handle to material
        //-----------------------------------------------------------
        const MaterialCSPtr GetMaterialOfSubMesh(u32 indwSubMeshIndex) const;
        //-----------------------------------------------------------
        /// Get Material Of Sub Model
        ///
        /// Get the material of a single sub mesh.
        ///
        /// @param The name of the submesh.
        /// @return Handle to material
        //-----------------------------------------------------------
        MaterialCSPtr GetMaterialOfSubMesh(const std::string& instrSubMeshName) const;
        //----------------------------------------------------------
        /// Attach Model
        ///
        /// Attach a mesh to this component
        /// @param Model object
        //----------------------------------------------------------
        void AttachMesh(const ModelCSPtr& inpModel);
        //----------------------------------------------------------
        /// Attach Model
        ///
        /// Attach a mesh to this component but uses the given 
        /// material
        /// @param Model object
        //----------------------------------------------------------
        void AttachMesh(const ModelCSPtr& inpModel, const MaterialCSPtr& inpMaterial);
        //----------------------------------------------------------
        /// Get Model
        ///
        /// @return The components internal mesh
        //----------------------------------------------------------
        const ModelCSPtr& GetMesh() const;
        //----------------------------------------------------------
        /// Attach Animation
        ///
        /// Attaches an animation to the active animation group.
        /// The active animation group must have at least one 
        /// animation in it for the mesh to render.
        ///
        /// @param The animation pointer.
        /// @param the position of the animation on the blendline.
        //----------------------------------------------------------
        void AttachAnimation(const SkinnedAnimationCSPtr& inpAnimation, f32 infBlendlinePosition = 0.0f);
        //----------------------------------------------------------
        /// Detatch Animation
        ///
        /// Removes an animation from the active animation group.
        ///
        /// @param The animation pointer.
        /// @param the position of the animation on the blendline.
        //----------------------------------------------------------
        void DetatchAnimation(const SkinnedAnimationCSPtr& inpAnimation);
        //----------------------------------------------------------
        /// Get Animations
        ///
        /// @param OUT: A list of all attached animations.
        //----------------------------------------------------------
        void GetAnimations(std::vector<SkinnedAnimationCSPtr>& outapSkinnedAnimationList);
        //----------------------------------------------------------
        /// Fade Out
        ///
        /// Fades out the active animation group. The active group
        /// will become the "fading" group while a new animation
        /// group will replace it as "active". This new group will
        /// need to have at least one animation attached to it 
        /// before the mesh can be rendered.
        ///
        /// @param The animation fade type.
        /// @param the time to fade out over.
        //----------------------------------------------------------
        void FadeOut(AnimationBlendType ineFadeType, f32 infFadeOutTime);
        //----------------------------------------------------------
        /// Clear Animations
        ///
        /// Clears the content of all animation groups. The
        /// mesh will not be able to render until at least one 
        /// animation has been added again to the active animation
        /// group.
        //----------------------------------------------------------
        void ClearAnimations();
        //----------------------------------------------------------
        /// Attach Entity
        ///
        /// Attached an entity to a node on the animated meshes
        /// skeleton.
        ///
        /// @param The entity.
        /// @param the name of the node to attach to.
        //----------------------------------------------------------
        void AttachEntity(const EntitySPtr& inpEntity, const std::string& instrNodeName);
        //----------------------------------------------------------
        /// Detatch Entity
        ///
        /// Removes an entity that has previously been attached to
        /// the animated meshes skeleton.
        ///
        /// @param The entity.
        //----------------------------------------------------------
        void DetatchEntity(Entity* inpEntity);
        //----------------------------------------------------------
        /// Detatch All Entities
        ///
        /// Removes all entities that have previously been attached to
        /// the animated meshes skeleton.
        //----------------------------------------------------------
        void DetatchAllEntities();
        //----------------------------------------------------------
        /// Set Blendline Position
        ///
        /// Sets the current position on the blendline.
        ///
        /// @param The blendline position.
        //----------------------------------------------------------
        void SetBlendlinePosition(f32 infBlendlinePosition);
        //----------------------------------------------------------
        /// Set Playback Type
        ///
        /// Sets the current playback type, looping, etc.
        ///
        /// @param The playback type.
        //----------------------------------------------------------
        void SetPlaybackType(AnimationPlaybackType inePlaybackType);
        //----------------------------------------------------------
        /// Set Playback Speed Multiplier
        ///
        /// Sets the current playback speed multiplier.
        ///
        /// @param The playback speed multiplier.
        //----------------------------------------------------------
        void SetPlaybackSpeedMultiplier(f32 infSpeedMultiplier);
        //----------------------------------------------------------
        /// Set Playback Position
        ///
        /// Sets the current position through the animation.
        ///
        /// @param The position
        //----------------------------------------------------------
        void SetPlaybackPosition(f32 infPosition);
        //----------------------------------------------------------
        /// Set Playback Position Normalised
        ///
        /// Sets the current position through the animation normalised
        /// to between 0.0 and 1.0.
        ///
        /// @param The position normalised.
        //----------------------------------------------------------
        void SetPlaybackPositionNormalised(f32 infPosition);
        //----------------------------------------------------------
        /// Set Blend Type
        ///
        /// Sets the type of blending to be used when blending
        /// animations.
        ///
        /// @param The blend type.
        //----------------------------------------------------------
        void SetBlendType(AnimationBlendType ineBlendType);
        //----------------------------------------------------------
        /// Get Animation Changed Event
        ///
        /// @param The animation changed event. This is invoked every
        /// time the current animation group changes, e.g Fade Out
        /// or Clear Animations is called.
        //----------------------------------------------------------
        AnimationChangedEvent& GetAnimationChangedEvent();
        //----------------------------------------------------------
        /// Get Animation Completion Event
        ///
        /// @param The animation completion event. This is invoked
        /// whenever a non-looping animation reaches it's end.
        //----------------------------------------------------------
        AnimationCompletionEvent& GetAnimationCompletionEvent();
        //----------------------------------------------------------
        /// Get Animation Looped Event
        ///
        /// @param The animation looped event. This is invoked every
        /// Time an animation loops back to the start of an animation.
        //----------------------------------------------------------
        AnimationLoopedEvent& GetAnimationLoopedEvent();
        //----------------------------------------------------------
        /// Get Blendline Position
        ///
        /// @return the current position along the blendline.
        //----------------------------------------------------------
        f32 GetBlendlinePosition() const;
        //----------------------------------------------------------
        /// Get Playback Type
        ///
        /// @return the current playback type.
        //----------------------------------------------------------
        AnimationPlaybackType GetPlaybackType() const;
        //----------------------------------------------------------
        /// Get Animation Length
        ///
        /// @return the animation length in seconds.
        //----------------------------------------------------------
        f32 GetAnimationLength();
        //----------------------------------------------------------
        /// Get Playback Speed Multiplier
        ///
        /// @return the multiplier that is curently applied to the
        ///         playback speed.
        //----------------------------------------------------------
        f32 GetPlaybackSpeedMultiplier() const;
        //----------------------------------------------------------
        /// Get Playback Position
        ///
        /// @return the number of seconds into the animation.
        //----------------------------------------------------------
        f32 GetPlaybackPosition() const;
        //----------------------------------------------------------
        /// get Playback Position Normalised
        ///
        /// @return distance through the animation normalised to
        ///         between 0 and 1.
        //----------------------------------------------------------
        f32 GetPlaybackPositionNormalised();
        //----------------------------------------------------------
        /// Get Blend Type
        ///
        /// @return the blend type used for all animation blending.
        //----------------------------------------------------------
        AnimationBlendType GetBlendType() const;
        //----------------------------------------------------------
        /// Has Finished
        ///
        /// @return whether or not the animation has finished.
        //----------------------------------------------------------
        bool HasFinished() const;
        //-----------------------------------------------------
        /// Set Shadow Casting Enabled
        ///
        /// @param Whether the render component casts shadows
        //-----------------------------------------------------
        void SetShadowCastingEnabled(bool inbEnabled);
        //-----------------------------------------------------
        /// Is Shadow Casting Enabled
        ///
        /// @return Whether the render component casts shadows
        //-----------------------------------------------------
        bool IsShadowCastingEnabled() const;
        
    private:
        //----------------------------------------------------------
        /// Update Animation
        ///
        /// Updates the animation, rebuilding the animation matrices.
        ///
        /// @param The delta time.
        //----------------------------------------------------------
        void UpdateAnimation(f32 infDeltaTime);
        //----------------------------------------------------------
        /// Update Animation Timer
        ///
        /// Updates the animation timer.
        ///
        /// @param The delta time.
        //----------------------------------------------------------
        void UpdateAnimationTimer(f32 infDeltaTime);
        //----------------------------------------------------------
        /// Update Attached Entities
        ///
        /// Update the transforms of all entities attached to this
        /// animated mesh components skeleton.
        //----------------------------------------------------------
        void UpdateAttachedEntities();
        //----------------------------------------------------------
        /// Reset
        ///
        /// Resets the component back to a state where it is ready
        /// to start a new animation.
        //----------------------------------------------------------
        void Reset();
        //----------------------------------------------------------
        /// Triggered when the component is added to the scene.
        ///
        /// @author Ian Copland
        //----------------------------------------------------------
        void OnAddedToScene() override;
        //----------------------------------------------------------
        /// Update
        ///
        /// Updates the animation.
        ///
        /// @param The delta time.
        //----------------------------------------------------------
        void OnUpdate(f32 infDeltaTime) override;
        //----------------------------------------------------------
        /// Triggered when the component is removed to the scene.
        ///
        /// @author Ian Copland
        //----------------------------------------------------------
        void OnRemovedFromScene() override;
        
    private:
        typedef std::vector<std::pair<EntityWPtr, s32> > AttachedEntityList;
        AttachedEntityList maAttachedEntities;
        ModelCSPtr mpModel;
        std::vector<MaterialCSPtr> mMaterials;
        SkinnedAnimationGroupSPtr mActiveAnimationGroup;
        SkinnedAnimationGroupSPtr mFadingAnimationGroup;
        f32 mfPlaybackPosition;
        f32 mfPlaybackSpeedMultiplier;
        f32 mfBlendlinePosition;
        AnimationBlendType meBlendType;
        AnimationPlaybackType mePlaybackType;
        AnimationBlendType meFadeType;
        f32 mfFadeTimer;
        f32 mfFadeMaxTime;
        f32 mfFadePlaybackPosition;
        f32 mfFadeBlendlinePosition;
        bool mbFinished;
        bool mbAnimationDataDirty;
        AnimationCompletionEvent mAnimationCompletionEvent;
        AnimationLoopedEvent mAnimationLoopedEvent;
        AnimationChangedEvent mAnimationChangedEvent;
        
        AABB mBoundingBox;
        OOBB mOBBoundingBox;
        Sphere mBoundingSphere;
        bool m_shadowCastingEnabled = true;
        bool m_isVisible = true;
    };
}

#endif
