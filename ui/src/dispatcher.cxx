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
#include "dispatcher.h"

#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <algorithm>
#include <condition_variable>

using namespace hprof;

class EventsDisparcherImpl : public EventsDisparcher {
public:
    EventsDisparcherImpl();
    virtual ~EventsDisparcherImpl();
    void start();
    void stop();
    void subscribe(Storage* storage);
    void emit(std::unique_ptr<Action>&& action);
private:
    void emit_events();
    void emit_event_locked(const Action& action);
private:
    volatile bool _do_loop;
    std::mutex _mutex;
    std::condition_variable _actions_conditions;
    std::thread _worker_thread;
    std::vector<Storage*> _storages;
    std::queue<std::unique_ptr<Action>> _actions;
};

EventsDisparcher::~EventsDisparcher() {}

std::unique_ptr<EventsDisparcher> EventsDisparcher::create() {
    return std::make_unique<EventsDisparcherImpl>();
}

EventsDisparcherImpl::EventsDisparcherImpl() {}

EventsDisparcherImpl::~EventsDisparcherImpl() {
    stop();
}

void EventsDisparcherImpl::subscribe(Storage* storage) {
    std::unique_lock<std::mutex> lock { _mutex };
    auto item = std::find_if(std::begin(_storages), std::end(_storages), [storage] (auto item) -> bool { return item == storage; });
    if (item != std::end(_storages)) {
        return;
    }
    _storages.push_back(storage);
}

void EventsDisparcherImpl::emit(std::unique_ptr<Action>&& action) {
    std::unique_lock<std::mutex> lock { _mutex };
    if (!_do_loop) return;
    _actions.push(std::move(action));
    _actions_conditions.notify_one();
}

void EventsDisparcherImpl::start() {
    std::unique_lock<std::mutex> lock { _mutex };
    _do_loop = true;
    _worker_thread = std::thread { [this] () { this->emit_events(); }};
}

void EventsDisparcherImpl::stop() {
    std::unique_lock<std::mutex> lock { _mutex };
    _do_loop = false;
    if (!_worker_thread.joinable()) return;
    _actions_conditions.notify_one();
    lock.unlock();
    _worker_thread.join();
}

void EventsDisparcherImpl::emit_events() {
    std::unique_lock<std::mutex> lock { _mutex };
    do {
        while (!_actions.empty()) {
            emit_event_locked(*_actions.front());
            _actions.pop();
        }
        _actions_conditions.wait(lock);
    } while(_do_loop);
}

void EventsDisparcherImpl::emit_event_locked(const Action& action) {
    for (auto storage : _storages) {
        storage->emit(action);
    }
}