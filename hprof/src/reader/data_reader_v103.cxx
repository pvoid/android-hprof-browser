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
#include "hprof.h"
#include "heap_profile.h"
#include "reader/data_reader_v103.h"

#include <vector>

#include <string>
#include <iostream>
#include <sstream>

using namespace hprof;
using std::unique_ptr;
using std::vector;
using std::unordered_map;
using std::string;

enum hprof_type_t : u_int8_t {
    HPROF_TYPE_OBJECT = 2,
    HPROF_TYPE_BOOL = 4,
    HPROF_TYPE_CHAR = 5,
    HPROF_TYPE_FLOAT = 6,
    HPROF_TYPE_DOUBLE = 7,
    HPROF_TYPE_BYTE = 8,
    HPROF_TYPE_SHORT = 9,
    HPROF_TYPE_INT = 10,
    HPROF_TYPE_LONG = 11,
};

static size_t get_field_size(hprof_type_t type, size_t id_size) {
    switch (type) {
        case HPROF_TYPE_OBJECT:
            return id_size;
        case HPROF_TYPE_BOOL:
        case HPROF_TYPE_BYTE:
            return 1;
        case HPROF_TYPE_CHAR:
        case HPROF_TYPE_SHORT:
            return 2;
        case HPROF_TYPE_FLOAT:
        case HPROF_TYPE_INT:
            return 4;
        case HPROF_TYPE_DOUBLE:
        case HPROF_TYPE_LONG:
            return 8;
    }
    assert(false);
}

static jvm_type_t to_jvm_type(hprof_type_t type) {
    switch (type) {
        case HPROF_TYPE_OBJECT: return jvm_type_t::JVM_TYPE_OBJECT;
        case HPROF_TYPE_BOOL:   return jvm_type_t::JVM_TYPE_BOOL;
        case HPROF_TYPE_CHAR:   return jvm_type_t::JVM_TYPE_CHAR;
        case HPROF_TYPE_FLOAT:  return jvm_type_t::JVM_TYPE_FLOAT;
        case HPROF_TYPE_DOUBLE: return jvm_type_t::JVM_TYPE_DOUBLE;
        case HPROF_TYPE_BYTE:   return jvm_type_t::JVM_TYPE_BYTE;
        case HPROF_TYPE_SHORT:  return jvm_type_t::JVM_TYPE_SHORT;
        case HPROF_TYPE_INT:    return jvm_type_t::JVM_TYPE_INT;
        case HPROF_TYPE_LONG:   return jvm_type_t::JVM_TYPE_LONG;
    }
    return jvm_type_t::JVM_TYPE_UNKNOWN;
}

unique_ptr<heap_profile_t> data_reader_v103_t::build(hprof_istream_t& in) const {
    heap_profile_data_t data;
    // Read id size
    data.id_size = static_cast<u_int8_t>(in.read_int32());
    if (data.id_size == 0 || in.eof()) {
        return std::make_unique<heap_profile_impl_t>("Can't read id size from heap file");
    }

    int64_t timestamp = in.read_int64();
    if (in.eof()) {
        return std::make_unique<heap_profile_impl_t>("Can't read timestamp from heap file");
    }

    auto hprof_time = std::chrono::system_clock::from_time_t(static_cast<::time_t>(timestamp / 1000));

    hprof_tag_t tag;
    int32_t time_delta;
    int32_t section_size;

    read_token_result_t read_result;
    do {
        read_result = next_record(in, tag, time_delta, section_size);
        switch (read_result) {
            case HAS_NEXT_TOKEN: {
                hprof_section_reader reader { in, data.id_size, static_cast<size_t>(section_size) };

                if (process_next_token(tag, reader, data)) {
                    continue;
                }
                std::stringstream message;
                message << "Failed processing section: 0x" << std::hex << tag;
                return std::make_unique<heap_profile_impl_t>(message.str());
            }
            case FAILED: {
                return std::make_unique<heap_profile_impl_t>("Unexpected end of file");
            }
            case DONE:
                break;
        }
    } while(read_result != DONE);

    auto result = std::make_unique<heap_profile_impl_t>(std::move(data.gc_roots));
    if (!prepare(data, *result)) return std::make_unique<heap_profile_impl_t>("Error occuried while perapring data");
    return result;
}

