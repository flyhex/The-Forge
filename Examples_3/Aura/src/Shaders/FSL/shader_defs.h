/*
 * Copyright (c) 2018-2021 Confetti Interactive Inc.
 *
 * This is a part of Aura.
 *
 * This file(code) is licensed under a
 * Creative Commons Attribution-NonCommercial 4.0 International License
 *
 *   (https://creativecommons.org/licenses/by-nc/4.0/legalcode)
 *
 * Based on a work at https://github.com/ConfettiFX/The-Forge.
 * You may not use the material for commercial purposes.
*/

#ifndef SHADER_DEFS_H
#define SHADER_DEFS_H

#include "wind.h"

#ifdef NO_FSL_DEFINITIONS
	#define STATIC static
	#define STRUCT(NAME) struct NAME
	#define DATA(TYPE, NAME, SEM) TYPE NAME
	#define CBUFFER(NAME, REG, FREQ, BINDING) struct NAME
	#define f4x4 mat4
	#define uint uint32_t
#endif

#define LIGHT_COUNT 128
#define LIGHT_SIZE  150.0f

#define LIGHT_CLUSTER_WIDTH  8
#define LIGHT_CLUSTER_HEIGHT 8

#define LIGHT_CLUSTER_COUNT_POS(ix, iy)    (((iy) * LIGHT_CLUSTER_WIDTH) + (ix))
#define LIGHT_CLUSTER_DATA_POS(il, ix, iy) (LIGHT_CLUSTER_COUNT_POS(ix, iy) * LIGHT_COUNT + (il))

#define MAX_TEXTURE_UNITS 256U

// This defines the amount of triangles that will be processed in parallel by the
// compute shader in the triangle filtering process.
// Should be a multiple of the wavefront size
#define CLUSTER_SIZE 256

// BATCH_COUNT limits the amount of triangle batches we can process on the GPU.
// It depends on the amoutnt of data we need to store in the constant buffers, since
// the max constant buffer size is 64KB - sizeof(SmallBatchData) * 2048 = 64KB
#define BATCH_COUNT 2048

// This defines the amount of viewports that are going to be culled in parallel.
#define NUM_CULLING_VIEWPORTS 5
#define VIEW_SHADOW 0
#define VIEW_CAMERA 1

#define NUM_RSM_CASCADES 3
#define VIEW_RSM_CASCADE0 2
#define VIEW_RSM_CASCADE1 3
#define VIEW_RSM_CASCADE2 4

// This value defines the amount of threads per group that will be used to clear the
// indirect draw buffers.
#define CLEAR_THREAD_COUNT 256

// The following value defines the maximum amount of indirect draw calls that will be
// drawn at once. This value depends on the number of submeshes or individual objects
// in the scene. Changing a scene will require to change this value accordingly.
#define MAX_DRAWS_INDIRECT 256

// Size for the material buffer assuming each draw call uses one material index.
#define MATERIAL_BUFFER_SIZE (MAX_DRAWS_INDIRECT * 2 * NUM_CULLING_VIEWPORTS)

// The following values point to the position in the indirect draw buffer that holds the
// number of draw calls to draw after triangle filtering and batch compaction.
// This value number is stored in the last position of the indirect draw buffer.
// So it depends on MAX_DRAWS_INDIRECT
#if defined(ORBIS)
#define INDIRECT_DRAW_ARGUMENTS_STRUCT_NUM_ELEMENTS 5
#else
#define INDIRECT_DRAW_ARGUMENTS_STRUCT_NUM_ELEMENTS 8
#endif
#if defined(ORBIS) || defined(PROSPERO) || defined(METAL)
#define INDIRECT_DRAW_ARGUMENTS_STRUCT_OFFSET 0
#else
#define INDIRECT_DRAW_ARGUMENTS_STRUCT_OFFSET 1
#endif
#define DRAW_COUNTER_SLOT_POS 				((MAX_DRAWS_INDIRECT - 1) * INDIRECT_DRAW_ARGUMENTS_STRUCT_NUM_ELEMENTS)
#define DRAW_COUNTER_SLOT_OFFSET_IN_BYTES	(DRAW_COUNTER_SLOT_POS * sizeof(uint))

