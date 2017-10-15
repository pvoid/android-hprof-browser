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

#include "types.h"
#include "types/class.h"
#include "types/instance.h"
#include "types/string_instance.h"
#include "types/primitives_array.h"
#include "types/objects_array.h"
#include <memory>

namespace hprof {
    class heap_item_impl_t : public heap_item_t {
    public:
        heap_item_impl_t(class_info_impl_ptr_t&& klass) : 
            _type(Class), _class(klass.release()), _deleter(new (std::nothrow) deleter_holder_impl_t<decltype(klass.get_deleter()), decltype(klass.get())>(klass.get_deleter())) {}

        heap_item_impl_t(instance_info_impl_ptr_t&& instance) : 
            _type(Object), _instance(instance.release()), _deleter(new (std::nothrow) deleter_holder_impl_t<decltype(instance.get_deleter()), decltype(instance.get())>(instance.get_deleter())) {}

        heap_item_impl_t(string_info_impl_ptr_t&& text) : 
            _type(String), _string(text.release()), _deleter(new (std::nothrow) deleter_holder_impl_t<decltype(text.get_deleter()), decltype(text.get())>(text.get_deleter())) {}

        heap_item_impl_t(primitives_array_info_impl_ptr_t&& array) : 
            _type(PrimitivesArray), _primitives_array(array.release()), _deleter(new (std::nothrow) deleter_holder_impl_t<decltype(array.get_deleter()), decltype(array.get())>(array.get_deleter())) {}

        heap_item_impl_t(objects_array_info_impl_ptr_t&& array) : 
            _type(ObjectsArray), _objects_array(array.release()), _deleter(new (std::nothrow) deleter_holder_impl_t<decltype(array.get_deleter()), decltype(array.get())>(array.get_deleter())) {}

        heap_item_impl_t(const heap_item_impl_t&) = delete;
        heap_item_impl_t(heap_item_impl_t&&) = default;

        virtual ~heap_item_impl_t() {
            (*_deleter)(_class);
        }

        heap_item_impl_t& operator=(const heap_item_impl_t&) = delete;
        heap_item_impl_t& operator=(heap_item_impl_t&&) = default;

        virtual type_t type() const override { return _type; }

        virtual operator const class_info_t*() const override {
            if (_type == Class) return _class;
            return nullptr;
        }

        virtual operator class_info_impl_t*() {
            if (_type == Class) return _class;
            return nullptr;
        }

        virtual operator const instance_info_t*() const override {
            if (_type == Object) return _instance;
            return nullptr;
        }

        virtual operator const string_info_t*() const override {
            if (_type == String) return _string;
            return nullptr;
        }

        virtual operator const primitives_array_info_t*() const override {
            if (_type == PrimitivesArray) return _primitives_array;
            return nullptr;
        }

        virtual operator const objects_array_info_t*() const override {
            if (_type == ObjectsArray) return _objects_array;
            return nullptr;
        }

    private:
        struct deleter_holder_t {
            virtual void operator()(void *ptr) = 0;
        };

        template<typename Deleter, typename Type>
        struct deleter_holder_impl_t : public deleter_holder_t {
            virtual void operator()(void *ptr) override {
                _deleter((Type)ptr);
            }

            deleter_holder_impl_t(const Deleter& deleter) : _deleter(deleter) {}

            Deleter _deleter;
        };
    private:
        type_t _type;
        union {
            class_info_impl_t* _class;
            instance_info_impl_t* _instance;
            string_info_impl_t* _string;
            primitives_array_info_impl_t* _primitives_array;
            objects_array_info_impl_t* _objects_array;
        };
        std::unique_ptr<deleter_holder_t> _deleter;
    };

    using heap_item_impl_ptr_t = std::shared_ptr<heap_item_impl_t>;
}