data_reader_v103_t::read_token_result_t data_reader_v103_t::next_record(hprof_istream_t& in, hprof_tag_t& tag, int32_t& time_delta, int32_t& size) const {
    tag = static_cast<hprof_tag_t>(in.read_byte());
    if (in.eof()) return DONE;

    time_delta = in.read_int32();
    if (in.eof()) return FAILED;

    size = in.read_int32();
    if (in.eof()) return FAILED;

    return HAS_NEXT_TOKEN;
}

bool data_reader_v103_t::process_next_token(hprof_tag_t tag, hprof_section_reader& reader, heap_profile_data_t& data) const {
    switch (tag) {
        case TAG_UTF8_STRING:
            return read_utf8_string(reader, data);
        case TAG_LOAD_CLASS:
            return read_load_class(reader, data);
        case TAG_UNLOAD_CLASS:
            // Tag is not supported in Android
            return false;
        case TAG_STACK_FRAME:
            return read_stack_frame(reader, data);
        case TAG_STACK_TRACE:
            return read_stack_trace(reader, data);
        case TAG_ALLOC_SITES:
        case TAG_HEAP_SUMMARY:
        case TAG_START_THREAD:
        case TAG_END_THREAD:
        case TAG_HEAP_DUMP:
            // Tags are not supported in Android
            return false;
        case TAG_HEAP_DUMP_SEGMENT:
            return read_heap_dump_segment(reader, data);
        case TAG_HEAP_DUMP_END:
            return true;
        case TAG_CPU_SAMPLES:
        case TAG_CONTROL_SETTINGS:
            // Tags are not supported in Android
            return false;

        default:
            std::cout << "Tag: " << std::hex << (int) tag << std::dec << " size: " << reader.data_left() << std::endl;
            break;
    }

    return false;
}

bool data_reader_v103_t::read_utf8_string(hprof_section_reader& reader, heap_profile_data_t& data) const {
    jvm_id_t id = reader.read_id();
    if (id == 0) {
        return false;
    }

    size_t length = reader.data_left();
    u_int8_t text[length + 1]; //FIXME: Possible heap overflow problem
    if (!reader.read_bytes(text, length)) {
        return false;
    }

    text[length] = '\0';
    data.strings.emplace(id, std::string { reinterpret_cast<char*>(text) });

    return true;
}

bool data_reader_v103_t::read_load_class(hprof_section_reader& reader, heap_profile_data_t& data) const {
    int32_t class_seq = reader.read_int32();

    jvm_id_t class_id = reader.read_id();
    int32_t stack_trace_id = reader.read_int32();
    jvm_id_t class_name_id = reader.read_id();

    if (reader.is_error_occurred()) return false;

    data.loaded_class.emplace(class_id, loaded_class_t { class_seq, stack_trace_id, class_name_id });

    return true;
}

bool data_reader_v103_t::read_stack_frame(hprof_section_reader& reader, heap_profile_data_t&) const {
    // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#681
    std::cout << "Stack frame" << std::endl;
    reader.skip_all();
    return !reader.is_error_occurred();
}

bool data_reader_v103_t::read_stack_trace(hprof_section_reader& reader, heap_profile_data_t&) const {
    // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#706
    /*int32_t stack_trace_sn = */ reader.read_int32();
    /* int32_t thread_id = */ reader.read_int32();
    int32_t depth = reader.read_int32();

    for (int index = 0; index < depth; ++index) {
        if (reader.is_error_occurred()) return false;
        /*int32_t stack_frame_id = */ reader.read_int32();
    }

    return !reader.is_error_occurred();
}

