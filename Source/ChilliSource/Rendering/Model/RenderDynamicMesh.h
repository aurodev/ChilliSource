//
//  The MIT License (MIT)
//
//  Copyright (c) 2016 Tag Games Limited
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

#ifndef _CHILLISOURCE_RENDERING_MODEL_RENDERDYNAMICMESH_H_
#define _CHILLISOURCE_RENDERING_MODEL_RENDERDYNAMICMESH_H_

#include <ChilliSource/ChilliSource.h>
#include <ChilliSource/Core/Base/ByteBuffer.h>
#include <ChilliSource/Core/Math/Geometry/Shapes.h>
#include <ChilliSource/Rendering/Model/VertexFormat.h>

namespace ChilliSource
{
    /// A container for information which the renderer needs pertaining to a single dynamic
    /// mesh. This data is stored here, rather than in graphics memory, due to it's temporary
    /// nature.
    ///
    /// This is immutable and therefore thread-safe.
    ///
    class RenderDynamicMesh final
    {
    public:
        CS_DECLARE_NOCOPY(RenderDynamicMesh);
        
        /// Creates a new instance with the given mesh description data.
        ///
        /// @param polygonType
        ///     The type of polygon the mesh uses.
        /// @param vertexFormat
        ///     The vertex format.
        /// @param indexFormat
        ///     The type of index.
        /// @param numVertices
        ///     The number of vertices in the mesh. The maximum number is determined by the type of index.
        /// @param numIndices
        ///     The number of indices in the mesh.
        /// @param boundingSphere
        ///     A local space sphere enclosing the mesh.
        /// @param vertexData
        ///     The vertex data.
        /// @param indexData
        ///     The index data.
        ///
        RenderDynamicMesh(PolygonType polygonType, const VertexFormat& vertexFormat, IndexFormat indexFormat, u32 numVertices, u32 numIndices, const Sphere& boundingSphere,
                          ByteBuffer vertexData, ByteBuffer indexData) noexcept;
        
        /// @return The type of polygon the mesh uses.
        ///
        PolygonType GetPolygonType() const noexcept { return m_polygonType; }
        
        /// @return The vertex format.
        ///
        const VertexFormat& GetVertexFormat() const noexcept { return m_vertexFormat; }
        
        /// @return The type of index.
        ///
        IndexFormat GetIndexFormat() const noexcept { return m_indexFormat; }
        
        /// @return The number of vertices in the mesh. The maximum number is determined by the type of index.
        ///
        u32 GetNumVertices() const noexcept { return m_numVertices; }
        
        /// @return The number of indices in the mesh.
        ///
        u32 GetNumIndices() const noexcept { return m_numIndices; }
        
        /// @return A local space sphere enclosing the mesh.
        ///
        const Sphere& GetBoundingSphere() const noexcept { return m_boundingSphere; }
        
        /// @return The vertex data buffer.
        ///
        const ByteBuffer& GetVertexData() const noexcept { return m_vertexData; }
        
        /// @return The index data buffer.
        ///
        const ByteBuffer& GetIndexData() const noexcept { return m_indexData; }
        
    private:
        friend class RenderMeshManager;
        
        PolygonType m_polygonType;
        VertexFormat m_vertexFormat;
        IndexFormat m_indexFormat;
        u32 m_numVertices;
        u32 m_numIndices;
        Sphere m_boundingSphere;
        ByteBuffer m_vertexData;
        ByteBuffer m_indexData;
    };
}

#endif
