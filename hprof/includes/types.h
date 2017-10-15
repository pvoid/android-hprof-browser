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

#include "name_tokenizer.h"

#include <sys/types.h>
#include <chrono>
#include <set>
#include <unordered_map>
#include <memory>
#include <vector>
#include <type_traits>

namespace hprof {
    using jvm_id_t = u_int64_t;
    using jvm_bool_t = u_int8_t;
    using jvm_byte_t = int8_t;
    using jvm_char_t = u_int16_t;
    using jvm_short_t = int16_t;
    using jvm_float_t = float;
    using jvm_double_t = double;
    using jvm_int_t = int32_t;
    using jvm_long_t = int64_t;
    using time_t = std::chrono::time_point<std::chrono::system_clock>;

    class jvm_type_t {
    public:
        enum type_spec {
            JVM_TYPE_UNKNOWN,
            JVM_TYPE_OBJECT,
            JVM_TYPE_BOOL,
            JVM_TYPE_CHAR,
            JVM_TYPE_FLOAT,
            JVM_TYPE_DOUBLE,
            JVM_TYPE_BYTE,
            JVM_TYPE_SHORT,
            JVM_TYPE_INT,
            JVM_TYPE_LONG
        };
    public:
        jvm_type_t(jvm_type_t::type_spec type) : _value(type) {}
        operator type_spec() const { return _value; }
        bool operator==(const jvm_type_t& src) const { return src._value == _value; }
        bool operator!=(const jvm_type_t& src) const { return src._value != _value; }
        bool operator==(const jvm_type_t::type_spec& spec) const { return spec == _value; }
        bool operator!=(const jvm_type_t::type_spec& spec) const { return spec != _value; }
    public:
        static size_t size(jvm_type_t type, size_t id_size) {
            switch (type) {
                case JVM_TYPE_BOOL:
                case JVM_TYPE_BYTE:
                    return 1;
                case JVM_TYPE_CHAR:
                case JVM_TYPE_SHORT:
                    return 2;
                case JVM_TYPE_INT:
                case JVM_TYPE_FLOAT:
                    return 4;
                case JVM_TYPE_LONG:
                case JVM_TYPE_DOUBLE:
                    return 8;
                case JVM_TYPE_OBJECT:
                    return id_size;
                case JVM_TYPE_UNKNOWN:
                default:
                    return 0;
            }
        }

        template<typename T>
        static jvm_type_t type() {
            if (std::is_same<jvm_bool_t, T>::value) {
                return JVM_TYPE_BOOL;
            }

            if (std::is_same<jvm_byte_t, T>::value) {
                return JVM_TYPE_BYTE;
            }

            if (std::is_same<jvm_char_t, T>::value) {
                return JVM_TYPE_CHAR;
            }

            if (std::is_same<jvm_short_t, T>::value) {
                return JVM_TYPE_SHORT;
            }

            if (std::is_same<jvm_int_t, T>::value) {
                return JVM_TYPE_INT;
            }

            if (std::is_same<jvm_float_t, T>::value) {
                return JVM_TYPE_FLOAT;
            }

            if (std::is_same<jvm_long_t, T>::value) {
                return JVM_TYPE_LONG;
            }

            if (std::is_same<jvm_double_t, T>::value) {
                return JVM_TYPE_DOUBLE;
            }

            if (std::is_same<jvm_id_t, T>::value) {
                return JVM_TYPE_OBJECT;
            }

            return JVM_TYPE_UNKNOWN;
        }
    private:
        type_spec _value;
    };

    struct link_t {
        jvm_id_t from;
        enum {
            TYPE_INSTANCE = 0x001,
            TYPE_SUPER = 0x002,
            TYPE_CLASS_LOADER = 0x004,
            TYPE_OWNERSHIP = 0x008
        } type;
    };

    struct link_comparator_t {
        bool operator()(const link_t& left, const link_t& right) const {
            return left.from < right.from;
        }
    };

    using incoming_links_t = std::set<link_t, link_comparator_t>;

    using strings_map_t = std::unordered_map<jvm_id_t, std::string>;

    struct heap_info_t {
        enum : int {
            HEAP_UNKNOWN = 0,
            HEAP_APP,
            HEAP_SYSTEM,
            HEAP_ZYGOTE
        };
        int32_t type;
        jvm_id_t name;
    };

    class gc_root_t {
    public:
        enum root_type_t {
            INVALID,
            UNKNOWN,
            JNI_GLOBAL,
            JNI_LOCAL,
            JAVA_FRAME,
            NATIVE_STACK,
            STICKY_CLASS,
            THREAD_BLOCK,
            MONITOR_USED,
            THREAD_OBJECT,
            INTERNED_STRING,
            FINALIZING,
            DEBUGGER,
            REFERENCE_CLEANUP,
            VM_INTERNAL,
            JNI_MONITOR,
            UNREACHABLE
        };

        struct jni_global_info_t {
            int32_t jni_ref;
        };