bool data_reader_v103_t::read_heap_dump_segment(hprof_section_reader& reader, heap_profile_data_t& data) const {
    heap_info_t heap_info { 0, 0 };

    size_t index_instances = data.instances.size();
    size_t index_primitives_arrays = data.primitives_arrays.size();
    size_t index_objects_arrays = data.objects_arrays.size();
    size_t index_classes = data.classes.size();

    while (reader.has_more_data()) {
        auto subtype = static_cast<hprof_gc_tag_t>(reader.read_byte());
        if (reader.is_error_occurred()) return false;

        switch (subtype) {
            case  DUMP_CLASS_DUMP: {
                if (!read_class_dump(reader, data.id_size, data.strings, data.classes)) {
                    return FAILED;
                }
                break;
            }

            case DUMP_INSTANCE_DUMP: {
                if (!read_instance_dump(reader, data.id_size, data.instances)) {
                    return false;
                }
                break;
            }

            case DUMP_OBJECT_ARRAY_DUMP: {
                if (!read_objects_array_dump(reader, data.id_size, data.objects_arrays)) {
                    return false;
                }
                break;
            }

            case DUMP_PRIMITIVE_ARRAY_DUMP: {
                if (!read_primitives_array_dump(reader, data.id_size, data.primitives_arrays)) {
                    return false;
                }
                break;
            }

            case DUMP_PRIMITIVE_ARRAY_NODATA_DUMP: {
                // FIXME: Check http://androidxref.com/7.1.1_r6/xref/dalvik/tools/hprof-conv/HprofConv.c#554
                /*jvm_id_t object_id = */reader.read_id();
                // Skip rest 5 bytes
                u_int8_t buff[5];
                reader.read_bytes(buff);
                break;
            }

            case DUMP_HEAP_DUMP_INFO: {
                heap_info.type = reader.read_int32();
                heap_info.name = reader.read_id();
                break;
            }

            default:
                if (!read_gc_root(subtype, reader, data.gc_roots)) {
                    return false;
                }
                break;
        }
    }

    for (; index_instances < data.instances.size(); ++index_instances) {
        data.instances[index_instances]->set_heap_type(heap_info.type);
    }

    for (; index_classes < data.classes.size(); ++index_classes) {
        data.classes[index_classes]->set_heap_type(heap_info.type);
    }

    for (; index_primitives_arrays < data.primitives_arrays.size(); ++index_primitives_arrays) {
        data.primitives_arrays[index_primitives_arrays]->set_heap_type(heap_info.type);
    }

    for (; index_objects_arrays < data.objects_arrays.size(); ++index_objects_arrays) {
        data.objects_arrays[index_objects_arrays]->set_heap_type(heap_info.type);
    }

    return true;
}

// NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#1173
bool data_reader_v103_t::read_class_dump(hprof_section_reader& reader, u_int8_t id_size, 
                const std::unordered_map<jvm_id_t, std::string>& strings, std::vector<class_info_impl_ptr_t>& classes) const {
    jvm_id_t class_id = reader.read_id();
    int32_t stack_id = reader.read_int32();
    jvm_id_t super_id = reader.read_id();
    jvm_id_t class_loader_id = reader.read_id();

    // Skip 4 ids. They are not valid in dalvik anyway
    reader.read_id();
    reader.read_id();
    reader.read_id();
    reader.read_id();

    size_t instance_size = static_cast<size_t>(reader.read_int32());
    // pool, value is always empty
    reader.read_int16();

    // check before reading fields that data is fine
    if (reader.is_error_occurred()) return false;

    std::unique_ptr<u_int8_t[]> buffer;
    std::vector<field_spec_impl_t> static_fields;
    size_t data_size = 0;

    int16_t static_fields_count = reader.read_int16();
    if (static_fields_count != 0) {
        buffer.reset(new (std::nothrow) u_int8_t[reader.data_left()]);
        u_int8_t *pointer = buffer.get();

        for (int index = 0; index < static_fields_count; ++index) {
            jvm_id_t field_name_id = reader.read_id();
            auto field_type = static_cast<hprof_type_t>(reader.read_byte());
            size_t field_size = get_field_size(field_type, id_size);

            reader.read_bytes(pointer, field_size);
            if (reader.is_error_occurred()) return false;
            
            field_spec_impl_t field { field_name_id, to_jvm_type(field_type), static_cast<size_t>(pointer - buffer.get()) };
            auto name = strings.find(field_name_id);
            if (name != std::end(strings)) {
                field.set_name(name->second);
            }

            static_fields.push_back(field);
            pointer += field_size;
        }

        data_size = static_cast<size_t>(pointer - buffer.get());
    }

    auto klass = class_info_impl_t::create(id_size, class_id, data_size);
    klass->set_super_id(super_id);
    klass->set_class_loader_id(class_loader_id);
    klass->set_instance_size(instance_size);
    klass->set_stack_trace_id(stack_id);

    if (!static_fields.empty()) {
        for (auto& field : static_fields) {
            klass->add_static_field(field);
        }
        std::memcpy(klass->data(), buffer.get(), data_size);
    }

    size_t fields_count = static_cast<size_t>(reader.read_int16());
    size_t offset = 0;
    for (size_t index = 0; index < fields_count; ++index) {
        jvm_id_t field_name_id = reader.read_id();
        auto field_type = static_cast<hprof_type_t>(reader.read_byte());
        if (reader.is_error_occurred()) return false;

        field_spec_impl_t field { field_name_id, to_jvm_type(field_type), offset };
        auto name = strings.find(field_name_id);
        if (name != std::end(strings)) {
            field.set_name(name->second);
        }

        klass->add_field(field);
        offset += get_field_size(field_type, id_size);
    }

    classes.push_back(std::move(klass));

    return true;
}

