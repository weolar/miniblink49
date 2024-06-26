//
// Copyright (c) 2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// DynamicHLSL.cpp: Implementation for link and run-time HLSL generation
//

#include "libANGLE/renderer/d3d/DynamicHLSL.h"

#include "common/utilities.h"
#include "compiler/translator/blocklayoutHLSL.h"
#include "libANGLE/Program.h"
#include "libANGLE/Shader.h"
#include "libANGLE/formatutils.h"
#include "libANGLE/renderer/d3d/RendererD3D.h"
#include "libANGLE/renderer/d3d/ShaderD3D.h"

// For use with ArrayString, see angleutils.h
static_assert(GL_INVALID_INDEX == UINT_MAX,
              "GL_INVALID_INDEX must be equal to the max unsigned int.");

using namespace gl;

namespace rx
{

namespace
{

std::string HLSLComponentTypeString(GLenum componentType)
{
    switch (componentType)
    {
        case GL_UNSIGNED_INT:
            return "uint";
        case GL_INT:
            return "int";
        case GL_UNSIGNED_NORMALIZED:
        case GL_SIGNED_NORMALIZED:
        case GL_FLOAT:
            return "float";
        default:
            UNREACHABLE();
            return "not-component-type";
    }
}

std::string HLSLComponentTypeString(GLenum componentType, int componentCount)
{
    return HLSLComponentTypeString(componentType) + (componentCount > 1 ? Str(componentCount) : "");
}

std::string HLSLMatrixTypeString(GLenum type)
{
    switch (type)
    {
        case GL_FLOAT_MAT2:
            return "float2x2";
        case GL_FLOAT_MAT3:
            return "float3x3";
        case GL_FLOAT_MAT4:
            return "float4x4";
        case GL_FLOAT_MAT2x3:
            return "float2x3";
        case GL_FLOAT_MAT3x2:
            return "float3x2";
        case GL_FLOAT_MAT2x4:
            return "float2x4";
        case GL_FLOAT_MAT4x2:
            return "float4x2";
        case GL_FLOAT_MAT3x4:
            return "float3x4";
        case GL_FLOAT_MAT4x3:
            return "float4x3";
        default:
            UNREACHABLE();
            return "not-matrix-type";
    }
}

std::string HLSLTypeString(GLenum type)
{
    if (gl::IsMatrixType(type))
    {
        return HLSLMatrixTypeString(type);
    }

    return HLSLComponentTypeString(gl::VariableComponentType(type),
                                   gl::VariableComponentCount(type));
}

const PixelShaderOutputVariable *FindOutputAtLocation(
    const std::vector<PixelShaderOutputVariable> &outputVariables,
    unsigned int location)
{
    for (size_t variableIndex = 0; variableIndex < outputVariables.size(); ++variableIndex)
    {
        if (outputVariables[variableIndex].outputIndex == location)
        {
            return &outputVariables[variableIndex];
        }
    }

    return nullptr;
}

typedef const PackedVarying *VaryingPacking[gl::IMPLEMENTATION_MAX_VARYING_VECTORS][4];

bool PackVarying(PackedVarying *packedVarying, const int maxVaryingVectors, VaryingPacking &packing)
{
    // Make sure we use transposed matrix types to count registers correctly.
    int registers = 0;
    int elements  = 0;

    const sh::Varying &varying = *packedVarying->varying;

    if (varying.isStruct())
    {
        registers = HLSLVariableRegisterCount(varying, true) * varying.elementCount();
        elements  = 4;
    }
    else
    {
        GLenum transposedType = TransposeMatrixType(varying.type);
        registers             = VariableRowCount(transposedType) * varying.elementCount();
        elements              = VariableColumnCount(transposedType);
    }

    if (elements >= 2 && elements <= 4)
    {
        for (int r = 0; r <= maxVaryingVectors - registers; r++)
        {
            bool available = true;

            for (int y = 0; y < registers && available; y++)
            {
                for (int x = 0; x < elements && available; x++)
                {
                    if (packing[r + y][x])
                    {
                        available = false;
                    }
                }
            }

            if (available)
            {
                packedVarying->registerIndex = r;
                packedVarying->columnIndex   = 0;

                for (int y = 0; y < registers; y++)
                {
                    for (int x = 0; x < elements; x++)
                    {
                        packing[r + y][x] = packedVarying;
                    }
                }

                return true;
            }
        }

        if (elements == 2)
        {
            for (int r = maxVaryingVectors - registers; r >= 0; r--)
            {
                bool available = true;

                for (int y = 0; y < registers && available; y++)
                {
                    for (int x = 2; x < 4 && available; x++)
                    {
                        if (packing[r + y][x])
                        {
                            available = false;
                        }
                    }
                }

                if (available)
                {
                    packedVarying->registerIndex = r;
                    packedVarying->columnIndex   = 2;

                    for (int y = 0; y < registers; y++)
                    {
                        for (int x = 2; x < 4; x++)
                        {
                            packing[r + y][x] = packedVarying;
                        }
                    }

                    return true;
                }
            }
        }
    }
    else if (elements == 1)
    {
        int space[4] = {0};

        for (int y = 0; y < maxVaryingVectors; y++)
        {
            for (int x = 0; x < 4; x++)
            {
                space[x] += packing[y][x] ? 0 : 1;
            }
        }

        int column = 0;

        for (int x = 0; x < 4; x++)
        {
            if (space[x] >= registers && (space[column] < registers || space[x] < space[column]))
            {
                column = x;
            }
        }

        if (space[column] >= registers)
        {
            for (int r = 0; r < maxVaryingVectors; r++)
            {
                if (!packing[r][column])
                {
                    packedVarying->registerIndex = r;
                    packedVarying->columnIndex   = column;

                    for (int y = r; y < r + registers; y++)
                    {
                        packing[y][column] = packedVarying;
                    }

                    break;
                }
            }

            return true;
        }
    }
    else
        UNREACHABLE();

    return false;
}

unsigned int VaryingRegisterIndex(const gl::Caps &caps,
                                  const PackedVarying &packedVarying,
                                  unsigned int elementIndex,
                                  unsigned int row)
{
    const sh::Varying &varying = *packedVarying.varying;

    GLenum transposedType = TransposeMatrixType(varying.type);
    unsigned int variableRows =
        static_cast<unsigned int>(varying.isStruct() ? 1 : VariableRowCount(transposedType));

    return (elementIndex * variableRows + (packedVarying.columnIndex * caps.maxVaryingVectors) +
            (packedVarying.registerIndex + row));
}

struct PackedVaryingRegister final
{
    PackedVaryingRegister() : varyingIndex(0), elementIndex(0), rowIndex(0) {}

    PackedVaryingRegister(const PackedVaryingRegister &) = default;
    PackedVaryingRegister &operator=(const PackedVaryingRegister &) = default;