        struct jni_local_info_t {
            int32_t thread_seq_num;
            int32_t stack_frame_id;
        };

        struct java_frame_info_t {
            int32_t thread_seq_num;
            int32_t stack_frame_id;
        };

        struct native_stack_info_t {
            int32_t thread_seq_num;
        };

        struct thread_block_info_t {
            int32_t thread_seq_num;
        };

        struct thread_object_info_t {
            int32_t thread_seq_num;
            int32_t stack_frame_id;
        };

        struct jni_monitor_info_t {
            int32_t thread_seq_num;
            int32_t stack_frame_id;
        };

    public:
        virtual ~gc_root_t();
        virtual root_type_t type() const = 0;
        virtual jvm_id_t object_id() const = 0;
        virtual const jni_global_info_t& jni_global_info() const = 0;
        virtual const jni_local_info_t& jni_local_info() const = 0;
        virtual const java_frame_info_t& java_frame_info() const = 0;
        virtual const native_stack_info_t& native_stack_info() const = 0;
        virtual const thread_block_info_t& thread_block_info() const = 0;
        virtual const thread_object_info_t& thread_object_info() const = 0;
        virtual const jni_monitor_info_t& jni_monitor_info() const = 0;
    };

    using gc_root_ptr_t = std::unique_ptr<gc_root_t>;

    class object_info_t {
    public:
        virtual ~object_info_t();
        virtual jvm_id_t id() const = 0;
        virtual u_int8_t id_size() const = 0;
        virtual int32_t heap_type() const = 0;
        virtual int32_t has_link_to(jvm_id_t id) const = 0;
        virtual const std::vector<std::unique_ptr<gc_root_t>>& gc_roots() const = 0;
    };

    class field_spec_t {
    public:
        virtual ~field_spec_t();
        virtual jvm_id_t name_id() const = 0;
        virtual const std::string& name() const = 0;
        virtual jvm_type_t type() const = 0;
        virtual size_t offset() const = 0;
    };

    template<typename I>
    class iterator_container_t {
        template<typename T>
        friend void swap(iterator_container_t<T>& lhs, iterator_container_t<T>& rhs);
        
        template<typename T>
        friend bool operator==(const iterator_container_t<T>&, const iterator_container_t<T>&);

        template<typename T>
        friend bool operator!=(const iterator_container_t<T>&, const iterator_container_t<T>&);
    public:
        template<typename T>
        // cppcheck-suppress noExplicitConstructor
        iterator_container_t(const T& base) : _impl(new (std::nothrow) iterator_holder_impl_t<T>(base)) {}
        iterator_container_t(const iterator_container_t<I>& src) : _impl(src._impl->clone()) {}
        virtual ~iterator_container_t() {}
        iterator_container_t<I>& operator++() { _impl->operator++(); return *this; }
        iterator_container_t<I> operator++(int) { auto result = *this; _impl->operator++(); return result; }
        const I& operator*() const { return _impl->operator*(); }
        const I* operator->() const { return _impl->operator->(); }
        iterator_container_t<I>& operator=(const iterator_container_t<I>& src) { _impl->operator=(src._impl.get()); return *this; }
    private:
        class iterator_holder_t {
        public:
            virtual ~iterator_holder_t() {}
            virtual iterator_holder_t& operator++() = 0;
            virtual iterator_holder_t& operator=(iterator_holder_t* src) = 0;
            virtual const I& operator*() const = 0;
            virtual const I* operator->() const = 0;
            virtual bool operator==(const iterator_holder_t&) const = 0;
            virtual bool operator!=(const iterator_holder_t&) const = 0;
            virtual iterator_holder_t* clone() const = 0;
        };

        template<typename T>
        class iterator_holder_impl_t : public iterator_holder_t{
        public:
            explicit iterator_holder_impl_t(const T& impl) : _impl(impl) {}
            virtual ~iterator_holder_impl_t() {}
            
            virtual bool operator==(const iterator_holder_t& src) const { 
                return _impl == static_cast<const iterator_holder_impl_t*>(&src)->_impl;
            }

            virtual bool operator!=(const iterator_holder_t& src) const { 
                return _impl != static_cast<const iterator_holder_impl_t*>(&src)->_impl;
            }
            
            virtual iterator_holder_t& operator++() { 
                ++_impl; 
                return *this; 
            }
            
            virtual iterator_holder_t& operator=(iterator_holder_t* src) { 
                _impl = static_cast<iterator_holder_impl_t*>(src)->_impl; 
                return *this;
            }
            
            virtual const I& operator*() const { return _impl.operator*(); }
            virtual const I* operator->() const { return _impl.operator->(); }
            virtual iterator_holder_t* clone() const { return new (std::nothrow) iterator_holder_impl_t(_impl); }
        private:
            T _impl;
        };

    private:
        std::unique_ptr<iterator_holder_t> _impl;
    };