// This function is used to get the offset of the current material base index depending
// on the type of geometry and on the culling view.
#define BaseMaterialBuffer(alpha, viewID) (((viewID) * 2 + ((alpha) ? 0 : 1)) * MAX_DRAWS_INDIRECT)

#define LoadVertex(vertexDataBuffer, index) (float4(asfloat(LoadByte4(vertexDataBuffer, (index) * 12)).xyz, 1))

#ifdef METAL
// icb units
#define UNIT_VBPASS_POSITION              30
#define UNIT_VBPASS_TEXCOORD              29
#define UNIT_VBPASS_NORMAL                28
#define UNIT_VBPASS_TANGENT               27
// textures are placed in AB UPDATE_FREQ_NONE
#define UNIT_VBPASS_TEXTURES              UPDATE_FREQ_NONE
// the following buffer locations need to be manually set from generated mtl shaders
#define UNIT_VBPASS_UNIFORMS              6
#define UNIT_INDIRECT_MATERIAL_RW         7
#define UNIT_VBPASS_DRAWID                8
#define UNIT_RSMPASS_MATCONSTANTS         9
#define UNIT_VBPASS_MAX                   12
#endif

STRUCT(SmallBatchData)
{
	DATA(uint, meshIndex,         None); // Index into meshConstants
	DATA(uint, indexOffset,       None); // Index relative to the meshConstants[meshIndex].indexOffset
	DATA(uint, faceCount,         None); // Number of faces in this small batch
	DATA(uint, outputIndexOffset, None); // Offset into the output index buffer
	DATA(uint, drawBatchStart,    None); // First slot for the current draw call
	DATA(uint, accumDrawIndex,    None);
	DATA(uint, _pad0,             None);
	DATA(uint, _pad1,             None);
};

STRUCT(MeshConstants)
{
	DATA(uint, faceCount,   None);
	DATA(uint, indexOffset, None);
	DATA(uint, materialID,  None);
	DATA(uint, twoSided,    None); // 0 or 1
};

STRUCT(UncompactedDrawArguments)
{
	DATA(uint, numIndices, None);
	DATA(uint, startIndex, None);
	DATA(uint, materialID, None);
	DATA(uint, pad_,       None);
};

STRUCT(CullingViewPort)
{
	DATA(float2, windowSize,  None);
	DATA(uint,   sampleCount, None);
	DATA(uint,   _pad0,       None);
};

STRUCT(Transform)
{
	DATA(f4x4, mvp,        None);
	DATA(f4x4, invVP,      None);
	DATA(f4x4, vp,         None);
	DATA(f4x4, projection, None);
};

STRUCT(LightData)
{
	DATA(float4, position, None);
	DATA(float4, color,    None);
};

#ifdef METAL
STRUCT(MaterialConstants)
{
	DATA(uint, viewID,       None);
	DATA(uint, useColorMaps, None);
};
#endif

CBUFFER(PerFrameConstants, UPDATE_FREQ_PER_FRAME, b0, binding = 0)
{
	DATA(Transform,         transform[NUM_CULLING_VIEWPORTS],        None);
	DATA(CullingViewPort,   cullingViewports[NUM_CULLING_VIEWPORTS], None);
	DATA(float4,            camPos,                                  None);
	DATA(float4,            lightDir,                                None);
	DATA(float4,            lightColor,                              None);
	DATA(float2,            CameraPlane,                             None); //x : near, y : far
	DATA(uint,              lightingMode,                            None);
	DATA(uint,              _pad0,                                   None);
	DATA(float2,            twoOverRes,                              None);
	DATA(float,             esmControl,                              None);
	DATA(uint,              specularQuality,                         None);
	DATA(WindConstants,     windData,                                None);
#ifdef METAL
	DATA(MaterialConstants, materialConstants[NUM_RSM_CASCADES],     None);
#endif
};

#endif // SHADER_DEFS_H
