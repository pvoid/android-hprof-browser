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
#include "storage.h"

using namespace hprof;

SignalsHelper::SignalsHelper() {
    _dispatcher.connect(sigc::mem_fun(this, &SignalsHelper::on_emit_next_signal));
}

SignalsHelper::~SignalsHelper() {}

void SignalsHelper::send_signal(std::unique_ptr<StorageSignal>&& signal) {
    std::unique_lock<std::mutex> lock { _mutex };
    _signals.push(std::move(signal));
    _dispatcher.emit();
}

void SignalsHelper::on_emit_next_signal() {
    std::unique_lock<std::mutex> lock { _mutex };
    auto signal = std::move(_signals.front());
    _signals.pop();
    lock.unlock();

    if (signal != nullptr) {
        on_process_signal(signal.get());
    }
}