// NOTE: Specs http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#1303
bool data_reader_v103_t::read_instance_dump(hprof_section_reader& reader, u_int8_t id_size, std::vector<instance_info_impl_ptr_t>& objects) const {
    jvm_id_t object_id = reader.read_id();
    int32_t stack_trace_id = reader.read_int32();
    jvm_id_t class_id = reader.read_id();
    size_t object_size = static_cast<size_t>(reader.read_int32());

    if (reader.is_error_occurred() || reader.data_left() < object_size) return false;

    auto result = instance_info_impl_t::create(id_size, object_id, object_size);
    if (result == nullptr) return false;

    result->set_class_id(class_id);
    result->set_stack_trace_id(stack_trace_id);

    reader.read_bytes(result->data(), object_size);
    if (reader.is_error_occurred()) return false;

    objects.push_back(std::move(result));

    return true;
}

// TODO: Refs http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#1263
bool data_reader_v103_t::read_objects_array_dump(hprof_section_reader& reader, u_int8_t id_size, std::vector<objects_array_info_impl_ptr_t>& objects) const {
    jvm_id_t object_id = reader.read_id();
    int32_t stack_trace_id = reader.read_int32();
    size_t length = static_cast<size_t>(reader.read_int32());
    jvm_id_t class_id = reader.read_id();

    if (reader.is_error_occurred()) return false;

    size_t array_size = length * id_size;
    auto result = objects_array_info_impl_t::create(id_size, object_id, class_id, length, array_size);
    if (result == nullptr) return false;

    reader.read_bytes(result->data(), array_size);
    if (reader.is_error_occurred()) return false;

    objects.push_back(std::move(result));

    return true;
}

// NOTE: Ref: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#1283
bool data_reader_v103_t::read_primitives_array_dump(hprof_section_reader& reader, u_int8_t id_size, std::vector<primitives_array_info_impl_ptr_t>& objects) const {
    jvm_id_t object_id = reader.read_id();
    int32_t stack_trace_id = reader.read_int32();
    size_t length = static_cast<size_t>(reader.read_int32());
    auto type = static_cast<hprof_type_t>(reader.read_byte());

    if (reader.is_error_occurred()) return false;

    size_t array_size = length * get_field_size(type, id_size);

    auto result = primitives_array_info_impl_t::create(id_size, object_id,  to_jvm_type(type), length, array_size);
    if (result == nullptr) return false;

    reader.read_bytes(result->data(), array_size);
    if (reader.is_error_occurred()) return false;

    objects.push_back(std::move(result));

    return true;
}

