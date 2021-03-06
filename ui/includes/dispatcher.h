///
///  Copyright 2017 Dmitry "PVOID" Petukhov
///
///  Licensed under the Apache License, Version 2.0 (the "License");
///  you may not use this file except in compliance with the License.
///  You may obtain a copy of the License at
///
///      http://www.apache.org/licenses/LICENSE-2.0
///
///  Unless required by applicable law or agreed to in writing, software
///  distributed under the License is distributed on an "AS IS" BASIS,
///  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
///  See the License for the specific language governing permissions and
///  limitations under the License.
///
#pragma once

#include "actions.h"
#include "storage.h"

#include <memory>

namespace hprof {
    class EventsDisparcher {
    public:
        virtual ~EventsDisparcher() = 0;
        virtual void start() = 0;
        virtual void stop() = 0;
        virtual void subscribe(Storage* storage) = 0;
        virtual void emit(std::unique_ptr<Action>&& action) = 0;
    public:
        static std::unique_ptr<EventsDisparcher> create();
    };
}
