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
#include "hprof_data_reader_v103.h"

#include <cassert>
#include <iostream>
#include <time.h>
#include <vector>
#include <algorithm>

using namespace hprof;
using std::unique_ptr;

unique_ptr<dump_data_t> data_reader_v103_t::build(hprof_istream& in) const {
    // Read id size
    size_t size = in.read_int32();
    if (size == 0 || in.eof()) {
        return unique_ptr<dump_data_t>();
    }

    int64_t timestamp = in.read_int64();
    if (in.eof()) {
        return unique_ptr<dump_data_t>();
    }
    auto hprof_time = std::chrono::system_clock::from_time_t(static_cast<::time_t>(timestamp / 1000));
    data_reader_v103_t::reader reader { size };

    parse_result_t result = HAS_NEXT_TOKEN;
    while (result == HAS_NEXT_TOKEN) {
        result = reader.read_next_record(in);
    }

    if (result == FAILED) {
        return unique_ptr<dump_data_t> {};
    }

    auto hprof = std::make_unique<dump_data_t>(size, hprof_time);
    hprof->add(reader.strings());
    hprof->add(reader.classes());
    hprof->add(reader.roots());
    hprof->add(reader.instances());
    hprof->add(reader.arrays());

    return hprof;
}

data_reader_v103_t::parse_result_t data_reader_v103_t::reader::read_next_reacord_header(hprof_istream& in, hprof_tag_t& tag, int32_t& time_delta, int32_t& size) {
    tag = static_cast<hprof_tag_t>(in.read_byte());
    if (in.eof()) return DONE;

    time_delta = in.read_int32();
    if (in.eof()) return FAILED;

    size = in.read_int32();
    if (in.eof()) return FAILED;

    return HAS_NEXT_TOKEN;
}

data_reader_v103_t::parse_result_t data_reader_v103_t::reader::read_next_record(hprof_istream& in) {
    hprof_tag_t tag;
    int32_t time_delta, record_size;
    parse_result_t result = read_next_reacord_header(in, tag, time_delta, record_size);
    if (result != HAS_NEXT_TOKEN) {
        return result;
    }

    switch (tag) {
        case TAG_UTF8_STRING:
            return read_utf8_string(in, record_size);
        case TAG_LOAD_CLASS:
            return read_load_class(in, record_size);
        case TAG_UNLOAD_CLASS:
            // Tag is not supported in Android
            return FAILED;
        case TAG_STACK_FRAME:
            return read_stack_frame(in, record_size);
        case TAG_STACK_TRACE:
            return read_stack_trace(in, record_size);
        case TAG_ALLOC_SITES:
        case TAG_HEAP_SUMMARY:
        case TAG_START_THREAD:
        case TAG_END_THREAD:
        case TAG_HEAP_DUMP:
            // Tags are not supported in Android
            return FAILED;
        case TAG_HEAP_DUMP_SEGMENT:
            return read_heap_dump_segment(in, record_size);
        case TAG_HEAP_DUMP_END:
            return HAS_NEXT_TOKEN;
        case TAG_CPU_SAMPLES:
        case TAG_CONTROL_SETTINGS:
            // Tags are not supported in Android
            return FAILED;

        default:
            std::cout << "Tag: " << std::hex << (int) tag << std::dec << " delta: " << time_delta << " size: " << record_size << std::endl;
            break;
    }

    return FAILED;
}

data_reader_v103_t::parse_result_t data_reader_v103_t::reader::read_utf8_string(hprof_istream& in, int32_t size) {
    if (size <= _id_size) {
        return FAILED;
    }

    id_t id = 0;
    if (read_id(in, id) == 0) {
        return FAILED;
    }

    u_int8_t text[size - _id_size + 1];
    if (!in.read_bytes(text, size - _id_size)) {
        return FAILED;
    }

    text[size - _id_size] = '\0';
    _strings.emplace(id, std::string { reinterpret_cast<char*>(text) });

    return HAS_NEXT_TOKEN;
}