bool data_reader_v103_t::read_gc_root(hprof_gc_tag_t subtype, hprof_section_reader& reader, std::vector<gc_root_impl_ptr_t>& roots) const {
    switch (subtype) {
        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#969
        case DUMP_ROOT_UNKNOWN: {
            jvm_id_t object_id = reader.read_id();
            if (reader.is_error_occurred()) return false;
            auto value = gc_root_impl_t::create<gc_root_t::UNKNOWN>(object_id);
            roots.push_back(std::make_unique<gc_root_impl_t>(value));
            return true;
        }
        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#985
        case DUMP_ROOT_JNI_GLOBAL: {
            jvm_id_t object_id = reader.read_id();
            int32_t jni_ref = reader.read_int32();
            if (reader.is_error_occurred()) return false;
            auto value = gc_root_impl_t::create<gc_root_t::JNI_GLOBAL>(object_id, jni_ref);
            roots.push_back(std::make_unique<gc_root_impl_t>(value));
            return true;
        }

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#994
        case DUMP_ROOT_JNI_LOCAL: {
            jvm_id_t object_id = reader.read_id();
            int32_t thread_seq_num = reader.read_int32();
            int32_t stack_frame_id = reader.read_int32();
            if (reader.is_error_occurred()) return false;
            auto value = gc_root_impl_t::create<gc_root_t::JNI_LOCAL>(object_id, thread_seq_num, stack_frame_id);
            roots.push_back(std::make_unique<gc_root_impl_t>(value));
            return true;
        }

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#996
        case DUMP_ROOT_JAVA_FRAME: {
            jvm_id_t object_id = reader.read_id();
            int32_t thread_seq_num = reader.read_int32();
            int32_t stack_frame_id = reader.read_int32();
            if (reader.is_error_occurred()) return false;
            auto value = gc_root_impl_t::create<gc_root_t::JAVA_FRAME>(object_id, thread_seq_num, stack_frame_id);
            roots.push_back(std::make_unique<gc_root_impl_t>(value));
            return true;
        }

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#1005
        case DUMP_ROOT_NATIVE_STACK: {
            jvm_id_t object_id = reader.read_id();
            int32_t thread_seq_num = reader.read_int32();
            if (reader.is_error_occurred()) return false;
            auto value = gc_root_impl_t::create<gc_root_t::NATIVE_STACK>(object_id, thread_seq_num);
            roots.push_back(std::make_unique<gc_root_impl_t>(value));
            return true;
        }

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#970
        case DUMP_ROOT_STICKY_CLASS: {
            jvm_id_t object_id = reader.read_id();
            if (reader.is_error_occurred()) return false;
            auto value = gc_root_impl_t::create<gc_root_t::STICKY_CLASS>(object_id);
            roots.push_back(std::make_unique<gc_root_impl_t>(value));
            return true;
        }

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#1006
        case DUMP_ROOT_THREAD_BLOCK: {
            jvm_id_t object_id = reader.read_id();
            int32_t thread_seq_num = reader.read_int32();
            if (reader.is_error_occurred()) return false;
            auto value = gc_root_impl_t::create<gc_root_t::THREAD_BLOCK>(object_id, thread_seq_num);
            roots.push_back(std::make_unique<gc_root_impl_t>(value));
            return true;
        }

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#971
        case DUMP_ROOT_MONITOR_USED: {
            jvm_id_t object_id = reader.read_id();
            if (reader.is_error_occurred()) return false;
            auto value = gc_root_impl_t::create<gc_root_t::MONITOR_USED>(object_id);
            roots.push_back(std::make_unique<gc_root_impl_t>(value));
            return true;
        }

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#1015
        case DUMP_ROOT_THREAD_OBJECT: {
            jvm_id_t object_id = reader.read_id();
            int32_t thread_seq_num = reader.read_int32();
            int32_t stack_frame_id = reader.read_int32();
            if (reader.is_error_occurred()) return false;
            auto value = gc_root_impl_t::create<gc_root_t::THREAD_OBJECT>(object_id, thread_seq_num, stack_frame_id);
            roots.push_back(std::make_unique<gc_root_impl_t>(value));
            return true;
        }

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#972
        case DUMP_ROOT_INTERNED_STRING: {
            jvm_id_t object_id = reader.read_id();
            if (reader.is_error_occurred()) return false;
            auto value = gc_root_impl_t::create<gc_root_t::INTERNED_STRING>(object_id);
            roots.push_back(std::make_unique<gc_root_impl_t>(value));
            return true;
        }

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#1031
        case DUMP_ROOT_FINALIZING: {
            auto value = gc_root_impl_t::create<gc_root_t::FINALIZING>();
            roots.push_back(std::make_unique<gc_root_impl_t>(value));
            return true;
        }

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#973
        case DUMP_ROOT_DEBUGGER: {
            jvm_id_t object_id = reader.read_id();
            if (reader.is_error_occurred()) return false;
            auto value = gc_root_impl_t::create<gc_root_t::DEBUGGER>(object_id);
            roots.push_back(std::make_unique<gc_root_impl_t>(value));
            return true;
        }

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#1032
        case DUMP_ROOT_REFERENCE_CLEANUP: {
            auto value = gc_root_impl_t::create<gc_root_t::REFERENCE_CLEANUP>();
            roots.push_back(std::make_unique<gc_root_impl_t>(value));
            return true;
        }

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#974
        case DUMP_ROOT_VM_INTERNAL: {
            jvm_id_t object_id = reader.read_id();
            if (reader.is_error_occurred()) return false;
            auto value = gc_root_impl_t::create<gc_root_t::VM_INTERNAL>(object_id);
            roots.push_back(std::make_unique<gc_root_impl_t>(value));
            return true;
        }

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#995
        case DUMP_ROOT_JNI_MONITOR: {
            jvm_id_t object_id = reader.read_id();
            int32_t thread_seq_num = reader.read_int32();
            int32_t stack_frame_id = reader.read_int32();
            if (reader.is_error_occurred()) return false;
            auto value =  gc_root_impl_t::create<gc_root_t::JNI_MONITOR>(object_id, thread_seq_num, stack_frame_id);
            roots.push_back(std::make_unique<gc_root_impl_t>(value));
            return true;
        }

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#1033
        case DUMP_UNREACHABLE: {
            auto value = gc_root_impl_t::create<gc_root_t::UNREACHABLE>();
            roots.push_back(std::make_unique<gc_root_impl_t>(value));
            return true;
        }

        default:
            return false;
    }
}

