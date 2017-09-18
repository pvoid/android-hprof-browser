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

data_reader_v103_t::parse_result_t data_reader_v103_t::reader::read_utf8_string(hprof_istream& in, ssize_t size) {
    if (size <= _id_size) {
        return FAILED;
    }

    id_t id = read_id(in, size);
    if (id == 0) {
        return FAILED;
    }

    u_int8_t text[size + 1];
    if (!in.read_bytes(text, size)) {
        return FAILED;
    }

    text[size] = '\0';
    _strings.emplace(id, std::string { reinterpret_cast<char*>(text) });

    return HAS_NEXT_TOKEN;
}

data_reader_v103_t::parse_result_t data_reader_v103_t::reader::read_load_class(hprof_istream& in, ssize_t size) {
    int32_t class_seq = in.read_int32();

    id_t class_id = read_id(in, size);
    int32_t stack_trace_id = in.read_int32();
    id_t class_name_id = read_id(in, size);

    if (in.eof()) return FAILED;

    // Try to find class in DUMP
    auto info = _classes.find(class_id);
    if (info != std::end(_classes)) {
        class_info_impl_ptr_t &cls = info->second;
        cls->set_name_id(class_name_id);
        cls->set_seq_number(class_seq);
        cls->set_stack_trace_id(stack_trace_id);
    } else {
        // Insert new record
        assert(_loaded_classes.find(class_id) == std::end(_loaded_classes));
        _loaded_classes.emplace(class_id, loaded_class_t { class_seq, stack_trace_id, class_name_id });
    }

    return HAS_NEXT_TOKEN;
}

data_reader_v103_t::parse_result_t data_reader_v103_t::reader::read_stack_frame(hprof_istream& in, ssize_t size) {
    // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#681
    std::cout << "Stack frame" << std::endl;
    for (int index = 0; index < size; ++index) {
        in.read_byte();
        if (in.eof()) return FAILED;
    }
    return HAS_NEXT_TOKEN;
}

data_reader_v103_t::parse_result_t data_reader_v103_t::reader::read_stack_trace(hprof_istream& in, ssize_t size) {
    // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#706
    int32_t stack_trace_sn = in.read_int32();
    int32_t thread_id = in.read_int32();
    int32_t depth = in.read_int32();

    for (int index = 0; index < depth; ++index) {
        int32_t stack_frame_id = in.read_int32();
    }

    return HAS_NEXT_TOKEN;
}