    unsigned int registerIndex(const gl::Caps &caps,
                               const std::vector<PackedVarying> &packedVaryings) const
    {
        const PackedVarying &packedVarying = packedVaryings[varyingIndex];
        return VaryingRegisterIndex(caps, packedVarying, elementIndex, rowIndex);
    }

    size_t varyingIndex;
    unsigned int elementIndex;
    unsigned int rowIndex;
};

class PackedVaryingIterator final : public angle::NonCopyable
{
  public:
    PackedVaryingIterator(const std::vector<PackedVarying> &packedVaryings);

    class Iterator final
    {
      public:
        Iterator(const PackedVaryingIterator &parent);

        Iterator(const Iterator &) = default;
        Iterator &operator=(const Iterator &) = delete;

        Iterator &operator++();
        bool operator==(const Iterator &other) const;
        bool operator!=(const Iterator &other) const;

        const PackedVaryingRegister &operator*() const { return mRegister; }
        void setEnd() { mRegister.varyingIndex = mParent.mPackedVaryings.size(); }

      private:
        const PackedVaryingIterator &mParent;
        PackedVaryingRegister mRegister;
    };

    Iterator begin() const;
    const Iterator &end() const;

  private:
    const std::vector<PackedVarying> &mPackedVaryings;
    Iterator mEnd;
};

PackedVaryingIterator::PackedVaryingIterator(const std::vector<PackedVarying> &packedVaryings)
    : mPackedVaryings(packedVaryings), mEnd(*this)
{
    mEnd.setEnd();
}

PackedVaryingIterator::Iterator PackedVaryingIterator::begin() const
{
    return Iterator(*this);
}

const PackedVaryingIterator::Iterator &PackedVaryingIterator::end() const
{
    return mEnd;
}

PackedVaryingIterator::Iterator::Iterator(const PackedVaryingIterator &parent) : mParent(parent)
{
    while (mRegister.varyingIndex < mParent.mPackedVaryings.size() &&
           !mParent.mPackedVaryings[mRegister.varyingIndex].registerAssigned())
    {
        ++mRegister.varyingIndex;
    }
}

PackedVaryingIterator::Iterator &PackedVaryingIterator::Iterator::operator++()
{
    const sh::Varying *varying = mParent.mPackedVaryings[mRegister.varyingIndex].varying;
    GLenum transposedType = TransposeMatrixType(varying->type);
    unsigned int variableRows =
        static_cast<unsigned int>(varying->isStruct() ? 1 : VariableRowCount(transposedType));

    // Innermost iteration: row count
    if (mRegister.rowIndex + 1 < variableRows)
    {
        ++mRegister.rowIndex;
        return *this;
    }

    mRegister.rowIndex = 0;

    // Middle iteration: element count
    if (mRegister.elementIndex + 1 < varying->elementCount())
    {
        ++mRegister.elementIndex;
        return *this;
    }

    mRegister.elementIndex = 0;

    // Outer iteration: the varying itself. Once we pass the last varying, this Iterator will
    // equal the end Iterator.
    do
    {
        ++mRegister.varyingIndex;
    } while (mRegister.varyingIndex < mParent.mPackedVaryings.size() &&
             !mParent.mPackedVaryings[mRegister.varyingIndex].registerAssigned());
    return *this;
}

bool PackedVaryingIterator::Iterator::operator==(const Iterator &other) const
{
    return mRegister.elementIndex == other.mRegister.elementIndex &&
           mRegister.rowIndex == other.mRegister.rowIndex &&
           mRegister.varyingIndex == other.mRegister.varyingIndex;
}

bool PackedVaryingIterator::Iterator::operator!=(const Iterator &other) const
{
    return !(*this == other);
}

const std::string VERTEX_ATTRIBUTE_STUB_STRING = "@@ VERTEX ATTRIBUTES @@";
const std::string PIXEL_OUTPUT_STUB_STRING     = "@@ PIXEL OUTPUT @@";
}  // anonymous namespace

DynamicHLSL::DynamicHLSL(RendererD3D *const renderer) : mRenderer(renderer)
{
}

// Packs varyings into generic varying registers, using the algorithm from [OpenGL ES Shading
// Language 1.00 rev. 17] appendix A section 7 page 111
// Returns the number of used varying registers, or -1 if unsuccesful
bool DynamicHLSL::packVaryings(const gl::Caps &caps,
                               InfoLog &infoLog,
                               std::vector<PackedVarying> *packedVaryings,
                               const std::vector<std::string> &transformFeedbackVaryings,
                               unsigned int *registerCountOut)
{
    VaryingPacking packing = {};
    *registerCountOut      = 0;

    std::set<std::string> uniqueVaryingNames;

    for (PackedVarying &packedVarying : *packedVaryings)
    {
        const sh::Varying &varying = *packedVarying.varying;

        // Do not assign registers to built-in or unreferenced varyings
        if (varying.isBuiltIn() || !varying.staticUse)
        {
            continue;
        }

        ASSERT(uniqueVaryingNames.count(varying.name) == 0);

        if (PackVarying(&packedVarying, caps.maxVaryingVectors, packing))
        {
            uniqueVaryingNames.insert(varying.name);
        }
        else
        {
            infoLog << "Could not pack varying " << varying.name;
            return false;
        }
    }

    for (const std::string &transformFeedbackVaryingName : transformFeedbackVaryings)
    {
        if (transformFeedbackVaryingName.compare(0, 3, "gl_") == 0)
        {
            // do not pack builtin XFB varyings
            continue;
        }

        for (PackedVarying &packedVarying : *packedVaryings)
        {
            const sh::Varying &varying = *packedVarying.varying;

            // Make sure transform feedback varyings aren't optimized out.
            if (uniqueVaryingNames.count(transformFeedbackVaryingName) == 0)
            {
                bool found = false;
                if (transformFeedbackVaryingName == varying.name)
                {
                    if (!PackVarying(&packedVarying, caps.maxVaryingVectors, packing))
                    {
                        infoLog << "Could not pack varying " << varying.name;
                        return false;
                    }

                    found = true;
                    break;
                }
                if (!found)
                {
                    infoLog << "Transform feedback varying " << transformFeedbackVaryingName
                            << " does not exist in the vertex shader.";
                    return false;
                }
            }

            // Add duplicate transform feedback varyings for 'flat' shaded attributes. This is
            // necessary because we write out modified vertex data to correct for the provoking
            // vertex in D3D11. This extra transform feedback varying is the unmodified stream.
            if (varying.interpolation == sh::INTERPOLATION_FLAT)
            {
                sh::Varying duplicateVarying(varying);
                duplicateVarying.name = "StreamOut_" + duplicateVarying.name;
            }
        }
    }

    // Return the number of used registers
    for (GLuint r = 0; r < caps.maxVaryingVectors; r++)
    {
        if (packing[r][0] || packing[r][1] || packing[r][2] || packing[r][3])
        {
            (*registerCountOut)++;
        }
    }

    return true;
}

void DynamicHLSL::generateVaryingHLSL(const gl::Caps &caps,
                                      const std::vector<PackedVarying> &varyings,
                                      bool programUsesPointSize,
                                      std::stringstream &hlslStream) const
{
    std::string varyingSemantic = getVaryingSemantic(programUsesPointSize);

    for (const PackedVaryingRegister &registerInfo : PackedVaryingIterator(varyings))
    {
        const PackedVarying &packedVarying = varyings[registerInfo.varyingIndex];
        const sh::Varying &varying = *packedVarying.varying;
        GLenum transposedType              = gl::TransposeMatrixType(varying.type);
        unsigned int registerIndex         = registerInfo.registerIndex(caps, varyings);

        // TODO: Add checks to ensure D3D interpolation modifiers don't result in too many
        // registers being used.
        // For example, if there are N registers, and we have N vec3 varyings and 1 float
        // varying, then D3D will pack them into N registers.
        // If the float varying has the 'nointerpolation' modifier on it then we would need
        // N + 1 registers, and D3D compilation will fail.

        switch (varying.interpolation)
        {
            case sh::INTERPOLATION_SMOOTH:
                hlslStream << "    ";
                break;
            case sh::INTERPOLATION_FLAT:
                hlslStream << "    nointerpolation ";
                break;
            case sh::INTERPOLATION_CENTROID:
                hlslStream << "    centroid ";
                break;
            default:
                UNREACHABLE();
        }

        if (varying.isStruct())
        {
            // TODO(jmadill): pass back translated name from the shader translator
            hlslStream << decorateVariable(varying.structName);
        }
        else
        {
            GLenum componentType = VariableComponentType(transposedType);
            int columnCount = VariableColumnCount(transposedType);
            hlslStream << HLSLComponentTypeString(componentType, columnCount);
        }

        hlslStream << " v" << registerIndex << " : " << varyingSemantic << registerIndex << ";\n";
    }
}

std::string DynamicHLSL::generateVertexShaderForInputLayout(
    const std::string &sourceShader,
    const InputLayout &inputLayout,
    const std::vector<sh::Attribute> &shaderAttributes) const
{
    std::stringstream structStream;
    std::stringstream initStream;

    structStream << "struct VS_INPUT\n"
                 << "{\n";

    int semanticIndex       = 0;
    unsigned int inputIndex = 0;

    // If gl_PointSize is used in the shader then pointsprites rendering is expected.
    // If the renderer does not support Geometry shaders then Instanced PointSprite emulation
    // must be used.
    bool usesPointSize = sourceShader.find("GL_USES_POINT_SIZE") != std::string::npos;
    bool useInstancedPointSpriteEmulation =
        usesPointSize && mRenderer->getWorkarounds().useInstancedPointSpriteEmulation;

    // Instanced PointSprite emulation requires additional entries in the
    // VS_INPUT structure to support the vertices that make up the quad vertices.
    // These values must be in sync with the cooresponding values added during inputlayout creation
    // in InputLayoutCache::applyVertexBuffers().
    //
    // The additional entries must appear first in the VS_INPUT layout because
    // Windows Phone 8 era devices require per vertex data to physically come
    // before per instance data in the shader.
    if (useInstancedPointSpriteEmulation)
    {
        structStream << "    float3 spriteVertexPos : SPRITEPOSITION0;\n"
                     << "    float2 spriteTexCoord : SPRITETEXCOORD0;\n";
    }

    for (size_t attributeIndex = 0; attributeIndex < shaderAttributes.size(); ++attributeIndex)
    {
        const sh::Attribute &shaderAttribute = shaderAttributes[attributeIndex];
        if (!shaderAttribute.name.empty())
        {
            ASSERT(inputIndex < MAX_VERTEX_ATTRIBS);
            VertexFormatType vertexFormatType =
                inputIndex < inputLayout.size() ? inputLayout[inputIndex] : VERTEX_FORMAT_INVALID;

            // HLSL code for input structure
            if (IsMatrixType(shaderAttribute.type))
            {
                // Matrix types are always transposed
                structStream << "    "
                             << HLSLMatrixTypeString(TransposeMatrixType(shaderAttribute.type));
            }
            else
            {
                GLenum componentType = mRenderer->getVertexComponentType(vertexFormatType);

                if (shaderAttribute.name == "gl_InstanceID")
                {
                    // The input type of the instance ID in HLSL (uint) differs from the one in ESSL
                    // (int).
                    structStream << " uint";
                }
                else
                {
                    structStream << "    " << HLSLComponentTypeString(
                                                  componentType,
                                                  VariableComponentCount(shaderAttribute.type));
                }
            }

            structStream << " " << decorateVariable(shaderAttribute.name) << " : ";

            if (shaderAttribute.name == "gl_InstanceID")
            {
                structStream << "SV_InstanceID";
            }
            else
            {
                structStream << "TEXCOORD" << semanticIndex;
                semanticIndex += VariableRegisterCount(shaderAttribute.type);
            }

            structStream << ";\n";

            // HLSL code for initialization
            initStream << "    " << decorateVariable(shaderAttribute.name) << " = ";

            // Mismatched vertex attribute to vertex input may result in an undefined
            // data reinterpretation (eg for pure integer->float, float->pure integer)
            // TODO: issue warning with gl debug info extension, when supported
            if (IsMatrixType(shaderAttribute.type) ||
                (mRenderer->getVertexConversionType(vertexFormatType) & VERTEX_CONVERT_GPU) != 0)
            {
                initStream << generateAttributeConversionHLSL(vertexFormatType, shaderAttribute);
            }
            else
            {
                initStream << "input." << decorateVariable(shaderAttribute.name);
            }

            initStream << ";\n";

            inputIndex += VariableRowCount(TransposeMatrixType(shaderAttribute.type));
        }
    }

    structStream << "};\n"
                    "\n"
                    "void initAttributes(VS_INPUT input)\n"
                    "{\n"
                 << initStream.str() << "}\n";

    std::string vertexHLSL(sourceShader);

    size_t copyInsertionPos = vertexHLSL.find(VERTEX_ATTRIBUTE_STUB_STRING);
    vertexHLSL.replace(copyInsertionPos, VERTEX_ATTRIBUTE_STUB_STRING.length(), structStream.str());

    return vertexHLSL;
}

std::string DynamicHLSL::generatePixelShaderForOutputSignature(
    const std::string &sourceShader,
    const std::vector<PixelShaderOutputVariable> &outputVariables,
    bool usesFragDepth,
    const std::vector<GLenum> &outputLayout) const
{
    const int shaderModel      = mRenderer->getMajorShaderModel();
    std::string targetSemantic = (shaderModel >= 4) ? "SV_TARGET" : "COLOR";
    std::string depthSemantic  = (shaderModel >= 4) ? "SV_Depth" : "DEPTH";

    std::stringstream declarationStream;
    std::stringstream copyStream;

    declarationStream << "struct PS_OUTPUT\n"
                         "{\n";

    for (size_t layoutIndex = 0; layoutIndex < outputLayout.size(); ++layoutIndex)
    {
        GLenum binding = outputLayout[layoutIndex];

        if (binding != GL_NONE)
        {
            unsigned int location = (binding - GL_COLOR_ATTACHMENT0);

            const PixelShaderOutputVariable *outputVariable =
                FindOutputAtLocation(outputVariables, location);

            // OpenGL ES 3.0 spec $4.2.1
            // If [...] not all user-defined output variables are written, the values of fragment
            // colors
            // corresponding to unwritten variables are similarly undefined.
            if (outputVariable)
            {
                declarationStream << "    " + HLSLTypeString(outputVariable->type) << " "
                                  << outputVariable->name << " : " << targetSemantic
                                  << static_cast<int>(layoutIndex) << ";\n";

                copyStream << "    output." << outputVariable->name << " = "
                           << outputVariable->source << ";\n";
            }
        }
    }

    if (usesFragDepth)
    {
        declarationStream << "    float gl_Depth : " << depthSemantic << ";\n";
        copyStream << "    output.gl_Depth = gl_Depth; \n";
    }

    declarationStream << "};\n"
                         "\n"
                         "PS_OUTPUT generateOutput()\n"
                         "{\n"
                         "    PS_OUTPUT output;\n"
                      << copyStream.str() << "    return output;\n"
                                             "}\n";

    std::string pixelHLSL(sourceShader);

    size_t outputInsertionPos = pixelHLSL.find(PIXEL_OUTPUT_STUB_STRING);
    pixelHLSL.replace(outputInsertionPos, PIXEL_OUTPUT_STUB_STRING.length(),
                      declarationStream.str());

    return pixelHLSL;
}

std::string DynamicHLSL::getVaryingSemantic(bool programUsesPointSize) const
{
    // SM3 reserves the TEXCOORD semantic for point sprite texcoords (gl_PointCoord)
    // In D3D11 we manually compute gl_PointCoord in the GS.
    int shaderModel = mRenderer->getMajorShaderModel();
    return ((programUsesPointSize && shaderModel < 4) ? "COLOR" : "TEXCOORD");
}

struct DynamicHLSL::SemanticInfo
{
    struct BuiltinInfo
    {
        BuiltinInfo() : enabled(false), index(0), systemValue(false) {}

