//
//  The MIT License (MIT)
//
//  Copyright (c) 2010 Tag Games Limited
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

#include <ChilliSource/Rendering/Base/RenderPassObject.h>

#include <ChilliSource/Rendering/Material/RenderMaterial.h>
#include <ChilliSource/Rendering/Model/RenderMesh.h>

namespace ChilliSource
{
    //------------------------------------------------------------------------------
    RenderPassObject::RenderPassObject(const RenderMaterial* renderMaterial, const RenderMesh* renderMesh, const Matrix4& worldMatrix, const Sphere& boundingSphere) noexcept
        : m_type(Type::k_static), m_renderMaterial(renderMaterial), m_renderMesh(renderMesh), m_worldMatrix(worldMatrix), m_boundingSphere(boundingSphere)
    { 
    }
    
    //------------------------------------------------------------------------------
    RenderPassObject::RenderPassObject(const RenderMaterial* renderMaterial, const RenderDynamicMesh* renderDynamicMesh, const Matrix4& worldMatrix, const Sphere& boundingSphere) noexcept
        : m_type(Type::k_dynamic), m_renderMaterial(renderMaterial), m_renderDynamicMesh(renderDynamicMesh), m_worldMatrix(worldMatrix), m_boundingSphere(boundingSphere)
    {
    }
}