/*
 *  StaticMeshComponent.h
 *  moFlo
 *
 *  Created by Scott Downie on 07/10/2010.
 *  Copyright 2010 Tag Games. All rights reserved.
 *
 */

#ifndef _MO_FLO_RENDERING_STATIC_MESH_COMPONENT_H_
#define _MO_FLO_RENDERING_STATIC_MESH_COMPONENT_H_

#include <ChilliSource/ChilliSource.h>
#include <ChilliSource/Rendering/Base/RenderComponent.h>
#include <ChilliSource/Rendering/Model/Mesh.h>

namespace ChilliSource
{
	namespace Rendering
	{
		//===============================================================
		/// Description:
		///
		/// A static mesh component. This defines a 3D mesh that can
		/// be manipulated, textured but not animated.
		//===============================================================
		class StaticMeshComponent : public RenderComponent
		{
		public:
			CS_DECLARE_NAMEDTYPE(StaticMeshComponent);
			
            StaticMeshComponent();
			//----------------------------------------------------------
			/// Is A
			///
			/// Returns if it is of the type given
			/// @param Comparison Type
			/// @return Whether the class matches the comparison type
			//----------------------------------------------------------
			bool IsA(Core::InterfaceIDType inInterfaceID) const override;
			//----------------------------------------------------
			/// Get Axis Aligned Bounding Box
			///
			/// All render components have a box used for culling
			/// and coarse intersections. This is cached and 
			/// recomputed when required.
			/// @return Axis aligned bounding box
			//----------------------------------------------------
			const Core::AABB& GetAABB() override;
			//----------------------------------------------------
			/// Get Object Oriented Bounding Box
			///
			/// All render objects have an OOBB for
			/// picking. This is cached and 
			/// recomputed when required.
			/// @return OOBB
			//----------------------------------------------------
			const Core::OOBB& GetOOBB() override;
			//----------------------------------------------------
			/// Get Bounding Sphere
			///
			/// All render objects have an bounding sphere for
			/// culling. This is cached and 
			/// recomputed when required.
			/// @return bounding sphere
			//----------------------------------------------------
			const Core::Sphere& GetBoundingSphere() override;
			//-----------------------------------------------------------
			/// Is Transparent
			///
			/// Returns whether or not this component has any transparency
			///
			/// @return whether or not this has transparency
			//-----------------------------------------------------------
			bool IsTransparent() override;
			//-----------------------------------------------------------
			/// Set Material
			///
			/// Set the material that the mesh will use. Applies the material
			/// To all submeshes
			///
			/// @param Handle to material
			//-----------------------------------------------------------
			void SetMaterial(const MaterialCSPtr& inpMaterial) override;
			//-----------------------------------------------------------
			/// Set Material For Sub Mesh
			///
			/// Set the material that one sub mesh will use.
			///
			/// @param Handle to material
			/// @Param Index to the submesh
			//-----------------------------------------------------------
			void SetMaterialForSubMesh(const MaterialCSPtr& inpMaterial, u32 indwSubMeshIndex);
            //-----------------------------------------------------------
			/// Set Material For Sub Mesh
			///
			/// Set the material that one sub mesh will use.
			///
			/// @param Handle to material
			/// @param The name of the submesh.
			//-----------------------------------------------------------
			void SetMaterialForSubMesh(const MaterialCSPtr& inpMaterial, const std::string& instrSubMeshName);
			//-----------------------------------------------------------
			/// Get Material Of Sub Mesh
			///
			/// Get the material of a single sub mesh.
			///
			/// @param Index to the sub mesh
			/// @return Handle to material
			//-----------------------------------------------------------
			MaterialCSPtr GetMaterialOfSubMesh(u32 indwSubMeshIndex) const;
            //-----------------------------------------------------------
			/// Get Material Of Sub Mesh
			///
			/// Get the material of a single sub mesh.
			///
			/// @param The name of the submesh.
			/// @return Handle to material
			//-----------------------------------------------------------
			MaterialCSPtr GetMaterialOfSubMesh(const std::string& instrSubMeshName) const;
			//----------------------------------------------------------
			/// Attach Mesh
			///
			/// Attach a mesh to this component
			/// @param Mesh object
			//----------------------------------------------------------
			void AttachMesh(const MeshCSPtr& inpModel);
            //----------------------------------------------------------
			/// Attach Mesh
			///
			/// Attach a mesh to this component but uses the given 
            /// material
			/// @param Mesh object
			//----------------------------------------------------------
            void AttachMesh(const MeshCSPtr& inpModel, const MaterialCSPtr& inpMaterial);
            //----------------------------------------------------------
            /// Get Mesh
            ///
            /// @return The components internal mesh
            //----------------------------------------------------------
            const MeshCSPtr& GetMesh() const;
			
		private:
			//----------------------------------------------------------
			/// Render
			///
			/// NotifyConnections render on objects mesh
            ///
            /// @param Render system
            /// @param Active camera component
            /// @param The current shader pass.
			//----------------------------------------------------------
			void Render(RenderSystem* inpRenderSystem, CameraComponent* inpCam, ShaderPass ineShaderPass) override;
            //-----------------------------------------------------
            /// Render Shadow Map
            ///
            /// Render the mesh to the shadow map
            ///
            /// @param Render system
            /// @param Active camera component
            //-----------------------------------------------------
            void RenderShadowMap(RenderSystem* inpRenderSystem, CameraComponent* inpCam) override;
            //----------------------------------------------------
            /// Triggered when the component is attached to
            /// an entity on the scene
            ///
            /// @author S Downie
            //----------------------------------------------------
            void OnAddedToScene() override;
            //----------------------------------------------------
            /// On Entity Transform Changed
            ///
            /// Delegate called when the owning entities transform
            /// changes. This is used to dirty the bounding volumes
            //----------------------------------------------------
            void OnEntityTransformChanged();
            //----------------------------------------------------
            /// Triggered when the component is removed from
            /// an entity on the scene
            ///
            /// @author S Downie
            //----------------------------------------------------
            void OnRemovedFromScene() override;
            
		private:
			
			MeshCSPtr mpModel;
			std::vector<MaterialCSPtr> mMaterials;
            
            Core::ConnectionUPtr m_transformChangedConnection;
            
            bool mbBoundingSphereValid;
            bool mbAABBValid;
            bool mbOOBBValid;
            
            static MaterialCSPtr mspShadowMapMaterial;
		};
	}
}

#endif