        bool enabled;
        std::string semantic;
        unsigned int index;
        bool systemValue;

        std::string str() const { return (systemValue ? semantic : (semantic + Str(index))); }

        void enableSystem(const std::string &systemValueSemantic)
        {
            enabled     = true;
            semantic    = systemValueSemantic;
            systemValue = true;
        }

        void enable(const std::string &semanticVal, unsigned int indexVal)
        {
            enabled  = true;
            semantic = semanticVal;
            index    = indexVal;
        }
    };

    BuiltinInfo dxPosition;
    BuiltinInfo glPosition;
    BuiltinInfo glFragCoord;
    BuiltinInfo glPointCoord;
    BuiltinInfo glPointSize;
};

DynamicHLSL::SemanticInfo DynamicHLSL::getSemanticInfo(ShaderType shaderType,
                                                       unsigned int startRegisters,
                                                       bool position,
                                                       bool fragCoord,
                                                       bool pointCoord,
                                                       bool pointSize) const
{
    SemanticInfo info;
    bool hlsl4                         = (mRenderer->getMajorShaderModel() >= 4);
    const std::string &varyingSemantic = getVaryingSemantic(pointSize);

    unsigned int reservedRegisterIndex = startRegisters;

    if (hlsl4)
    {
        info.dxPosition.enableSystem("SV_Position");
    }
    else if (shaderType == SHADER_PIXEL)
    {
        info.dxPosition.enableSystem("VPOS");
    }
    else
    {
        info.dxPosition.enableSystem("POSITION");
    }

    if (position)
    {
        info.glPosition.enable(varyingSemantic, reservedRegisterIndex++);
    }

    if (fragCoord)
    {
        info.glFragCoord.enable(varyingSemantic, reservedRegisterIndex++);
    }

    if (pointCoord)
    {
        // SM3 reserves the TEXCOORD semantic for point sprite texcoords (gl_PointCoord)
        // In D3D11 we manually compute gl_PointCoord in the GS.
        if (hlsl4)
        {
            info.glPointCoord.enable(varyingSemantic, reservedRegisterIndex++);
        }
        else
        {
            info.glPointCoord.enable("TEXCOORD", 0);
        }
    }

    // Special case: do not include PSIZE semantic in HLSL 3 pixel shaders
    if (pointSize && (shaderType != SHADER_PIXEL || hlsl4))
    {
        info.glPointSize.enableSystem("PSIZE");
    }

    return info;
}

void DynamicHLSL::generateVaryingLinkHLSL(const gl::Caps &caps,
                                          bool programUsesPointSize,
                                          const SemanticInfo &info,
                                          const std::vector<PackedVarying> &packedVaryings,
                                          std::stringstream &linkStream) const
{
    ASSERT(info.dxPosition.enabled);
    linkStream << "{\n"
               << "    float4 dx_Position : " << info.dxPosition.str() << ";\n";

    if (info.glPosition.enabled)
    {
        linkStream << "    float4 gl_Position : " << info.glPosition.str() << ";\n";
    }

    if (info.glFragCoord.enabled)
    {
        linkStream << "    float4 gl_FragCoord : " << info.glFragCoord.str() << ";\n";
    }

    if (info.glPointCoord.enabled)
    {
        linkStream << "    float2 gl_PointCoord : " << info.glPointCoord.str() << ";\n";
    }

    if (info.glPointSize.enabled)
    {
        linkStream << "    float gl_PointSize : " << info.glPointSize.str() << ";\n";
    }

    // Do this after glPointSize, to potentially combine gl_PointCoord and gl_PointSize into the
    // same register.
    generateVaryingHLSL(caps, packedVaryings, programUsesPointSize, linkStream);

    linkStream << "};\n";
}

void DynamicHLSL::storeBuiltinLinkedVaryings(const SemanticInfo &info,
                                             std::vector<LinkedVarying> *linkedVaryings) const
{
    if (info.glPosition.enabled)
    {
        linkedVaryings->push_back(LinkedVarying(
            "gl_Position", GL_FLOAT_VEC4, 1, info.glPosition.semantic, info.glPosition.index, 1));
    }

    if (info.glFragCoord.enabled)
    {
        linkedVaryings->push_back(LinkedVarying("gl_FragCoord", GL_FLOAT_VEC4, 1,
                                                info.glFragCoord.semantic, info.glFragCoord.index,
                                                1));
    }

    if (info.glPointSize.enabled)
    {
        linkedVaryings->push_back(LinkedVarying("gl_PointSize", GL_FLOAT, 1, "PSIZE", 0, 1));
    }
}

void DynamicHLSL::storeUserLinkedVaryings(const std::vector<PackedVarying> &packedVaryings,
                                          bool programUsesPointSize,
                                          std::vector<LinkedVarying> *linkedVaryings) const
{
    const std::string &varyingSemantic = getVaryingSemantic(programUsesPointSize);

    for (const PackedVarying &packedVarying : packedVaryings)
    {
        if (packedVarying.registerAssigned())
        {
            const sh::Varying &varying = *packedVarying.varying;

            ASSERT(!varying.isBuiltIn());
            GLenum transposedType = TransposeMatrixType(varying.type);
            int variableRows      = (varying.isStruct() ? 1 : VariableRowCount(transposedType));

            linkedVaryings->push_back(
                LinkedVarying(varying.name, varying.type, varying.elementCount(), varyingSemantic,
                              packedVarying.registerIndex, variableRows * varying.elementCount()));
        }
    }
}

bool DynamicHLSL::generateShaderLinkHLSL(const gl::Data &data,
                                         const gl::Program::Data &programData,
                                         InfoLog &infoLog,
                                         unsigned int registerCount,
                                         std::string *pixelHLSL,
                                         std::string *vertexHLSL,
                                         const std::vector<PackedVarying> &packedVaryings,
                                         std::vector<LinkedVarying> *linkedVaryings,
                                         std::vector<PixelShaderOutputVariable> *outPixelShaderKey,
                                         bool *outUsesFragDepth) const
{
    ASSERT(pixelHLSL->empty() && vertexHLSL->empty());

    const gl::Shader *vertexShaderGL   = programData.getAttachedVertexShader();
    const ShaderD3D *vertexShader      = GetImplAs<ShaderD3D>(vertexShaderGL);
    const gl::Shader *fragmentShaderGL = programData.getAttachedFragmentShader();
    const ShaderD3D *fragmentShader    = GetImplAs<ShaderD3D>(fragmentShaderGL);
    const int shaderModel              = mRenderer->getMajorShaderModel();

    bool usesMRT        = fragmentShader->usesMultipleRenderTargets();
    bool usesFragCoord  = fragmentShader->usesFragCoord();
    bool usesPointCoord = fragmentShader->usesPointCoord();
    bool usesPointSize = vertexShader->usesPointSize();
    bool useInstancedPointSpriteEmulation =
        usesPointSize && mRenderer->getWorkarounds().useInstancedPointSpriteEmulation;
    bool insertDummyPointCoordValue = !usesPointSize && usesPointCoord && shaderModel >= 4;
    bool addPointCoord =
        (useInstancedPointSpriteEmulation && usesPointCoord) || insertDummyPointCoordValue;

    // Validation done in the compiler
    ASSERT(!fragmentShader->usesFragColor() || !fragmentShader->usesFragData());

    // Write the HLSL input/output declarations
    const unsigned int registersNeeded =
        registerCount + (usesFragCoord ? 1u : 0u) + (usesPointCoord ? 1u : 0u);

    // Two cases when writing to gl_FragColor and using ESSL 1.0:
    // - with a 3.0 context, the output color is copied to channel 0
    // - with a 2.0 context, the output color is broadcast to all channels
    const bool broadcast                = (fragmentShader->usesFragColor() && data.clientVersion < 3);
    const unsigned int numRenderTargets = (broadcast || usesMRT ? data.caps->maxDrawBuffers : 1);

    // gl_Position only needs to be outputted from the vertex shader if transform feedback is
    // active. This isn't supported on D3D11 Feature Level 9_3, so we don't output gl_Position from
    // the vertex shader in this case. This saves us 1 output vector.
    bool outputPositionFromVS = !(shaderModel >= 4 && mRenderer->getShaderModelSuffix() != "");

    int shaderVersion = vertexShaderGL->getShaderVersion();

    if (static_cast<GLuint>(registersNeeded) > data.caps->maxVaryingVectors)
    {
        infoLog << "No varying registers left to support gl_FragCoord/gl_PointCoord";
        return false;
    }

    // Instanced PointSprite emulation requires that gl_PointCoord is present in the vertex shader
    // VS_OUTPUT structure to ensure compatibility with the generated PS_INPUT of the pixel shader.
    // GeometryShader PointSprite emulation does not require this additional entry because the
    // GS_OUTPUT of the Geometry shader contains the pointCoord value and already matches the
    // PS_INPUT of the generated pixel shader. The Geometry Shader point sprite implementation needs
    // gl_PointSize to be in VS_OUTPUT and GS_INPUT. Instanced point sprites doesn't need
    // gl_PointSize in VS_OUTPUT.
    const SemanticInfo &vertexSemantics =
        getSemanticInfo(SHADER_VERTEX, registerCount, outputPositionFromVS, usesFragCoord,
                        addPointCoord, (!useInstancedPointSpriteEmulation && usesPointSize));

    storeUserLinkedVaryings(packedVaryings, usesPointSize, linkedVaryings);
    storeBuiltinLinkedVaryings(vertexSemantics, linkedVaryings);

    std::stringstream vertexStream;
    vertexStream << vertexShaderGL->getTranslatedSource();

    // Instanced PointSprite emulation requires additional entries originally generated in the
    // GeometryShader HLSL. These include pointsize clamp values.
    if (useInstancedPointSpriteEmulation)
    {
        vertexStream << "static float minPointSize = "
                     << static_cast<int>(data.caps->minAliasedPointSize) << ".0f;\n"
                     << "static float maxPointSize = "
                     << static_cast<int>(data.caps->maxAliasedPointSize) << ".0f;\n";
    }

    // Add stub string to be replaced when shader is dynamically defined by its layout
    vertexStream << "\n" << VERTEX_ATTRIBUTE_STUB_STRING + "\n"
                 << "struct VS_OUTPUT\n";
    generateVaryingLinkHLSL(*data.caps, usesPointSize, vertexSemantics, packedVaryings,
                            vertexStream);
    vertexStream << "\n"
                 << "VS_OUTPUT main(VS_INPUT input)\n"
                 << "{\n"
                 << "    initAttributes(input);\n";

    if (vertexShader->usesDeferredInit())
    {
        vertexStream << "\n"
                     << "    initializeDeferredGlobals();\n";
    }

    vertexStream << "\n"
                 << "    gl_main();\n"
                 << "\n"
                 << "    VS_OUTPUT output;\n";

    if (outputPositionFromVS)
    {
        vertexStream << "    output.gl_Position = gl_Position;\n";
    }

    // On D3D9 or D3D11 Feature Level 9, we need to emulate large viewports using dx_ViewAdjust.
    if (shaderModel >= 4 && mRenderer->getShaderModelSuffix() == "")
    {
        vertexStream << "    output.dx_Position.x = gl_Position.x;\n"
                     << "    output.dx_Position.y = -gl_Position.y;\n"
                     << "    output.dx_Position.z = (gl_Position.z + gl_Position.w) * 0.5;\n"
                     << "    output.dx_Position.w = gl_Position.w;\n";
    }
    else
    {
        vertexStream << "    output.dx_Position.x = gl_Position.x * dx_ViewAdjust.z + "
                        "dx_ViewAdjust.x * gl_Position.w;\n"
                     << "    output.dx_Position.y = -(gl_Position.y * dx_ViewAdjust.w + "
                        "dx_ViewAdjust.y * gl_Position.w);\n"
                     << "    output.dx_Position.z = (gl_Position.z + gl_Position.w) * 0.5;\n"
                     << "    output.dx_Position.w = gl_Position.w;\n";
    }

    // We don't need to output gl_PointSize if we use are emulating point sprites via instancing.
    if (usesPointSize && shaderModel >= 3 && !useInstancedPointSpriteEmulation)
    {
        vertexStream << "    output.gl_PointSize = gl_PointSize;\n";
    }

    if (usesFragCoord)
    {
        vertexStream << "    output.gl_FragCoord = gl_Position;\n";
    }

    for (const PackedVaryingRegister &registerInfo : PackedVaryingIterator(packedVaryings))
    {
        const PackedVarying &packedVarying = packedVaryings[registerInfo.varyingIndex];
        const sh::Varying &varying = *packedVarying.varying;
        GLenum transposedType = TransposeMatrixType(varying.type);
        unsigned int variableRows =
            static_cast<unsigned int>(varying.isStruct() ? 1 : VariableRowCount(transposedType));

        int r = registerInfo.registerIndex(*data.caps, packedVaryings);
        vertexStream << "    output.v" << r << " = _" + varying.name;

        if (varying.isArray())
        {
            vertexStream << ArrayString(registerInfo.elementIndex);
        }

        if (variableRows > 1)
        {
            vertexStream << ArrayString(registerInfo.rowIndex);
        }

        vertexStream << ";\n";
    }

    // Instanced PointSprite emulation requires additional entries to calculate
    // the final output vertex positions of the quad that represents each sprite.
    if (useInstancedPointSpriteEmulation)
    {
        vertexStream << "\n"
                     << "    gl_PointSize = clamp(gl_PointSize, minPointSize, maxPointSize);\n"
                     << "    output.dx_Position.xyz += float3(input.spriteVertexPos.x * "
                        "gl_PointSize / (dx_ViewCoords.x*2), input.spriteVertexPos.y * "
                        "gl_PointSize / (dx_ViewCoords.y*2), input.spriteVertexPos.z) * "
                        "output.dx_Position.w;\n";

        if (usesPointCoord)
        {
            vertexStream << "\n"
                         << "    output.gl_PointCoord = input.spriteTexCoord;\n";
        }
    }

    // Renderers that enable instanced pointsprite emulation require the vertex shader output member
    // gl_PointCoord to be set to a default value if used without gl_PointSize. 0.5,0.5 is the same
    // default value used in the generated pixel shader.
    if (insertDummyPointCoordValue)
    {
        ASSERT(!useInstancedPointSpriteEmulation);
        vertexStream << "\n"
                     << "    output.gl_PointCoord = float2(0.5, 0.5);\n";
    }

    vertexStream << "\n"
                 << "    return output;\n"
                 << "}\n";

    std::stringstream pixelStream;
    pixelStream << fragmentShaderGL->getTranslatedSource();

    const SemanticInfo &pixelSemantics =
        getSemanticInfo(SHADER_PIXEL, registerCount, outputPositionFromVS, usesFragCoord,
                        usesPointCoord, (!useInstancedPointSpriteEmulation && usesPointSize));

    pixelStream << "struct PS_INPUT\n";
    generateVaryingLinkHLSL(*data.caps, usesPointSize, pixelSemantics, packedVaryings, pixelStream);
    pixelStream << "\n";

    if (shaderVersion < 300)
    {
        for (unsigned int renderTargetIndex = 0; renderTargetIndex < numRenderTargets;
             renderTargetIndex++)
        {
            PixelShaderOutputVariable outputKeyVariable;
            outputKeyVariable.type = GL_FLOAT_VEC4;
            outputKeyVariable.name = "gl_Color" + Str(renderTargetIndex);
            outputKeyVariable.source =
                broadcast ? "gl_Color[0]" : "gl_Color[" + Str(renderTargetIndex) + "]";
            outputKeyVariable.outputIndex = renderTargetIndex;

            outPixelShaderKey->push_back(outputKeyVariable);
        }

        *outUsesFragDepth = fragmentShader->usesFragDepth();
    }
    else
    {
        const auto &shaderOutputVars = fragmentShaderGL->getActiveOutputVariables();

        for (auto outputPair : programData.getOutputVariables())
        {
            const VariableLocation &outputLocation   = outputPair.second;
            const sh::ShaderVariable &outputVariable = shaderOutputVars[outputLocation.index];
            const std::string &variableName = "out_" + outputLocation.name;
            const std::string &elementString =
                (outputLocation.element == GL_INVALID_INDEX ? "" : Str(outputLocation.element));

            ASSERT(outputVariable.staticUse);

            PixelShaderOutputVariable outputKeyVariable;
            outputKeyVariable.type        = outputVariable.type;
            outputKeyVariable.name        = variableName + elementString;
            outputKeyVariable.source      = variableName + ArrayString(outputLocation.element);
            outputKeyVariable.outputIndex = outputPair.first;

            outPixelShaderKey->push_back(outputKeyVariable);
        }

        *outUsesFragDepth = false;
    }

    pixelStream << PIXEL_OUTPUT_STUB_STRING + "\n";

    if (fragmentShader->usesFrontFacing())
    {
        if (shaderModel >= 4)
        {
            pixelStream << "PS_OUTPUT main(PS_INPUT input, bool isFrontFace : SV_IsFrontFace)\n"
                        << "{\n";
        }
        else
        {
            pixelStream << "PS_OUTPUT main(PS_INPUT input, float vFace : VFACE)\n"
                        << "{\n";
        }
    }
    else
    {
        pixelStream << "PS_OUTPUT main(PS_INPUT input)\n"
                    << "{\n";
    }

    if (usesFragCoord)
    {
        pixelStream << "    float rhw = 1.0 / input.gl_FragCoord.w;\n";

        // Certain Shader Models (4_0+ and 3_0) allow reading from dx_Position in the pixel shader.
        // Other Shader Models (4_0_level_9_3 and 2_x) don't support this, so we emulate it using
        // dx_ViewCoords.
        if (shaderModel >= 4 && mRenderer->getShaderModelSuffix() == "")
        {
            pixelStream << "    gl_FragCoord.x = input.dx_Position.x;\n"
                        << "    gl_FragCoord.y = input.dx_Position.y;\n";
        }
        else if (shaderModel == 3)
        {
            pixelStream << "    gl_FragCoord.x = input.dx_Position.x + 0.5;\n"
                        << "    gl_FragCoord.y = input.dx_Position.y + 0.5;\n";
        }
        else
        {
            // dx_ViewCoords contains the viewport width/2, height/2, center.x and center.y. See
            // Renderer::setViewport()
            pixelStream << "    gl_FragCoord.x = (input.gl_FragCoord.x * rhw) * dx_ViewCoords.x + "
                           "dx_ViewCoords.z;\n"
                        << "    gl_FragCoord.y = (input.gl_FragCoord.y * rhw) * dx_ViewCoords.y + "
                           "dx_ViewCoords.w;\n";
        }

        pixelStream << "    gl_FragCoord.z = (input.gl_FragCoord.z * rhw) * dx_DepthFront.x + "
                       "dx_DepthFront.y;\n"
                    << "    gl_FragCoord.w = rhw;\n";
    }

    if (usesPointCoord && shaderModel >= 3)
    {
        pixelStream << "    gl_PointCoord.x = input.gl_PointCoord.x;\n"
                    << "    gl_PointCoord.y = 1.0 - input.gl_PointCoord.y;\n";
    }

    if (fragmentShader->usesFrontFacing())
    {
        if (shaderModel <= 3)
        {
            pixelStream << "    gl_FrontFacing = (vFace * dx_DepthFront.z >= 0.0);\n";
        }
        else
        {
            pixelStream << "    gl_FrontFacing = isFrontFace;\n";
        }
    }

    for (const PackedVaryingRegister &registerInfo : PackedVaryingIterator(packedVaryings))
    {
        const PackedVarying &packedVarying = packedVaryings[registerInfo.varyingIndex];
        const sh::Varying &varying = *packedVarying.varying;

        // Don't reference VS-only transform feedback varyings in the PS.
        if (packedVarying.vertexOnly)
            continue;

        ASSERT(!varying.isBuiltIn());
        GLenum transposedType      = TransposeMatrixType(varying.type);
        int variableRows           = (varying.isStruct() ? 1 : VariableRowCount(transposedType));
        unsigned int registerIndex = registerInfo.registerIndex(*data.caps, packedVaryings);
        pixelStream << "    _" << varying.name;

        if (varying.isArray())
        {
            pixelStream << ArrayString(registerInfo.elementIndex);
        }

        if (variableRows > 1)
        {
            pixelStream << ArrayString(registerInfo.rowIndex);
        }

        pixelStream << " = input.v" << registerIndex;

        if (!varying.isStruct())
        {
            switch (VariableColumnCount(transposedType))
            {
                case 1:
                    pixelStream << ".x";
                    break;
                case 2:
                    pixelStream << ".xy";
                    break;
                case 3:
                    pixelStream << ".xyz";
                    break;
                case 4:
                    break;
                default:
                    UNREACHABLE();
            }
        }
        pixelStream << ";\n";
    }

    if (fragmentShader->usesDeferredInit())
    {
        pixelStream << "\n"
                    << "    initializeDeferredGlobals();\n";
    }

    pixelStream << "\n"
                << "    gl_main();\n"
                << "\n"
                << "    return generateOutput();\n"
                << "}\n";

    *vertexHLSL = vertexStream.str();
    *pixelHLSL  = pixelStream.str();

    return true;
}

std::string DynamicHLSL::generateGeometryShaderPreamble(
    const gl::Data &data,
    const gl::Program::Data &programData,
    unsigned int registerCount,
    const std::vector<PackedVarying> &packedVaryings) const
{
    ASSERT(registerCount >= 0 && registerCount <= data.caps->maxVaryingVectors);
    ASSERT(mRenderer->getMajorShaderModel() >= 4);

    // Must be called during link, not from a binary load.
    const ShaderD3D *vertexShader   = GetImplAs<ShaderD3D>(programData.getAttachedVertexShader());
    const ShaderD3D *fragmentShader = GetImplAs<ShaderD3D>(programData.getAttachedFragmentShader());
    ASSERT(vertexShader && fragmentShader);

    bool usesFragCoord  = fragmentShader->usesFragCoord();
    bool usesPointCoord = fragmentShader->usesPointCoord();
    bool usesPointSize  = vertexShader->usesPointSize();

    const SemanticInfo &inSemantics =
        getSemanticInfo(SHADER_VERTEX, registerCount, true, usesFragCoord, false, usesPointSize);
    const SemanticInfo &outSemantics = getSemanticInfo(
        SHADER_GEOMETRY, registerCount, true, usesFragCoord, usesPointCoord, usesPointSize);

    std::stringstream preambleStream;

    preambleStream << "struct GS_INPUT\n";
    generateVaryingLinkHLSL(*data.caps, usesPointSize, inSemantics, packedVaryings, preambleStream);
    preambleStream << "\n"
                   << "struct GS_OUTPUT\n";
    generateVaryingLinkHLSL(*data.caps, usesPointSize, outSemantics, packedVaryings,
                            preambleStream);
    preambleStream
        << "\n"
        << "void copyVertex(inout GS_OUTPUT output, GS_INPUT input, GS_INPUT flatinput)\n"
        << "{\n"
        << "    output.gl_Position = input.gl_Position;\n";

    if (usesPointSize)
    {
        preambleStream << "    output.gl_PointSize = input.gl_PointSize;\n";
    }

    for (const PackedVaryingRegister &varyingRegister : PackedVaryingIterator(packedVaryings))
    {
        const sh::Varying &varying = *packedVaryings[varyingRegister.varyingIndex].varying;
        unsigned int registerIndex = varyingRegister.registerIndex(*data.caps, packedVaryings);

        preambleStream << "    output.v" << registerIndex << " = ";
        if (varying.interpolation == sh::INTERPOLATION_FLAT)
        {
            preambleStream << "flat";
        }
        preambleStream << "input.v" << registerIndex << "; \n";
    }

    if (usesFragCoord)
    {
        preambleStream << "    output.gl_FragCoord = input.gl_FragCoord;\n";
    }

    // Only write the dx_Position if we aren't using point sprites
    preambleStream << "#ifndef ANGLE_POINT_SPRITE_SHADER\n"
                   << "    output.dx_Position = input.dx_Position;\n"
                   << "#endif  // ANGLE_POINT_SPRITE_SHADER\n"
                   << "}\n";

    return preambleStream.str();
}

std::string DynamicHLSL::generateGeometryShaderHLSL(gl::PrimitiveType primitiveType,
                                                    const gl::Data &data,
                                                    const gl::Program::Data &programData,
                                                    const std::string &preambleString) const
{
    ASSERT(mRenderer->getMajorShaderModel() >= 4);

    std::stringstream shaderStream;

    const bool pointSprites   = (primitiveType == PRIMITIVE_POINTS);
    const bool usesPointCoord = preambleString.find("gl_PointCoord") != std::string::npos;

    const char *inputPT  = nullptr;
    const char *outputPT = nullptr;
    int inputSize        = 0;
    int maxVertexOutput  = 0;

    switch (primitiveType)
    {
        case PRIMITIVE_POINTS:
            inputPT         = "point";
            outputPT        = "Triangle";
            inputSize       = 1;
            maxVertexOutput = 4;
            break;

        case PRIMITIVE_LINES:
        case PRIMITIVE_LINE_STRIP:
        case PRIMITIVE_LINE_LOOP:
            inputPT         = "line";
            outputPT        = "Line";
            inputSize       = 2;
            maxVertexOutput = 2;
            break;

        case PRIMITIVE_TRIANGLES:
        case PRIMITIVE_TRIANGLE_STRIP:
        case PRIMITIVE_TRIANGLE_FAN:
            inputPT         = "triangle";
            outputPT        = "Triangle";
            inputSize       = 3;
            maxVertexOutput = 3;
            break;

        default:
            UNREACHABLE();
            break;
    }

    if (pointSprites)
    {
        shaderStream << "#define ANGLE_POINT_SPRITE_SHADER\n"
                        "\n"
                        "uniform float4 dx_ViewCoords : register(c1);\n"
                        "\n"
                        "static float2 pointSpriteCorners[] = \n"
                        "{\n"
                        "    float2( 0.5f, -0.5f),\n"
                        "    float2( 0.5f,  0.5f),\n"
                        "    float2(-0.5f, -0.5f),\n"
                        "    float2(-0.5f,  0.5f)\n"
                        "};\n"
                        "\n"
                        "static float2 pointSpriteTexcoords[] = \n"
                        "{\n"
                        "    float2(1.0f, 1.0f),\n"
                        "    float2(1.0f, 0.0f),\n"
                        "    float2(0.0f, 1.0f),\n"
                        "    float2(0.0f, 0.0f)\n"
                        "};\n"
                        "\n"
                        "static float minPointSize = "
                     << static_cast<int>(data.caps->minAliasedPointSize)
                     << ".0f;\n"
                        "static float maxPointSize = "
                     << static_cast<int>(data.caps->maxAliasedPointSize) << ".0f;\n"
                     << "\n";
    }

    shaderStream << preambleString << "\n"
                 << "[maxvertexcount(" << maxVertexOutput << ")]\n"
                 << "void main(" << inputPT << " GS_INPUT input[" << inputSize << "], ";

    if (primitiveType == PRIMITIVE_TRIANGLE_STRIP)
    {
        shaderStream << "uint primitiveID : SV_PrimitiveID, ";
    }

    shaderStream << " inout " << outputPT << "Stream<GS_OUTPUT> outStream)\n"
                 << "{\n"
                 << "    GS_OUTPUT output = (GS_OUTPUT)0;\n";

    if (primitiveType == PRIMITIVE_TRIANGLE_STRIP)
    {
        shaderStream << "    uint lastVertexIndex = (primitiveID % 2 == 0 ? 2 : 1);\n";
    }
    else
    {
        shaderStream << "    uint lastVertexIndex = " << (inputSize - 1) << ";\n";
    }

    for (int vertexIndex = 0; vertexIndex < inputSize; ++vertexIndex)
    {
        shaderStream << "    copyVertex(output, input[" << vertexIndex
                     << "], input[lastVertexIndex]);\n";

        if (!pointSprites)
        {
            ASSERT(inputSize == maxVertexOutput);
            shaderStream << "    outStream.Append(output);\n";
        }
    }

    if (pointSprites)
    {
        shaderStream << "\n"
                        "    float4 dx_Position = input[0].dx_Position;\n"
                        "    float gl_PointSize = clamp(input[0].gl_PointSize, minPointSize, "
                        "maxPointSize);\n"
                        "    float2 viewportScale = float2(1.0f / dx_ViewCoords.x, 1.0f / "
                        "dx_ViewCoords.y) * dx_Position.w;\n";

        for (int corner = 0; corner < 4; corner++)
        {
            shaderStream << "\n"
                            "    output.dx_Position = dx_Position + float4(pointSpriteCorners["
                         << corner << "] * viewportScale * gl_PointSize, 0.0f, 0.0f);\n";

            if (usesPointCoord)
            {
                shaderStream << "    output.gl_PointCoord = pointSpriteTexcoords[" << corner
                             << "];\n";
            }

            shaderStream << "    outStream.Append(output);\n";
        }
    }

    shaderStream << "    \n"
                    "    outStream.RestartStrip();\n"
                    "}\n";

    return shaderStream.str();
}

// This method needs to match OutputHLSL::decorate
std::string DynamicHLSL::decorateVariable(const std::string &name)
{
    if (name.compare(0, 3, "gl_") != 0)
    {
        return "_" + name;
    }

    return name;
}

std::string DynamicHLSL::generateAttributeConversionHLSL(
    gl::VertexFormatType vertexFormatType,
    const sh::ShaderVariable &shaderAttrib) const
{
    const gl::VertexFormat &vertexFormat = gl::GetVertexFormatFromType(vertexFormatType);
    std::string attribString             = "input." + decorateVariable(shaderAttrib.name);

    // Matrix
    if (IsMatrixType(shaderAttrib.type))
    {
        return "transpose(" + attribString + ")";
    }

    GLenum shaderComponentType = VariableComponentType(shaderAttrib.type);
    int shaderComponentCount   = VariableComponentCount(shaderAttrib.type);

    // Perform integer to float conversion (if necessary)
    bool requiresTypeConversion =
        (shaderComponentType == GL_FLOAT && vertexFormat.type != GL_FLOAT);

    if (requiresTypeConversion)
    {
        // TODO: normalization for 32-bit integer formats
        ASSERT(!vertexFormat.normalized && !vertexFormat.pureInteger);
        return "float" + Str(shaderComponentCount) + "(" + attribString + ")";
    }

    // No conversion necessary
    return attribString;
}
}  // namespace rx