data_reader_v103_t::parse_result_t data_reader_v103_t::reader::read_load_class(hprof_istream& in, int32_t size) {
    int32_t class_seq = in.read_int32();

    id_t class_id = 0;
    read_id(in, class_id);

    int32_t stack_trace_id = in.read_int32();

    id_t class_name_id = 0;
    read_id(in, class_name_id);

    if (in.eof()) return FAILED;

    // Try to find class in DUMP
    auto info = _classes.find(class_id);
    if (info != std::end(_classes)) {
        class_info_ptr_t &cls = info->second;
        cls->name_id = class_name_id;
        cls->seq_number = class_seq;
        cls->stack_trace_id = stack_trace_id;
    } else {
        // Insert new record
        assert(_loaded_classes.find(class_id) == std::end(_loaded_classes));
        _loaded_classes.emplace(class_id, loaded_class_t { class_seq, stack_trace_id, class_name_id });
    }

    return HAS_NEXT_TOKEN;
}

data_reader_v103_t::parse_result_t data_reader_v103_t::reader::read_stack_frame(hprof_istream& in, int32_t size) {
    // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#681
    std::cout << "Stack frame" << std::endl;
    for (int index = 0; index < size; ++index) {
        in.read_byte();
        if (in.eof()) return FAILED;
    }
    return HAS_NEXT_TOKEN;
}

data_reader_v103_t::parse_result_t data_reader_v103_t::reader::read_stack_trace(hprof_istream& in, int32_t size) {
    // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#706
    int32_t stack_trace_sn = in.read_int32();
    int32_t thread_id = in.read_int32();
    int32_t depth = in.read_int32();

    for (int index = 0; index < depth; ++index) {
        int32_t stack_frame_id = in.read_int32();
    }

    return HAS_NEXT_TOKEN;
}

data_reader_v103_t::parse_result_t data_reader_v103_t::reader::read_heap_dump_segment(hprof_istream& in, int32_t size) {
    heap_info_t heap_info { 0, 0 };

    std::vector<class_info_ptr_t> classes;
    std::vector<instance_info_ptr_t> instances;
    std::vector<array_info_ptr_t> arrays;
    std::vector<gc_root_t> gc_roots;

    while (size > 0) {
        auto subtype = static_cast<hprof_gc_tag_t>(in.read_byte());
        --size;
        if (in.eof()) return FAILED;

        switch (subtype) {
            case  DUMP_CLASS_DUMP: {
                auto info = std::make_shared<class_info_t>();
                if (!read_class_dump(in, size, *info)) {
                    return FAILED;
                }

                auto item = _loaded_classes.find(info->class_id);
                if (item != std::end(_loaded_classes)) {
                    info->name_id = item->second.name_id;
                    info->seq_number = item->second.class_seq;
                    _loaded_classes.erase(item);
                }

                classes.push_back(info);
                break;
            }

            case DUMP_INSTANCE_DUMP: {
                instance_info_ptr_t info;
                if (!read_instance_dump(in, size, info) || info == nullptr) {
                    return FAILED;
                }
                instances.push_back(info);
                break;
            }

            case DUMP_OBJECT_ARRAY_DUMP: {
                array_info_ptr_t info;
                if (!read_objects_array_dump(in, size, info)) {
                    return FAILED;
                }
                arrays.push_back(info);
                break;
            }

            case DUMP_PRIMITIVE_ARRAY_DUMP: {
                array_info_ptr_t info;
                if (!read_array_dump(in, size, info)) {
                    return FAILED;
                }
                arrays.push_back(info);
                break;
            }

            case DUMP_PRIMITIVE_ARRAY_NODATA_DUMP: {
                // TODO: Check http://androidxref.com/7.1.1_r6/xref/dalvik/tools/hprof-conv/HprofConv.c#554
                id_t object_id = 0;
                size -= read_id(in, object_id);
                // Skip rest 5 bytes
                u_int8_t buff[5];
                in.read_bytes(buff);

                // std::cout << "Primitive arraye no data dump: " << object_id << std::endl;
                break;
            }

            case DUMP_HEAP_DUMP_INFO: {
                heap_info.type = in.read_int32();
                size -= 4;
                size -= read_id(in, heap_info.name);
                break;
            }

            default:
                gc_root_t gc_root;
                if (read_gc_root(subtype, in, size, gc_root)) {
                    gc_roots.push_back(gc_root);
                    break;
                }

                return FAILED;
        }

        if (in.eof()) return FAILED;
    }

    std::transform(std::begin(classes), std::end(classes), std::inserter(_classes, std::end(_classes)),
        [&heap_info] (auto item) -> auto { item->set_heap_type(heap_info.type); return std::make_pair(item->id(), item); });

    std::transform(std::begin(instances), std::end(instances), std::inserter(_instances, std::end(_instances)),
        [&heap_info] (auto item) -> auto { item->set_heap_type(heap_info.type); return std::make_pair(item->id(), item); });

    std::transform(std::begin(arrays), std::end(arrays), std::inserter(_arrays, std::end(_arrays)),
        [&heap_info] (auto item) -> auto { item->set_heap_type(heap_info.type); return std::make_pair(item->id(), item); });

    std::copy(std::begin(gc_roots), std::end(gc_roots), std::back_inserter(_gc_roots));

    return HAS_NEXT_TOKEN;
}

