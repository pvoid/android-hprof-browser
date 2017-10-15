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

#include <gmock/gmock.h>

#include "types.h"
#include "filters/base.h"
#include "filters/field_fetcher.h"

using namespace hprof;

class mock_field_spec_t : public field_spec_t {
public:
    virtual ~mock_field_spec_t() {}
    MOCK_CONST_METHOD0(name, const std::string&());
    MOCK_CONST_METHOD0(name_id, jvm_id_t());
    MOCK_CONST_METHOD0(type, jvm_type_t());
    MOCK_CONST_METHOD0(offset, size_t());
};

class mock_fields_iterator_helper_t;

class mock_fields_iterator_t {
public:
    virtual ~mock_fields_iterator_t() {}
    MOCK_METHOD0(increment, void());
    MOCK_METHOD1(assign, void(const mock_fields_iterator_helper_t&));
    MOCK_CONST_METHOD0(ref, const field_spec_t&());
    MOCK_CONST_METHOD0(link, const field_spec_t*());
    MOCK_CONST_METHOD1(equals, bool(const mock_fields_iterator_helper_t&));
    MOCK_CONST_METHOD1(not_equals, bool(const mock_fields_iterator_helper_t&));
};

class mock_fields_iterator_helper_t {
    friend class iterator_impl_matcher_t;
public:
    explicit mock_fields_iterator_helper_t(mock_fields_iterator_t* impl) : _impl(impl) {}

    mock_fields_iterator_helper_t& operator++() { _impl->increment(); return *this; }
    virtual mock_fields_iterator_helper_t& operator=(const mock_fields_iterator_helper_t& src) { _impl->assign(src); return *this; }
    virtual const field_spec_t& operator*() const { return _impl->ref(); }
    const field_spec_t* operator->() const { return _impl->link(); }
    bool operator==(const mock_fields_iterator_helper_t&src) const { return _impl->equals(src); }
    bool operator!=(const mock_fields_iterator_helper_t&src) const { return _impl->not_equals(src); }
private:
    mock_fields_iterator_t* _impl;
};

struct iterator_impl_matcher_t {
public:
    explicit iterator_impl_matcher_t(mock_fields_iterator_t* impl) : _impl(impl) {}
    bool operator()(const mock_fields_iterator_helper_t& src) const { return _impl == src._impl; }
private:
    mock_fields_iterator_t* _impl;
};

class mock_fields_spec_t : public fields_spec_t {
public:
    virtual ~mock_fields_spec_t() {}

    MOCK_CONST_METHOD0(count, size_t());
    MOCK_CONST_METHOD1(find, fields_spec_t::iterator(const std::string& name));
    MOCK_CONST_METHOD0(begin, fields_spec_t::iterator());
    MOCK_CONST_METHOD0(end, fields_spec_t::iterator());
    MOCK_CONST_METHOD0(data_size, size_t());
    MOCK_CONST_METHOD1(get_by_index, fields_spec_t::iterator(size_t));
    
    virtual fields_spec_t::iterator operator[](size_t index) const override { return get_by_index(index); }
};

class mock_instance_info_t : public instance_info_t {
public:
    virtual ~mock_instance_info_t() {}
    MOCK_CONST_METHOD0(id, jvm_id_t());
    MOCK_CONST_METHOD0(id_size, u_int8_t());
    MOCK_CONST_METHOD0(heap_type, int32_t());
    MOCK_CONST_METHOD1(has_link_to, int32_t (jvm_id_t));
    MOCK_CONST_METHOD0(gc_roots, const std::vector<std::unique_ptr<gc_root_t>>&());

    MOCK_CONST_METHOD0(class_id, jvm_id_t());
    MOCK_CONST_METHOD0(get_class, class_info_t*());
    MOCK_CONST_METHOD0(fields, fields_values_t&());
    MOCK_CONST_METHOD0(stack_trace_id, int32_t());
};

class mock_string_info_t : public string_info_t {
public:
    virtual ~mock_string_info_t() {}
    MOCK_CONST_METHOD0(id, jvm_id_t());
    MOCK_CONST_METHOD0(id_size, u_int8_t());
    MOCK_CONST_METHOD0(heap_type, int32_t());
    MOCK_CONST_METHOD1(has_link_to, int32_t (jvm_id_t));
    MOCK_CONST_METHOD0(gc_roots, const std::vector<std::unique_ptr<gc_root_t>>&());

    MOCK_CONST_METHOD0(class_id, jvm_id_t());
    MOCK_CONST_METHOD0(get_class, class_info_t*());
    MOCK_CONST_METHOD0(fields, fields_values_t&());
    MOCK_CONST_METHOD0(stack_trace_id, int32_t());
    MOCK_CONST_METHOD0(value, const std::string&());
};

