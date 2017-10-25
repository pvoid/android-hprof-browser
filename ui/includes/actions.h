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

#include <string>
#include <memory>

namespace hprof {
    struct Action {
        enum action_type_t {
            OpenFile
        } type;

        Action(action_type_t action) : type(action) {}
    };

    struct OpenFileAction : public Action {
        OpenFileAction(const std::string& file_name) : Action(OpenFile), file_name(file_name) {}
        std::string file_name;

        static std::unique_ptr<Action> create(const std::string& file_name) { return std::make_unique<OpenFileAction>(file_name); }
    };
}