// TODO: set roots for objects
bool data_reader_v103_t::prepare(heap_profile_data_t& data, heap_profile_impl_t& hprof) const {
    jvm_id_t string_class_id = 0;

    std::vector<heap_item_impl_ptr_t> classes;
    // Attach class name to each class and build map
    for (auto& klass : data.classes) {
        auto class_info = data.loaded_class.find(klass->id());
        if (class_info != std::end(data.loaded_class)) {
            auto name = data.strings.find(class_info->second.name_id);
            if (name != std::end(data.strings)) {
                klass->set_name(name->second);
                if ("java.lang.String" == name->second) {
                    string_class_id = klass->id();
                }
            }
        }

        jvm_id_t id = klass->id();
        auto ptr = std::make_shared<heap_item_impl_t>(std::move(klass));
        classes.push_back(ptr);

        hprof.add(id, ptr);
    }

    // Attach super class
    for (auto& item : classes) {
        auto cls = static_cast<class_info_impl_t *>(*item);

        if (cls->super_id() == 0) continue;

        auto super = hprof.find_class(cls->super_id());
        if (super == nullptr) continue;

        cls->set_super_class(super);
    }

    for (auto& array : data.primitives_arrays) {
        jvm_id_t id = array->id();
        hprof.add(id, std::make_shared<heap_item_impl_t>(std::move(array)));
    }

    // Attach classes to instances and store items
    for (auto& object : data.instances) {
        auto klass = hprof.find_class(object->class_id());
        if (klass != nullptr) {
            object->set_class(klass);
        }

        jvm_id_t id = object->id();
        if (object->class_id() == string_class_id) {
            auto str = string_info_impl_t::create(*object, hprof);
            hprof.add(id, std::make_shared<heap_item_impl_t>(std::move(str)));
            object.reset(nullptr);
        } else {
            hprof.add(id, std::make_shared<heap_item_impl_t>(std::move(object)));
        }
    }

    for (auto& array : data.objects_arrays) {
        jvm_id_t id = array->id();
        hprof.add(id, std::make_shared<heap_item_impl_t>(std::move(array)));
    }

    return true;
}