size_t data_reader_v103_t::reader::read_id(hprof_istream& in, id_t& id) {
    switch (_id_size) {
        case 4:
            id = in.read_int32();
            break;
        case 8:
            id = in.read_int64();
            break;
        default:
            // TODO: set stream to error state
            assert(false);
    }
    return _id_size;
}

// NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#1173
bool data_reader_v103_t::reader::read_class_dump(hprof_istream& in, int32_t& data_left, class_info_t& info) {
    data_left -= read_id(in, info.class_id);
    info.stack_trace_id = in.read_int32();
    data_left -= 4;
    data_left -= read_id(in, info.super_id);
    data_left -= read_id(in, info.class_loader_id);
    // Skip 4 ids. They are not valid in dalvik anyway
    id_t skip = 0;
    data_left -= read_id(in, skip);
    data_left -= read_id(in, skip);
    data_left -= read_id(in, skip);
    data_left -= read_id(in, skip);

    info.size = in.read_int32();
    data_left -= 4;
    // pool, value is always empty
    in.read_int16();
    data_left -= 2;

    // check before reading fields that data is fine
    if (in.eof()) return false;

    int16_t static_fields_count = in.read_int16();
    data_left -= 2;

    if (static_fields_count != 0) {
        u_int8_t buffer[8] = {0};

        for (int index = 0; index < static_fields_count; ++index) {
            id_t field_name = 0;
            data_left -= read_id(in, field_name);
            auto type = static_cast<jvm_type_t>(in.read_byte());
            --data_left;

            size_t field_size = get_field_size(type);
            in.read_bytes(buffer, field_size);
            data_left -= field_size;

            info.static_fields.emplace_back(field_name, to_field_type(type), buffer, field_size);

            if (in.eof()) return false;
        }
    }


    int16_t fields_count = in.read_int16();
    data_left -= 2;

    for (size_t index = 0, offset = 0; index < fields_count; ++index) {
        id_t field_name = 0;
        data_left -= read_id(in, field_name);
        auto type = static_cast<jvm_type_t>(in.read_byte());
        --data_left;

        if (in.eof()) return false;

        info.fields.emplace_back(field_name, to_field_type(type), offset);
        offset += get_field_size(type);
    }

    return !in.eof();
}

// NOTE: Specs http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#1303
bool data_reader_v103_t::reader::read_instance_dump(hprof_istream& in, int32_t& data_left, instance_info_ptr_t& info) {
    id_t object_id;
    data_left -= read_id(in, object_id);
    int32_t stack_trace_id = in.read_int32();
    data_left -= 4;
    id_t class_id;
    data_left -= read_id(in, class_id);

    int32_t object_size = in.read_int32();
    data_left -= 4;
    data_left -= object_size;

    if (in.eof()) return false;

    auto mem = new (std::nothrow) u_int8_t[sizeof(instance_info_t) + object_size];
    info.reset(new (mem) instance_info_t(object_id, class_id, stack_trace_id, object_size), [] (auto item) { delete[] reinterpret_cast<u_int8_t*>(item); });

    in.read_bytes(info->data, object_size);

    return !in.eof();
}

