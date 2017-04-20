// Copyright 2017 The NXT Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "PipelineLayoutGL.h"

#include "OpenGLBackend.h"

namespace backend {
namespace opengl {

    PipelineLayout::PipelineLayout(Device* device, PipelineLayoutBuilder* builder)
        : PipelineLayoutBase(builder), device(device) {
        GLuint uboIndex = 0;
        GLuint samplerIndex = 0;
        GLuint sampledTextureIndex = 0;
        GLuint ssboIndex = 0;

        for (size_t group = 0; group < kMaxBindGroups; ++group) {
            const auto& groupInfo = GetBindGroupLayout(group)->GetBindingInfo();

            for (size_t binding = 0; binding < kMaxBindingsPerGroup; ++binding) {
                if (!groupInfo.mask[binding]) {
                    continue;
                }

                switch (groupInfo.types[binding]) {
                    case nxt::BindingType::UniformBuffer:
                        indexInfo[group][binding] = uboIndex;
                        uboIndex ++;
                        break;
                    case nxt::BindingType::Sampler:
                        indexInfo[group][binding] = samplerIndex;
                        samplerIndex ++;
                        break;
                    case nxt::BindingType::SampledTexture:
                        indexInfo[group][binding] = sampledTextureIndex;
                        sampledTextureIndex ++;
                        break;

                    case nxt::BindingType::StorageBuffer:
                        indexInfo[group][binding] = ssboIndex;
                        ssboIndex ++;
                        break;
                }
            }
        }

        numSamplers = samplerIndex;
        numSampledTextures = sampledTextureIndex;
    }

    const PipelineLayout::BindingIndexInfo& PipelineLayout::GetBindingIndexInfo() const {
        return indexInfo;
    }

    GLuint PipelineLayout::GetTextureUnitsUsed() const {
        return 0;
    }

    size_t PipelineLayout::GetNumSamplers() const {
        return numSamplers;
    }

    size_t PipelineLayout::GetNumSampledTextures() const {
        return numSampledTextures;
    }

}
}