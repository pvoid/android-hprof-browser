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

#include "gtkmm.h"
#include "actions.h"

#include <mutex>
#include <queue>

namespace hprof {
    class Storage {
    public:
        virtual ~Storage() {}
        virtual void emit(const Action& action) = 0;
    };

    struct StorageSignal {
        int32_t signal;

        StorageSignal(int32_t signal) : signal(signal) {}
    };

    class SignalsHelper {
    public:
        SignalsHelper();
        virtual ~SignalsHelper();
    protected:
        void send_signal(std::unique_ptr<StorageSignal>&& signal);
        virtual void on_process_signal(const StorageSignal* signal) = 0;
    private:
        void on_emit_next_signal();
    private:
        std::mutex _mutex;
        std::queue<std::unique_ptr<StorageSignal>> _signals;
        Glib::Dispatcher _dispatcher;
    };
}