    class fields_spec_t {
    public:
        using iterator = iterator_container_t<field_spec_t>;
    public:
        virtual ~fields_spec_t();
        virtual size_t count() const = 0;
        virtual fields_spec_t::iterator operator[](size_t index) const = 0;
        virtual fields_spec_t::iterator find(const std::string& name) const = 0;
        virtual fields_spec_t::iterator begin() const = 0;
        virtual fields_spec_t::iterator end() const = 0;
        virtual size_t data_size() const = 0;
    };

    class field_value_t {
    public:
        virtual ~field_value_t();
        virtual const std::string& name() const = 0;
        virtual jvm_type_t type() const = 0;
        virtual size_t offset() const = 0;
        virtual operator jvm_id_t() const = 0;
        virtual operator jvm_bool_t() const = 0;
        virtual operator jvm_byte_t() const = 0;
        virtual operator jvm_char_t() const = 0;
        virtual operator jvm_short_t() const = 0;
        virtual operator jvm_float_t() const = 0;
        virtual operator jvm_double_t() const = 0;
        virtual operator jvm_int_t() const = 0;
        virtual operator jvm_long_t() const = 0;
    };

    class fields_values_t {
    public:
        using iterator = iterator_container_t<field_value_t>;
    public:
        virtual ~fields_values_t();
        virtual size_t count() const = 0;
        virtual fields_values_t::iterator operator[](size_t index) const = 0;
        virtual fields_values_t::iterator find(std::string name) const = 0;
        virtual fields_values_t::iterator begin() const = 0;
        virtual fields_values_t::iterator end() const = 0;
    };

    template<typename T>
    inline bool operator==(const iterator_container_t<T>& left, const iterator_container_t<T>& right) {
        return *left._impl == *right._impl;
    }

    template<typename T>
    inline bool operator!=(const iterator_container_t<T>& left, const iterator_container_t<T>& right) {
        return *left._impl != *right._impl;
    }

    class class_info_t : public virtual object_info_t {
    public:
        virtual ~class_info_t();
        virtual jvm_id_t super_id() const = 0;
        virtual const class_info_t* super() const = 0;
        virtual jvm_id_t name_id() const = 0;
        virtual jvm_id_t class_loader_id() const = 0;
        virtual int32_t sequence_number() const = 0;
        virtual int32_t stack_trace_id() const = 0;
        virtual const std::string& name() const = 0;
        virtual size_t name_matches(const name_tokens& name) const = 0;
        virtual size_t instance_size() const = 0;
        virtual const fields_spec_t& fields() const = 0;
        virtual const fields_values_t& static_fields() const = 0;
    };

    class instance_info_t : public virtual object_info_t {
    public:
        virtual ~instance_info_t();
        virtual jvm_id_t class_id() const = 0;
        virtual int32_t stack_trace_id() const = 0;
        virtual const class_info_t* get_class() const = 0;
        virtual const fields_values_t& fields() const = 0;
    };

    class string_info_t : public virtual instance_info_t {
    public:
        virtual ~string_info_t();
        virtual const std::string& value() const = 0;
    };

    class primitives_array_info_t : public virtual object_info_t {
    public:
        class array_item_t {
        public:
            virtual ~array_item_t();
            virtual jvm_type_t type() const = 0;
            virtual size_t offset() const = 0;
            virtual operator jvm_bool_t() const = 0;
            virtual operator jvm_byte_t() const = 0;
            virtual operator jvm_char_t() const = 0;
            virtual operator jvm_short_t() const = 0;
            virtual operator jvm_float_t() const = 0;
            virtual operator jvm_double_t() const = 0;
            virtual operator jvm_int_t() const = 0;
            virtual operator jvm_long_t() const = 0;
        };

        using iterator = iterator_container_t<array_item_t>;
    public:
        virtual ~primitives_array_info_t();

        virtual jvm_type_t item_type() const = 0;
        virtual size_t length() const = 0;
        virtual iterator begin() const = 0;
        virtual iterator end() const = 0;
        virtual iterator operator[](size_t index) const = 0;
    };

    class objects_array_info_t : public virtual object_info_t {
    public:
        using iterator = iterator_container_t<jvm_id_t>;
    public:
        virtual ~objects_array_info_t();

        virtual jvm_id_t class_id() const = 0;
        virtual size_t length() const = 0;
        virtual iterator begin() const = 0;
        virtual iterator end() const = 0;
        virtual iterator operator[](size_t index) const = 0;
    };

    class heap_item_t {
    public:
        enum type_t {
            Class,
            Object,
            String,
            PrimitivesArray,
            ObjectsArray
        };
    public:
        virtual ~heap_item_t();
        virtual type_t type() const = 0;
        virtual operator const class_info_t*() const = 0;
        virtual operator const instance_info_t*() const = 0;
        virtual operator const string_info_t*() const = 0;
        virtual operator const primitives_array_info_t*() const = 0;
        virtual operator const objects_array_info_t*() const = 0;
    };

    using heap_item_ptr_t = std::shared_ptr<heap_item_t>;
}