class mock_class_info_t : public class_info_t {
public:
    virtual ~mock_class_info_t() {}
    MOCK_CONST_METHOD0(id, jvm_id_t());
    MOCK_CONST_METHOD0(id_size, u_int8_t());
    MOCK_CONST_METHOD0(heap_type, int32_t());
    MOCK_CONST_METHOD1(has_link_to, int32_t (jvm_id_t));
    MOCK_CONST_METHOD0(gc_roots, const std::vector<std::unique_ptr<gc_root_t>>&());

    MOCK_CONST_METHOD0(super_id, jvm_id_t());
    MOCK_CONST_METHOD0(super, const class_info_t*());
    MOCK_CONST_METHOD0(name_id, jvm_id_t());
    MOCK_CONST_METHOD0(class_loader_id, jvm_id_t());
    MOCK_CONST_METHOD0(name, const std::string&());
    MOCK_CONST_METHOD1(name_matches, size_t(const name_tokens& name));
    MOCK_CONST_METHOD0(instance_size, size_t());
    MOCK_CONST_METHOD0(fields, const fields_spec_t&());
    MOCK_CONST_METHOD0(static_fields, const fields_values_t&());
    MOCK_CONST_METHOD0(sequence_number, int32_t());
    MOCK_CONST_METHOD0(stack_trace_id, int32_t());
};

class mock_primitives_array_t : public primitives_array_info_t {
public:
    virtual ~mock_primitives_array_t() {}
    MOCK_CONST_METHOD0(id, jvm_id_t());
    MOCK_CONST_METHOD0(id_size, u_int8_t());
    MOCK_CONST_METHOD0(heap_type, int32_t());
    MOCK_CONST_METHOD1(has_link_to, int32_t (jvm_id_t));
    MOCK_CONST_METHOD0(gc_roots, const std::vector<std::unique_ptr<gc_root_t>>&());

    MOCK_CONST_METHOD0(item_type, jvm_type_t());
    MOCK_CONST_METHOD0(length, size_t());
    MOCK_CONST_METHOD0(begin, iterator());
    MOCK_CONST_METHOD0(end, iterator());
    
    MOCK_CONST_METHOD1(access_by_index, iterator(size_t));
    virtual iterator operator[](size_t index) const override {
        return access_by_index(index);
    }
};

class mock_objects_array_t : public objects_array_info_t {
public:
    virtual ~mock_objects_array_t() {}
    MOCK_CONST_METHOD0(id, jvm_id_t());
    MOCK_CONST_METHOD0(id_size, u_int8_t());
    MOCK_CONST_METHOD0(heap_type, int32_t());
    MOCK_CONST_METHOD1(has_link_to, int32_t (jvm_id_t));
    MOCK_CONST_METHOD0(gc_roots, const std::vector<std::unique_ptr<gc_root_t>>&());

    MOCK_CONST_METHOD0(class_id, jvm_id_t());
    MOCK_CONST_METHOD0(length, size_t());
    MOCK_CONST_METHOD0(begin, iterator());
    MOCK_CONST_METHOD0(end, iterator());

    MOCK_CONST_METHOD1(access_by_index, iterator(size_t));
    virtual iterator operator[](size_t index) const override {
        return access_by_index(index);
    }
};

class mock_heap_item_t : public heap_item_t {
public:
    MOCK_CONST_METHOD0(type, type_t());
    MOCK_CONST_METHOD0(as_class, const class_info_t*());
    MOCK_CONST_METHOD0(as_instance, const instance_info_t*());
    MOCK_CONST_METHOD0(as_string, const string_info_t*());
    MOCK_CONST_METHOD0(as_primitives_array, const primitives_array_info_t*());
    MOCK_CONST_METHOD0(as_objects_array, const objects_array_info_t*());

    virtual operator const class_info_t*() const override { return as_class(); }
    virtual operator const instance_info_t*() const override { return as_instance(); }
    virtual operator const string_info_t*() const override { return as_string(); }
    virtual operator const primitives_array_info_t*() const override { return as_primitives_array(); }
    virtual operator const objects_array_info_t*() const override { return as_objects_array(); }
};

class mock_filter_t : public filter_t {
public:
    virtual ~mock_filter_t() {}

    MOCK_CONST_METHOD2(apply_filter, filter_result_t(const heap_item_ptr_t&, const objects_index_t&));

    virtual filter_result_t operator()(const heap_item_ptr_t& item, const objects_index_t& objects) const override {
        return apply_filter(item, objects);
    }
};

class mock_objects_index_t : public objects_index_t {
public:
    MOCK_CONST_METHOD1(find_object, heap_item_ptr_t(jvm_id_t id));
};
