"""Print the exception and nearby module return addresses from a Windows minidump."""

import pathlib
import struct
import sys


data = pathlib.Path(sys.argv[1]).read_bytes()
u32 = lambda offset: struct.unpack_from("<I", data, offset)[0]
u64 = lambda offset: struct.unpack_from("<Q", data, offset)[0]
if data[:4] != b"MDMP":
    raise SystemExit("Not a minidump")

streams = {}
for i in range(u32(8)):
    kind, size, rva = struct.unpack_from("<III", data, u32(12) + 12 * i)
    streams[kind] = (rva, size)

module_rva, _ = streams[4]
modules = []
for i in range(u32(module_rva)):
    record = module_rva + 4 + 108 * i
    base, size = struct.unpack_from("<QI", data, record)
    name_rva = u32(record + 20)
    name = data[name_rva + 4 : name_rva + 4 + u32(name_rva)].decode("utf-16le")
    modules.append((base, base + size, name))


def describe(address):
    for start, end, name in modules:
        if start <= address < end:
            return f"{name} + 0x{address - start:x}"
    return f"0x{address:x}"


exception_rva, _ = streams[6]
thread_id = u32(exception_rva)
print(f"thread={thread_id} exception=0x{u32(exception_rva + 8):08x}")
print("address=" + describe(u64(exception_rva + 24)))
print("access=" + str(u64(exception_rva + 40)) + " target=" + describe(u64(exception_rva + 48)))
context_size, context_rva = struct.unpack_from("<II", data, exception_rva + 160)
context_rsp = u64(context_rva + 152)
context_rip = u64(context_rva + 248)
print(f"context rsp=0x{context_rsp:x} rip={describe(context_rip)}")

threads_rva, _ = streams[3]
for i in range(u32(threads_rva)):
    record = threads_rva + 4 + 48 * i
    if u32(record) != thread_id:
        continue
    stack_base = u64(record + 24)
    stack_size, stack_rva = struct.unpack_from("<II", data, record + 32)
    print(f"stack=0x{stack_base:x} size={stack_size}")
    hits = 0
    start = stack_rva + max(0, context_rsp - stack_base)
    for position in range(start, stack_rva + stack_size - 7, 8):
        value = u64(position)
        description = describe(value)
        if description.startswith("0x"):
            continue
        print(f"  sp+0x{position - stack_rva:x}: {description}")
        hits += 1
        if hits >= 35:
            break
    break