// TODO: Refs http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#1263
bool data_reader_v103_t::reader::read_objects_array_dump(hprof_istream& in, int32_t& data_left, array_info_ptr_t& info) {
    id_t object_id = 0;
    data_left -= read_id(in, object_id);
    int32_t stack_trace_id = in.read_int32();
    int32_t length = in.read_int32();
    data_left -= 8;
    id_t class_id = 0;
    data_left -= read_id(in, class_id);

    if (in.eof()) return false;

    auto mem = new (std::nothrow) u_int8_t[sizeof(object_array_info_t) + (length * sizeof(id_t))];
    info.reset(new (mem) object_array_info_t(object_id, stack_trace_id, length), [] (auto item) { delete[] reinterpret_cast<u_int8_t*>(item); });

    object_array_info_t* array_info = static_cast<object_array_info_t*>(info.get());

    for (int index = 0; index < length; ++index) {
        data_left -= read_id(in, array_info->data[index]);
    }

    return !in.eof();
}

// NOTE: Ref: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#1283
bool data_reader_v103_t::reader::read_array_dump(hprof_istream& in, int32_t& data_left, array_info_ptr_t& info) {
    id_t object_id = 0;
    data_left -= read_id(in, object_id);
    int32_t stack_trace_id = in.read_int32();
    int32_t length = in.read_int32();
    auto type = static_cast<jvm_type_t>(in.read_byte());
    data_left -= 9;

    size_t array_size = length * get_field_size(type);
    auto mem = new (std::nothrow) u_int8_t[sizeof(primitive_array_info_t) + array_size];
    info.reset(new (mem) primitive_array_info_t(object_id, stack_trace_id, to_array_type(type), length), [] (auto item) { delete[] reinterpret_cast<u_int8_t*>(item); });
    data_left -= array_size;

    primitive_array_info_t* array_info = static_cast<primitive_array_info_t*>(info.get());
    in.read_bytes(reinterpret_cast<u_int8_t*>(array_info->data.byte_data), array_size);

    return !in.eof();
}

bool data_reader_v103_t::reader::read_gc_root(hprof_gc_tag_t subtype, hprof_istream& in, int32_t& data_left, gc_root_t& info) {
    switch (subtype) {
        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#969
        case DUMP_ROOT_UNKNOWN:
            info.root_type = gc_root_t::UNKNOWN;
            data_left -= read_id(in, info.object_id);
            break;

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#985
        case DUMP_ROOT_JNI_GLOBAL:
            info.root_type = gc_root_t::JNI_GLOBAL;
            data_left -= read_id(in, info.object_id);
            info.root_jni_global.jni_ref = in.read_int32();
            data_left -= 4;
            break;

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#994
        case DUMP_ROOT_JNI_LOCAL:
            info.root_type = gc_root_t::JNI_LOCAL;
            data_left -= read_id(in, info.object_id);
            info.root_jni_local.thread_seq_num = in.read_int32();
            info.root_jni_local.stack_frame_id = in.read_int32();
            data_left -= 8;
            break;

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#996
        case DUMP_ROOT_JAVA_FRAME:
            info.root_type = gc_root_t::JAVA_FRAME;
            data_left -= read_id(in, info.object_id);
            info.java_frame.thread_seq_num = in.read_int32();
            info.java_frame.stack_frame_id = in.read_int32();
            data_left -= 8;
            break;

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#1005
        case DUMP_ROOT_NATIVE_STACK:
            info.root_type = gc_root_t::NATIVE_STACK;
            data_left -= read_id(in, info.object_id);
            info.native_stack.thread_seq_num = in.read_int32();
            data_left -= 4;
            break;

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#970
        case DUMP_ROOT_STICKY_CLASS:
            info.root_type = gc_root_t::STICKY_CLASS;
            data_left -= read_id(in, info.object_id);
            break;

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#1006
        case DUMP_ROOT_THREAD_BLOCK:
            info.root_type = gc_root_t::THREAD_BLOCK;
            data_left -= read_id(in, info.object_id);
            info.thread_block.thread_seq_num = in.read_int32();
            data_left -= 4;
            break;

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#971
        case DUMP_ROOT_MONITOR_USED:
            info.root_type = gc_root_t::MONITOR_USED;
            data_left -= read_id(in, info.object_id);
            break;

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#1015
        case DUMP_ROOT_THREAD_OBJECT:
            info.root_type = gc_root_t::THREAD_OBJECT;
            data_left -= read_id(in, info.object_id);
            info.thread_object.thread_seq_num = in.read_int32();
            info.thread_object.stack_trace_id = in.read_int32();
            data_left -= 8;
            break;

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#972
        case DUMP_ROOT_INTERNED_STRING:
            info.root_type = gc_root_t::INTERNED_STRING;
            data_left -= read_id(in, info.object_id);
            break;

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#1031
        case DUMP_ROOT_FINALIZING:
            info.root_type = gc_root_t::FINALIZING;
            break;

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#973
        case DUMP_ROOT_DEBUGGER:
            info.root_type = gc_root_t::DEBUGGER;
            data_left -= read_id(in, info.object_id);
            break;

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#1032
        case DUMP_ROOT_REFERENCE_CLEANUP:
            info.root_type = gc_root_t::REFERENCE_CLEANUP;
            data_left -= read_id(in, info.object_id);
            break;

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#974
        case DUMP_ROOT_VM_INTERNAL:
            info.root_type = gc_root_t::VM_INTERNAL;
            data_left -= read_id(in, info.object_id);
            break;

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#995
        case DUMP_ROOT_JNI_MONITOR:
            info.root_type = gc_root_t::JNI_MONITOR;
            data_left -= read_id(in, info.object_id);
            info.jni_monitor.thread_seq_num = in.read_int32();
            info.jni_monitor.stack_trace_id = in.read_int32();
            data_left -= 8;
            break;

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#1033
        case DUMP_UNREACHABLE:
            info.root_type = gc_root_t::UNREACHABLE;
            break;
        default:
            return false;
    }

    return !in.eof();
}

