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

#ifndef BACKEND_D3D12_FRAMEBUFFERD3D12_H_
#define BACKEND_D3D12_FRAMEBUFFERD3D12_H_

#include "backend/Framebuffer.h"

#include "common/Constants.h"
#include "backend/d3d12/d3d12_platform.h"
#include "backend/d3d12/DescriptorHeapAllocator.h"

#include <array>
#include <vector>

namespace backend {
namespace d3d12 {

    class Device;

    class Framebuffer : public FramebufferBase {
        public:
            struct OMSetRenderTargetArgs {
                unsigned int numRTVs = 0;
                std::array<D3D12_CPU_DESCRIPTOR_HANDLE, kMaxColorAttachments> RTVs = {};
                D3D12_CPU_DESCRIPTOR_HANDLE dsv = {};
            };

            Framebuffer(Device* device, FramebufferBuilder* builder);
            OMSetRenderTargetArgs GetSubpassOMSetRenderTargetArgs(uint32_t subpassIndex);
            D3D12_CPU_DESCRIPTOR_HANDLE GetRTVDescriptor(uint32_t attachmentSlot);
            D3D12_CPU_DESCRIPTOR_HANDLE GetDSVDescriptor(uint32_t attachmentSlot);

        private:
            Device* device = nullptr;
            DescriptorHeapHandle rtvHeap = {};
            DescriptorHeapHandle dsvHeap = {};

            // Indices into either the RTV or DSV heap, depending on texture format.
            std::vector<uint32_t> attachmentHeapIndices;
    };

}
}

#endif // BACKEND_D3D12_FRAMEBUFFERD3D12_H_
