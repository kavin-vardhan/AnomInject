import struct
import sys


def f32(x):
    return struct.unpack('<f', struct.pack('<f', x))[0]


def f32_bits(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


class KatStream:
    def __init__(self, seed):
        self.seed = seed & 0xFFFFFFFF
        self.steps = 0

    def mutate(self):
        self.seed = (self.seed * 196314165 + 907633515) & 0xFFFFFFFF
        self.steps += 1

    def fraction(self):
        self.mutate()
        bits = 0x3F800000 | (self.seed >> 9)
        return struct.unpack('<f', struct.pack('<I', bits))[0] - 1.0

    def rand_helper(self, a):
        if a <= 0:
            return 0
        return int(f32(self.fraction() * f32(float(a))))

    def frand_range(self, lo, hi):
        return lo + (hi - lo) * self.fraction()


def attempt(stream, num_eligible, num_candidates, modes_per_id, hold_min, hold_max):
    before = stream.steps
    out = {'outcome': 0, 'id': -1, 'target': -1, 'hold_bits': 0, 'mode_drawn': 0, 'mode': -1}
    if num_eligible <= 0:
        out['outcome'] = 0
    else:
        out['id'] = stream.rand_helper(num_eligible)
        if num_candidates <= 0:
            out['outcome'] = 1
        else:
            out['target'] = stream.rand_helper(num_candidates)
            out['hold_bits'] = f32_bits(f32(stream.frand_range(f32(hold_min), f32(hold_max))))
            n = modes_per_id[out['id']]
            if n >= 0:
                out['mode_drawn'] = 1
                u = stream.fraction()
                out['mode'] = int(f32(u * f32(float(n)))) if n > 0 else -1
            out['outcome'] = 2
    out['seed_after'] = stream.seed
    out['draws'] = stream.steps - before
    return out


CASES = [
    ('m53_zero_modes', 4242, [(1, 5, [0], 3.0, 6.0)]),
    ('m53_one_mode', 4242, [(1, 5, [1], 3.0, 6.0), (1, 5, [1], 3.0, 6.0)]),
    ('m53_several_modes', 4242, [(1, 6, [2], 3.0, 6.0)] * 3 + [(1, 6, [4], 3.0, 6.0)] * 3),
    ('non_m53_no_mode_draw', 4242, [(2, 7, [-1, -1], 3.0, 6.0)] * 3),
    ('no_candidates_one_draw', 4242, [(3, 0, [-1, 2, 2], 3.0, 6.0), (3, 4, [-1, 2, 2], 3.0, 6.0)]),
    ('interleaved', 4242, [(4, 9, [-1, 2, -1, 2], 3.0, 6.0)] * 10),
    ('empty_set_one_draw_then_none', 99, [(1, 3, [0], 3.0, 6.0), (2, 3, [-1, 0], 3.0, 6.0), (2, 3, [-1, 0], 3.0, 6.0)]),
    ('all_excluded_zero_draws', 4242, [(0, 5, [], 3.0, 6.0), (0, 5, [], 3.0, 6.0), (1, 5, [-1], 3.0, 6.0)]),
    ('negative_seed_odd_hold', -123456, [(3, 11, [2, -1, 3], 0.35, 7.9)] * 5),
]


def flit(v):
    s = '%.9g' % f32(v)
    if '.' not in s and 'e' not in s:
        s += '.0'
    return s + 'f'


def main():
    rows = []
    for name, seed, attempts in CASES:
        stream = KatStream(seed)
        for i, (ne, nc, modes, lo, hi) in enumerate(attempts):
            r = attempt(stream, ne, nc, modes, lo, hi)
            padded = list(modes) + [-2] * (4 - len(modes))
            rows.append('\t{ "%s", %d, %d, %d, %d, { %s }, %d, %s, %s, %d, %d, %d, 0x%08Xu, %d, %d, 0x%08Xu, %du },' % (
                name, seed, i, ne, nc, ', '.join(str(m) for m in padded), len(modes), flit(lo), flit(hi),
                r['outcome'], r['id'], r['target'], r['hold_bits'], r['mode_drawn'], r['mode'], r['seed_after'], r['draws']))
    sys.stdout.write('static const FKatRow GKat[] =\n{\n')
    sys.stdout.write('\n'.join(rows))
    sys.stdout.write('\n};\n')


if __name__ == '__main__':
    main()
