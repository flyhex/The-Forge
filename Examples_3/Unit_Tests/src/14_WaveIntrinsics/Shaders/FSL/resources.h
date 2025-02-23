/*
 * Copyright (c) 2018-2021 The Forge Interactive Inc.
 * 
 * This file is part of The-Forge
 * (see https://github.com/ConfettiFX/The-Forge).
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
*/

#ifndef RESOURCES_H
#define RESOURCES_H

RES(Tex2D(float4), g_texture, UPDATE_FREQ_NONE, t1, binding = 1);
RES(SamplerState, g_sampler, UPDATE_FREQ_NONE, s2, binding = 2);

CBUFFER(SceneConstantBuffer, UPDATE_FREQ_PER_FRAME, b0, binding = 0)
{
	DATA(float4x4, orthProjMatrix, None);
	DATA(float2, mousePosition, None);
	DATA(float2, resolution, None);
	DATA(float, time, None);
	DATA(uint, renderMode, None);
	DATA(uint, laneSize, None);
	DATA(uint, padding, None);
};

#endif