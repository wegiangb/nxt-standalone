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

#include "backend/d3d12/PipelineLayoutD3D12.h"

#include "backend/d3d12/D3D12Backend.h"
#include "backend/d3d12/BindGroupLayoutD3D12.h"
#include "common/Assert.h"

using Microsoft::WRL::ComPtr;

namespace backend {
namespace d3d12 {

    PipelineLayout::PipelineLayout(Device* device, PipelineLayoutBuilder* builder)
        : PipelineLayoutBase(builder), device(device) {

        D3D12_ROOT_PARAMETER rootParameters[kMaxBindGroups * 2];

        // A root parameter is one of these types
        union {
            D3D12_ROOT_DESCRIPTOR_TABLE DescriptorTable;
            D3D12_ROOT_CONSTANTS Constants;
            D3D12_ROOT_DESCRIPTOR Descriptor;
        } rootParameterValues[kMaxBindGroups * 2];
        // samplers must be in a separate descriptor table so we need at most twice as many tables as bind groups

        // Ranges are D3D12_DESCRIPTOR_RANGE_TYPE_(SRV|UAV|CBV|SAMPLER)
        // They are grouped together so each bind group has at most 4 ranges
        D3D12_DESCRIPTOR_RANGE ranges[kMaxBindGroups * 4];

        uint32_t parameterIndex = 0;
        uint32_t rangeIndex = 0;

        for (uint32_t group = 0; group < kMaxBindGroups; ++group) {
            const BindGroupLayout* bindGroupLayout = ToBackend(GetBindGroupLayout(group));

            // Set the root descriptor table parameter and copy ranges. Ranges are offset by the bind group index
            // Returns whether or not the parameter was set. A root parameter is not set if the number of ranges is 0
            auto SetRootDescriptorTable = [&](uint32_t rangeCount, const D3D12_DESCRIPTOR_RANGE* descriptorRanges) -> bool {
                if (rangeCount == 0) {
                    return false;
                }

                auto& rootParameter = rootParameters[parameterIndex];
                rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
                rootParameter.DescriptorTable = rootParameterValues[parameterIndex].DescriptorTable;
                rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
                rootParameter.DescriptorTable.NumDescriptorRanges = rangeCount;
                rootParameter.DescriptorTable.pDescriptorRanges = &ranges[rangeIndex];

                for (uint32_t i = 0; i < rangeCount; ++i) {
                    ranges[rangeIndex] = descriptorRanges[i];
                    ranges[rangeIndex].BaseShaderRegister = group * kMaxBindingsPerGroup;
                    rangeIndex++;
                }

                return true;
            };

            if (SetRootDescriptorTable(bindGroupLayout->GetCbvUavSrvDescriptorTableSize(), bindGroupLayout->GetCbvUavSrvDescriptorRanges())) {
                cbvUavSrvRootParameterInfo[group] = parameterIndex++;
            }

            if (SetRootDescriptorTable(bindGroupLayout->GetSamplerDescriptorTableSize(), bindGroupLayout->GetSamplerDescriptorRanges())) {
                samplerRootParameterInfo[group] = parameterIndex++;
            }
        }

        D3D12_ROOT_SIGNATURE_DESC rootSignatureDescriptor;
        rootSignatureDescriptor.NumParameters = parameterIndex;
        rootSignatureDescriptor.pParameters = rootParameters;
        rootSignatureDescriptor.NumStaticSamplers = 0;
        rootSignatureDescriptor.pStaticSamplers = nullptr;
        rootSignatureDescriptor.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        ASSERT_SUCCESS(D3D12SerializeRootSignature(&rootSignatureDescriptor, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
        ASSERT_SUCCESS(device->GetD3D12Device()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));
    }


    uint32_t PipelineLayout::GetCbvUavSrvRootParameterIndex(uint32_t group) const {
        ASSERT(group < kMaxBindGroups);
        return cbvUavSrvRootParameterInfo[group];
    }

    uint32_t PipelineLayout::GetSamplerRootParameterIndex(uint32_t group) const {
        ASSERT(group < kMaxBindGroups);
        return samplerRootParameterInfo[group];
    }

    ComPtr<ID3D12RootSignature> PipelineLayout::GetRootSignature() {
        return rootSignature;
    }
}
}