size_t data_reader_v103_t::reader::get_field_size(jvm_type_t type) const {
    switch (type) {
        case JVM_TYPE_OBJECT:
            return _id_size;
        case JVM_TYPE_BOOL:
        case JVM_TYPE_BYTE:
            return 1;
        case JVM_TYPE_CHAR:
        case JVM_TYPE_SHORT:
            return 2;
        case JVM_TYPE_FLOAT:
        case JVM_TYPE_INT:
            return 4;
        case JVM_TYPE_DOUBLE:
        case JVM_TYPE_LONG:
            return 8;
        default:
            return 0;
    }
}

field_info_t::field_type_t data_reader_v103_t::reader::to_field_type(data_reader_v103_t::jvm_type_t type) const {
    switch (type) {
        case JVM_TYPE_OBJECT: return field_info_t::TYPE_OBJECT;
        case JVM_TYPE_BOOL: return field_info_t::TYPE_BOOL;
        case JVM_TYPE_CHAR: return field_info_t::TYPE_CHAR;
        case JVM_TYPE_FLOAT: return field_info_t::TYPE_FLOAT;
        case JVM_TYPE_DOUBLE: return field_info_t::TYPE_DOUBLE;
        case JVM_TYPE_BYTE: return field_info_t::TYPE_BYTE;
        case JVM_TYPE_SHORT: return field_info_t::TYPE_SHORT;
        case JVM_TYPE_INT: return field_info_t::TYPE_INT;
        case JVM_TYPE_LONG: return field_info_t::TYPE_LONG;
        default: return field_info_t::TYPE_UNKNOWN;
    }
}

primitive_array_info_t::array_type_t data_reader_v103_t::reader::to_array_type(data_reader_v103_t::jvm_type_t type) {
    switch (type) {
        case JVM_TYPE_BOOL: return primitive_array_info_t::TYPE_BOOL;
        case JVM_TYPE_CHAR: return primitive_array_info_t::TYPE_CHAR;
        case JVM_TYPE_FLOAT: return primitive_array_info_t::TYPE_FLOAT;
        case JVM_TYPE_DOUBLE: return primitive_array_info_t::TYPE_DOUBLE;
        case JVM_TYPE_BYTE: return primitive_array_info_t::TYPE_BYTE;
        case JVM_TYPE_SHORT: return primitive_array_info_t::TYPE_SHORT;
        case JVM_TYPE_INT: return primitive_array_info_t::TYPE_INT;
        case JVM_TYPE_LONG: return primitive_array_info_t::TYPE_LONG;
        default: return primitive_array_info_t::TYPE_INVALID;
    }
}