data_reader_v103_t::parse_result_t data_reader_v103_t::reader::read_heap_dump_segment(hprof_istream& in, ssize_t size) {
    heap_info_t heap_info { 0, 0 };

    std::vector<class_info_impl_ptr_t> classes;
    std::vector<instance_info_impl_ptr_t> instances;
    std::vector<array_info_ptr_t> arrays;
    std::vector<gc_root_t> gc_roots;

    while (size > 0) {
        auto subtype = static_cast<hprof_gc_tag_t>(in.read_byte());
        --size;
        if (in.eof()) return FAILED;

        switch (subtype) {
            case  DUMP_CLASS_DUMP: {
                auto info = std::make_shared<class_info_impl_t>();
                if (!read_class_dump(in, size, *info)) {
                    return FAILED;
                }

                auto item = _loaded_classes.find(info->id());
                if (item != std::end(_loaded_classes)) {
                    info->set_name_id(item->second.name_id);
                    info->set_seq_number(item->second.class_seq);
                    _loaded_classes.erase(item);
                }

                classes.push_back(info);
                break;
            }

            case DUMP_INSTANCE_DUMP: {
                instance_info_impl_ptr_t info;
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
                id_t object_id = read_id(in, size);
                // Skip rest 5 bytes
                u_int8_t buff[5];
                in.read_bytes(buff);

                // std::cout << "Primitive arraye no data dump: " << object_id << std::endl;
                break;
            }

            case DUMP_HEAP_DUMP_INFO: {
                heap_info.type = in.read_int32();
                heap_info.name = read_id(in, size);
                size -= 4;
                break;
            }

            default:
                gc_root_t gc_root = read_gc_root(subtype, in, size);
                if (gc_root.type() != gc_root_t::INVALID) {
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

hprof::id_t data_reader_v103_t::reader::read_id(hprof_istream& in, ssize_t& data_left) {
    id_t result = 0;
    switch (_id_size) {
        case 4:
            result = in.read_int32();
            break;
        case 8:
            result = in.read_int64();
            break;
        default:
            // TODO: set stream to error state
            assert(false);
    }
    data_left -= _id_size;
    return result;
}

// NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#1173
bool data_reader_v103_t::reader::read_class_dump(hprof_istream& in, ssize_t& data_left, class_info_impl_t& info) {
    info.set_class_id(read_id(in, data_left));

    info.set_stack_trace_id(in.read_int32());
    data_left -= 4;

    info.set_super_id(read_id(in, data_left));

    info.set_class_loader_id(read_id(in, data_left));
    // Skip 4 ids. They are not valid in dalvik anyway
    read_id(in, data_left);
    read_id(in, data_left);
    read_id(in, data_left);
    read_id(in, data_left);

    info.set_data_size(in.read_int32());
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
            id_t field_name = read_id(in, data_left);
            auto type = static_cast<jvm_type_t>(in.read_byte());
            --data_left;

            size_t field_size = get_field_size(type);
            in.read_bytes(buffer, field_size);
            data_left -= field_size;

            info.add_static_field(field_name, to_field_type(type), buffer, field_size);

            if (in.eof()) return false;
        }
    }


    int16_t fields_count = in.read_int16();
    data_left -= 2;

    for (size_t index = 0, offset = 0; index < fields_count; ++index) {
        id_t field_name = read_id(in, data_left);
        auto type = static_cast<jvm_type_t>(in.read_byte());
        --data_left;

        if (in.eof()) return false;

        info.add_field_info(field_name, to_field_type(type), offset);
        offset += get_field_size(type);
    }

    return !in.eof();
}

// NOTE: Specs http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#1303
bool data_reader_v103_t::reader::read_instance_dump(hprof_istream& in, ssize_t& data_left, instance_info_impl_ptr_t& info) {
    id_t object_id = read_id(in, data_left);
    int32_t stack_trace_id = in.read_int32();
    data_left -= 4;
    id_t class_id = read_id(in, data_left);

    int32_t object_size = in.read_int32();
    data_left -= 4;
    data_left -= object_size;

    if (in.eof()) return false;

    auto mem = new (std::nothrow) u_int8_t[sizeof(instance_info_t) + object_size];
    info.reset(new (mem) instance_info_impl_t(object_id, class_id, stack_trace_id, object_size),
        [] (auto item) { delete[] reinterpret_cast<u_int8_t*>(item); });

    info->read_data(in);

    return !in.eof();
}

// TODO: Refs http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#1263
bool data_reader_v103_t::reader::read_objects_array_dump(hprof_istream& in, ssize_t& data_left, array_info_ptr_t& info) {
    id_t object_id = read_id(in, data_left);
    int32_t stack_trace_id = in.read_int32();
    int32_t length = in.read_int32();
    data_left -= 8;
    id_t class_id = read_id(in, data_left);

    if (in.eof()) return false;

    auto mem = new (std::nothrow) u_int8_t[sizeof(object_array_info_t) + (length * sizeof(id_t))];
    info.reset(new (mem) object_array_info_t(object_id, stack_trace_id, length), [] (auto item) { delete[] reinterpret_cast<u_int8_t*>(item); });

    object_array_info_t* array_info = static_cast<object_array_info_t*>(info.get());

    for (int index = 0; index < length; ++index) {
        array_info->data[index] = read_id(in, data_left);
    }

    return !in.eof();
}

// NOTE: Ref: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#1283
bool data_reader_v103_t::reader::read_array_dump(hprof_istream& in, ssize_t& data_left, array_info_ptr_t& info) {
    id_t object_id = read_id(in, data_left);
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

gc_root_t data_reader_v103_t::reader::read_gc_root(hprof_gc_tag_t subtype, hprof_istream& in, ssize_t& data_left) {
    switch (subtype) {
        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#969
        case DUMP_ROOT_UNKNOWN: {
            id_t object_id = read_id(in, data_left);
            return gc_root_t::create<gc_root_t::UNKNOWN>(object_id);
        }
        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#985
        case DUMP_ROOT_JNI_GLOBAL: {
            // info.root_type = ;
            id_t object_id = read_id(in, data_left);
            int32_t jni_ref = in.read_int32();
            data_left -= 4;
            return gc_root_t::create<gc_root_t::JNI_GLOBAL>(object_id, jni_ref);
        }

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#994
        case DUMP_ROOT_JNI_LOCAL: {
            // info.root_type = gc_root_t::JNI_LOCAL;
            id_t object_id = read_id(in, data_left);
            int32_t thread_seq_num = in.read_int32();
            int32_t stack_frame_id = in.read_int32();
            data_left -= 8;
            return gc_root_t::create<gc_root_t::JNI_LOCAL>(object_id, thread_seq_num, stack_frame_id);
        }

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#996
        case DUMP_ROOT_JAVA_FRAME: {
            id_t object_id = read_id(in, data_left);
            int32_t thread_seq_num = in.read_int32();
            int32_t stack_frame_id = in.read_int32();
            data_left -= 8;
            return gc_root_t::create<gc_root_t::JAVA_FRAME>(object_id, thread_seq_num, stack_frame_id);
        }

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#1005
        case DUMP_ROOT_NATIVE_STACK: {
            id_t object_id = read_id(in, data_left);
            int32_t thread_seq_num = in.read_int32();
            data_left -= 4;
            return gc_root_t::create<gc_root_t::NATIVE_STACK>(object_id, thread_seq_num);
        }

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#970
        case DUMP_ROOT_STICKY_CLASS: {
            id_t object_id = read_id(in, data_left);
            return gc_root_t::create<gc_root_t::STICKY_CLASS>(object_id);
        }

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#1006
        case DUMP_ROOT_THREAD_BLOCK: {
            id_t object_id = read_id(in, data_left);
            int32_t thread_seq_num = in.read_int32();
            data_left -= 4;
            return gc_root_t::create<gc_root_t::THREAD_BLOCK>(object_id, thread_seq_num);
        }

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#971
        case DUMP_ROOT_MONITOR_USED: {
            id_t object_id = read_id(in, data_left);
            return gc_root_t::create<gc_root_t::MONITOR_USED>(object_id);
        }

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#1015
        case DUMP_ROOT_THREAD_OBJECT: {
            id_t object_id = read_id(in, data_left);
            int32_t thread_seq_num = in.read_int32();
            int32_t stack_frame_id = in.read_int32();
            data_left -= 8;
            return gc_root_t::create<gc_root_t::THREAD_OBJECT>(object_id, thread_seq_num, stack_frame_id);
        }

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#972
        case DUMP_ROOT_INTERNED_STRING: {
            return gc_root_t::create<gc_root_t::INTERNED_STRING>(read_id(in, data_left));
        }

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#1031
        case DUMP_ROOT_FINALIZING: {
            return gc_root_t::create<gc_root_t::FINALIZING>();
        }

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#973
        case DUMP_ROOT_DEBUGGER: {
            return gc_root_t::create<gc_root_t::DEBUGGER>(read_id(in, data_left));
        }

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#1032
        case DUMP_ROOT_REFERENCE_CLEANUP: {
            return gc_root_t::create<gc_root_t::REFERENCE_CLEANUP>(read_id(in, data_left));

        }

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#974
        case DUMP_ROOT_VM_INTERNAL: {
            return gc_root_t::create<gc_root_t::VM_INTERNAL>(read_id(in, data_left));

        }

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#995
        case DUMP_ROOT_JNI_MONITOR: {
            id_t object_id = read_id(in, data_left);
            int32_t thread_seq_num = in.read_int32();
            int32_t stack_frame_id = in.read_int32();
            data_left -= 8;
            return gc_root_t::create<gc_root_t::JNI_MONITOR>(object_id, thread_seq_num, stack_frame_id);
        }

        // NOTE: http://androidxref.com/7.1.1_r6/xref/art/runtime/hprof/hprof.cc#1033
        case DUMP_UNREACHABLE:
            return gc_root_t::create<gc_root_t::UNREACHABLE>();
        default:
            break;
    }

    return gc_root_t::create<gc_root_t::INVALID>();
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
