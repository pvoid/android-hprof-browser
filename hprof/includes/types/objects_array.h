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
#include "types/object.h"

namespace hprof {
    class objects_array_info_impl_t;

    class objects_array_info_impl_t_deleter {
    public:
        void operator()(objects_array_info_impl_t* ptr) const;
    };

    using objects_array_info_impl_ptr_t = std::unique_ptr<objects_array_info_impl_t, objects_array_info_impl_t_deleter>;

    class objects_array_info_impl_t : public virtual objects_array_info_t, public object_info_impl_t {
    private:
        class items_iterator {
        public:
            items_iterator(u_int8_t id_size, const u_int8_t* data, const u_int8_t* end) : _id_size(id_size), _data(data), _end(end) {
                fetch_current();
            }

            bool operator!=(const items_iterator& src) const { return _data != src._data; }
            bool operator==(const items_iterator& src) const { return _data == src._data; }

            items_iterator& operator++() { 
                _data += _id_size;
                fetch_current();
                return *this; 
            }

            const jvm_id_t& operator*() const {
                return _current;
            }

            const jvm_id_t* operator->() const {
                return &_current;
            }
        private:
            void fetch_current() const {
                _current = 0;
                for (const u_int8_t *start = _data, *end = std::min(_data + _id_size, _end); start < end; ++start) {
                    _current <<= 8;
                    _current |= *start;
                }
            }
        private:
            size_t _id_size;
            mutable const u_int8_t* _data;
            const u_int8_t* _end;
            mutable jvm_id_t _current;
        };
    public:
        objects_array_info_impl_t(const objects_array_info_impl_t&) = delete;
        objects_array_info_impl_t(objects_array_info_impl_t&&) = default;
        virtual ~objects_array_info_impl_t();

        objects_array_info_impl_t& operator=(const objects_array_info_impl_t&) = delete;
        objects_array_info_impl_t& operator=(objects_array_info_impl_t&&) = default;

        virtual int32_t has_link_to(jvm_id_t id) const override {
            int32_t result = 0;
            if (id == _class_id) {
                result |= link_t::TYPE_INSTANCE;
            }

            for (auto it = begin(); it != end(); ++it) {
                if (*it == id) {
                    result |= link_t::TYPE_OWNERSHIP;
                }
            }

            return result;
        }
        
        virtual jvm_id_t class_id() const override { return _class_id; }

        virtual size_t length() const override { return _length; }
        
        virtual iterator begin() const override { 
            return objects_array_info_t::iterator { items_iterator { id_size(), pointer_for_item(0), pointer_for_item(_length) } };
        }
        
        virtual iterator end() const override {
            auto end = pointer_for_item(_length);
            return objects_array_info_t::iterator {items_iterator { id_size(), end, end } };
        }

        virtual iterator operator[](size_t index) const override {
            return objects_array_info_t::iterator { items_iterator { id_size(), pointer_for_item(std::min(index, _length)), pointer_for_item(_length) } };
        }

        u_int8_t* data() { return _data; }
    private:
        const u_int8_t* pointer_for_item(size_t index) const {
            return static_cast<const u_int8_t*>(_data) + (static_cast<size_t>(id_size()) * index);
        }
    public:
        static objects_array_info_impl_ptr_t create(u_int8_t id_size, jvm_id_t id, jvm_id_t class_id, size_t length, size_t data_size) {
            auto mem = new (std::nothrow) u_int8_t[sizeof(objects_array_info_impl_t) + data_size];
            return objects_array_info_impl_ptr_t { new (mem) objects_array_info_impl_t(id_size, id, class_id, length, data_size) };
        }
    private:
        objects_array_info_impl_t(u_int8_t id_size, jvm_id_t id, jvm_id_t class_id, size_t length, size_t data_size) :
            object_info_impl_t(id_size, id), _class_id(class_id), _length(length), 
            _data(reinterpret_cast<u_int8_t *>(this) + sizeof(objects_array_info_impl_t)), _data_size(data_size) {}
    private:
        jvm_id_t _class_id;
        size_t _length;
        u_int8_t* _data;
        size_t _data_size;
    };
}
