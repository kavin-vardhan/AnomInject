import struct


class PE:
    def __init__(self, path):
        self.data = path.read_bytes()
        nt = self.u32(0x3c)
        optional = nt + 24
        self.image_base = self.u64(optional + 24)
        self.sections = []
        for i in range(self.u16(nt + 6)):
            o = optional + self.u16(nt + 20) + 40 * i
            size, rva, raw_size, raw = struct.unpack_from('<IIII', self.data, o + 8)
            self.sections.append((self.data[o:o+8].rstrip(b'\0').decode(), rva, size, raw, raw_size))
        self.directories = [struct.unpack_from('<II', self.data, optional + 112 + 8*i) for i in range(16)]

    def u16(self, o):
        return struct.unpack_from('<H', self.data, o)[0]

    def u32(self, o):
        return struct.unpack_from('<I', self.data, o)[0]

    def u64(self, o):
        return struct.unpack_from('<Q', self.data, o)[0]

    def offset(self, rva):
        for name, start, size, raw, raw_size in self.sections:
            if start <= rva < start + raw_size:
                return raw + rva - start
        raise ValueError(hex(rva))

    def cstring(self, o):
        return self.data[o:self.data.index(0, o)].decode('ascii')

    def compressed(self, o):
        length = [1,2,1,3,1,2,1,4,1,2,1,3,1,2,1,5][self.data[o] & 15]
        value = self.u32(o + length - 4) >> (32-7*length if length < 5 else 0)
        return value, o + length

    def imports(self):
        start, size = self.directories[1]
        result = {}
        if not start:
            return result
        o = self.offset(start)
        while self.u32(o + 12):
            lookup, stamp, forward, name, iat = struct.unpack_from('<IIIII', self.data, o)
            p = self.offset(lookup or iat)
            i = 0
            while self.u64(p + 8*i):
                item = self.u64(p + 8*i)
                result[iat+8*i] = self.cstring(self.offset(item)+2) if not item >> 63 else str(item & 65535)
                i += 1
            o += 20
        return result

    def debug_ranges(self):
        rva, size = self.directories[6]
        result = []
        if rva:
            o = self.offset(rva)
            for d in range(o, o+size, 28):
                if self.u32(d+12) == 2:
                    length, raw = self.u32(d+16), self.u32(d+24)
                    if self.data[raw:raw+4] == b'RSDS':
                        result.append((raw, raw+24, self.cstring(raw+24)))
        return result

    def eh4_ranges(self):
        imports = self.imports()
        ranges = []
        rva, size = self.directories[3]
        o = self.offset(rva)
        for entry in range(o, o+size, 12):
            begin, end, unwind = struct.unpack_from('<III', self.data, entry)
            u = self.offset(unwind)
            if not (self.data[u] >> 3) & 3:
                continue
            h = u + ((4 + 2*self.data[u+2] + 3) & ~3)
            handler = self.u32(h)
            code = self.offset(handler)
            direct = False
            if self.data[code:code+2] == b'\xff\x25':
                displacement = struct.unpack_from('<i', self.data, code+2)[0]
                direct = imports.get(handler+6+displacement) == '__CxxFrameHandler4'
            wrapper_calls = []
            if not direct:
                for instruction in range(code, code+112):
                    if self.data[instruction] != 0xe8:
                        continue
                    destination = handler + instruction-code+5 + struct.unpack_from('<i', self.data, instruction+1)[0]
                    try:
                        thunk = self.offset(destination)
                    except ValueError:
                        continue
                    if self.data[thunk:thunk+2] == b'\xff\x25':
                        displacement = struct.unpack_from('<i', self.data, thunk+2)[0]
                        if imports.get(destination+6+displacement) == '__CxxFrameHandler4':
                            wrapper_calls.append(dict(instruction_rva=handler+instruction-code, destination_rva=destination))
            if not direct and not wrapper_calls:
                continue
            info_rva = self.u32(h+4)
            first = p = self.offset(info_rva)
            header = self.data[p]
            p += 1
            assert not header & 128
            if header & 4:
                unused, p = self.compressed(p)
            maps = []
            if header & 8:
                maps.append(('unwind', self.u32(p)))
                p += 4
            if header & 16:
                maps.append(('try', self.u32(p)))
                p += 4
            ip_map = self.u32(p)
            p += 4
            if header & 1:
                unused, p = self.compressed(p)
            ranges.append(dict(first=first, last=p, kind='EH4 FuncInfo', function_rva=begin, handler_rva=handler))
            if header & 2:
                q = self.offset(ip_map)
                count, q = self.compressed(q)
                assert count < 100000
                for i in range(count):
                    segment, state_map = struct.unpack_from('<II', self.data, q)
                    q += 8
                    maps.append(('ip-state', state_map))
            elif ip_map:
                maps.append(('ip-state', ip_map))
            for kind, address in maps:
                if not address:
                    continue
                first = q = self.offset(address)
                count, q = self.compressed(q)
                assert count < 100000
                if kind == 'ip-state':
                    for i in range(count):
                        ip, q = self.compressed(q)
                        state, q = self.compressed(q)
                elif kind == 'unwind':
                    for i in range(count):
                        next_offset, q = self.compressed(q)
                        if next_offset & 3:
                            self.offset(self.u32(q))
                            q += 4
                        if next_offset & 3 in (1, 2):
                            obj, q = self.compressed(q)
                else:
                    for i in range(count):
                        for j in range(3):
                            unused, q = self.compressed(q)
                        self.offset(self.u32(q))
                        q += 4
                ranges.append(dict(first=first, last=q, kind='EH4 ' + kind, entries=count, function_rva=begin, handler_rva=handler))
        return ranges
