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

#ifndef BACKEND_BLENDSTATE_H_
#define BACKEND_BLENDSTATE_H_

#include "backend/Forward.h"
#include "backend/Builder.h"
#include "backend/RefCounted.h"

#include "nxt/nxtcpp.h"

namespace backend {

    class BlendStateBase : public RefCounted {
        public:
            BlendStateBase(BlendStateBuilder* builder);

            struct BlendInfo {
                struct BlendOpFactor {
                    nxt::BlendOperation operation = nxt::BlendOperation::Add;
                    nxt::BlendFactor srcFactor = nxt::BlendFactor::One;
                    nxt::BlendFactor dstFactor = nxt::BlendFactor::Zero;
                };

                bool blendEnabled = false;
                BlendOpFactor alphaBlend;
                BlendOpFactor colorBlend;
                nxt::ColorWriteMask colorWriteMask = nxt::ColorWriteMask::All;
            };

            const BlendInfo& GetBlendInfo() const;

            private:
                BlendInfo blendInfo;

    };

    class BlendStateBuilder : public Builder<BlendStateBase> {
        public:
            BlendStateBuilder(DeviceBase* device);

            // NXT API
            void SetBlendEnabled(bool blendEnabled);
            void SetAlphaBlend(nxt::BlendOperation blendOperation, nxt::BlendFactor srcFactor, nxt::BlendFactor dstFactor);
            void SetColorBlend(nxt::BlendOperation blendOperation, nxt::BlendFactor srcFactor, nxt::BlendFactor dstFactor);
            void SetColorWriteMask(nxt::ColorWriteMask colorWriteMask);

        private:
            friend class BlendStateBase;

            BlendStateBase* GetResultImpl() override;

            int propertiesSet = 0;

            BlendStateBase::BlendInfo blendInfo;
    };

}

#endif // BACKEND_BLENDSTATE_H_
