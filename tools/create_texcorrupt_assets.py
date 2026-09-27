import math
import os
import struct
import tempfile
import zlib

import unreal

UV_NAME = "M_CorruptTex_UV"
NORMAL_NAME = "M_CorruptTex_Normal"
NOISE_NAME = "T_CorruptTex_NoiseN"
MAT_DIR = "/AnomalyInjector/Materials"
TEX_DIR = "/AnomalyInjector/Textures"
UV_PATH = MAT_DIR + "/" + UV_NAME
NORMAL_PATH = MAT_DIR + "/" + NORMAL_NAME
NOISE_PATH = TEX_DIR + "/" + NOISE_NAME

DEFAULT_COLOR = "/Engine/EngineResources/DefaultTexture"
DEFAULT_DATA = "/Engine/EngineMaterials/T_Default_Material_Grid_M"
DEFAULT_NORMAL = "/Engine/EngineMaterials/DefaultNormal"

UV_SCALARS = [
    ("SrcKind", 0.0), ("SrcMip", 0.0), ("UvScale", 1.0), ("UvOffsetU", 0.0), ("UvOffsetV", 0.0), ("UvSwap", 0.0),
    ("ScrambleOn", 0.0), ("ScrambleK", 8.0), ("ScrambleAInv", 1.0), ("ScrambleB", 0.0),
    ("DbgChanSwap", 0.0), ("DbgSrgbTwice", 0.0), ("DbgTexelShift", 0.0), ("TexelSizeU", 0.0), ("TexelSizeV", 0.0),
    ("DbgSkipNormalEncode", 0.0), ("DbgOpaque", 0.0),
]
UV_TEXTURES = ["SrcColor", "SrcData", "SrcNormal"]
NORMAL_SCALARS = [
    ("SrcMip", 0.0), ("NoiseMip", 0.0), ("NormalSignX", 1.0), ("NormalSignY", 1.0), ("FlatMix", 0.0), ("NoiseAmp", 0.0),
    ("DbgChanSwap", 0.0), ("DbgTexelShift", 0.0), ("TexelSizeU", 0.0), ("TexelSizeV", 0.0), ("DbgSkipNormalEncode", 0.0),
]
NORMAL_TEXTURES = ["SrcNormal", "NoiseNormal"]

NOISE_SIZE = 256
NOISE_WAVES = [
    (1, 2, 0.020, 0.37), (3, -1, 0.013, 1.91), (4, 5, 0.008, 4.22), (-7, 3, 0.0055, 2.71),
    (9, 8, 0.0035, 5.53), (13, -11, 0.0024, 0.83), (16, 17, 0.0016, 3.14), (-23, 19, 0.0012, 1.29),
]

UV_FN = """float2 uv = UV;
uv = lerp(uv, uv.yx, step(0.5, UvSwap));
uv = frac(uv * UvScale + float2(UvOffsetU, UvOffsetV));
if (ScrambleOn > 0.5)
{
    float K = max(1.0, floor(ScrambleK + 0.5));
    float N = K * K;
    float2 c = min(floor(uv * K), K - 1.0);
    float2 l = uv * K - c;
    float j = c.y * K + c.x;
    float d = j - floor(ScrambleB + 0.5);
    d = d - N * floor(d / N);
    if (d >= N) { d -= N; }
    if (d < 0.0) { d += N; }
    float p = floor(ScrambleAInv + 0.5) * d;
    float i = p - N * floor(p / N);
    if (i >= N) { i -= N; }
    if (i < 0.0) { i += N; }
    float iy = floor(i / K);
    float ix = i - iy * K;
    if (ix >= K) { ix -= K; iy += 1.0; }
    if (ix < 0.0) { ix += K; iy -= 1.0; }
    uv = (float2(ix, iy) + l) / K;
}
uv.x += DbgTexelShift * TexelSizeU;
return uv;"""

UV_ENCODE = """float4 s = Col;
if (SrcKind > 1.5)
{
    float3 n = Nrm.xyz;
    float3 e = n * 0.5 + 0.5;
    e = lerp(e, n, step(0.5, DbgSkipNormalEncode));
    s = float4(e, Nrm.w);
}
else if (SrcKind > 0.5)
{
    s = Dat;
}
float3 rgb = s.rgb;
if (DbgSrgbTwice > 0.5)
{
    float3 c = saturate(rgb);
    float3 lo = c * 12.92;
    float3 hi = 1.055 * pow(max(c, 1e-6), 1.0 / 2.4) - 0.055;
    rgb = lerp(lo, hi, step(0.0031308, c));
}
rgb = lerp(rgb, rgb.bgr, step(0.5, DbgChanSwap));
float op = 1.0 - s.a;
op = lerp(op, 1.0, step(0.5, DbgOpaque));
return float4(rgb, op);"""

NORMAL_UV_FN = """float2 uv = UV;
uv.x += DbgTexelShift * TexelSizeU;
return uv;"""

NORMAL_ENCODE = """float3 n = Nrm.xyz;
n.x *= NormalSignX;
n.y *= NormalSignY;
if (FlatMix > 0.0)
{
    n = normalize(lerp(n, float3(0.0, 0.0, 1.0), saturate(FlatMix)));
}
if (NoiseAmp > 0.0)
{
    n = normalize(float3(n.xy + Noise.xy * NoiseAmp, n.z));
}
float3 e = n * 0.5 + 0.5;
e = lerp(e, n, step(0.5, DbgSkipNormalEncode));
e = lerp(e, e.bgr, step(0.5, DbgChanSwap));
return e;"""

MEL = unreal.MaterialEditingLibrary
EAL = unreal.EditorAssetLibrary
TOOLS = unreal.AssetToolsHelpers.get_asset_tools()


def log(msg):
    unreal.log("[texcorrupt-assets] " + msg)


def png_rgb(width, height, rows):
    raw = b"".join(b"\x00" + bytes(r) for r in rows)

    def chunk(tag, data):
        c = struct.pack(">I", len(data)) + tag + data
        return c + struct.pack(">I", zlib.crc32(tag + data) & 0xFFFFFFFF)

    hdr = struct.pack(">IIBBBBB", width, height, 8, 2, 0, 0, 0)
    return b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", hdr) + chunk(b"IDAT", zlib.compress(raw, 9)) + chunk(b"IEND", b"")


def noise_rows(size):
    rows = []
    two_pi = 2.0 * math.pi
    max_xy = 0.0
    for y in range(size):
        row = bytearray()
        for x in range(size):
            dx = 0.0
            dy = 0.0
            for fx, fy, amp, ph in NOISE_WAVES:
                a = two_pi * (fx * x + fy * y) / size + ph
                c = math.cos(a) * amp * two_pi
                dx += c * fx
                dy += c * fy
            nx, ny, nz = -dx, -dy, 1.0
            inv = 1.0 / math.sqrt(nx * nx + ny * ny + nz * nz)
            nx, ny, nz = nx * inv, ny * inv, nz * inv
            max_xy = max(max_xy, abs(nx), abs(ny))
            for v in (nx, ny, nz):
                row.append(max(0, min(255, int(math.floor((v * 0.5 + 0.5) * 255.0 + 0.5)))))
        rows.append(row)
    return rows, max_xy


def refuse_if_present(paths):
    present = [p for p in paths if EAL.does_asset_exist(p)]
    if present:
        raise RuntimeError(
            "refusing to author over existing assets %s. The subsystem CDO holds hard references to them, so an in-editor "
            "delete leaves partially loaded packages that cannot be saved. Delete the .uasset files on disk and re-run."
            % ", ".join(present))


def build_noise():
    rows, max_xy = noise_rows(NOISE_SIZE)
    tmp = os.path.join(tempfile.gettempdir(), "texcorrupt_noise_%d" % os.getpid())
    os.makedirs(tmp, exist_ok=True)
    src = os.path.join(tmp, NOISE_NAME + ".png")
    with open(src, "wb") as fh:
        fh.write(png_rgb(NOISE_SIZE, NOISE_SIZE, rows))
    task = unreal.AssetImportTask()
    task.set_editor_property("filename", src)
    task.set_editor_property("destination_path", TEX_DIR)
    task.set_editor_property("destination_name", NOISE_NAME)
    task.set_editor_property("automated", True)
    task.set_editor_property("replace_existing", True)
    task.set_editor_property("save", False)
    TOOLS.import_asset_tasks([task])
    tex = unreal.load_asset(NOISE_PATH)
    if tex is None:
        raise RuntimeError("noise import produced nothing at " + NOISE_PATH)
    tex.set_editor_property("compression_settings", unreal.TextureCompressionSettings.TC_NORMALMAP)
    tex.set_editor_property("srgb", False)
    tex.set_editor_property("lod_group", unreal.TextureGroup.TEXTUREGROUP_WORLD_NORMAL_MAP)
    tex.set_editor_property("never_stream", True)
    tex.set_editor_property("mip_gen_settings", unreal.TextureMipGenSettings.TMGS_FROM_TEXTURE_GROUP)
    tex.set_editor_property("address_x", unreal.TextureAddress.TA_WRAP)
    tex.set_editor_property("address_y", unreal.TextureAddress.TA_WRAP)
    EAL.save_asset(NOISE_PATH, only_if_is_dirty=False)
    os.remove(src)
    log("noise normal %s %dx%d, max |n.xy| %.4f, waves %d" % (NOISE_PATH, NOISE_SIZE, NOISE_SIZE, max_xy, len(NOISE_WAVES)))
    return tex


def fresh_material(name, blend):
    path = MAT_DIR + "/" + name
    mat = TOOLS.create_asset(name, MAT_DIR, unreal.Material, unreal.MaterialFactoryNew())
    mat.set_editor_property("material_domain", unreal.MaterialDomain.MD_SURFACE)
    mat.set_editor_property("blend_mode", blend)
    mat.set_editor_property("shading_model", unreal.MaterialShadingModel.MSM_UNLIT)
    mat.set_editor_property("two_sided", True)
    mat.set_editor_property("use_translucency_vertex_fog", False)
    return mat, path


def scalars(mat, specs, x, y0):
    out = {}
    for i, (name, default) in enumerate(specs):
        e = MEL.create_material_expression(mat, unreal.MaterialExpressionScalarParameter, x, y0 + i * 70)
        e.set_editor_property("parameter_name", name)
        e.set_editor_property("default_value", default)
        out[name] = e
    return out


def custom(mat, desc, code, inputs, out_type, x, y):
    e = MEL.create_material_expression(mat, unreal.MaterialExpressionCustom, x, y)
    e.set_editor_property("description", desc)
    e.set_editor_property("code", code)
    e.set_editor_property("output_type", out_type)
    ins = []
    for name, _ in inputs:
        ci = unreal.CustomInput()
        ci.set_editor_property("input_name", name)
        ins.append(ci)
    e.set_editor_property("inputs", ins)
    for name, (src, pin) in inputs:
        if not MEL.connect_material_expressions(src, pin, e, name):
            raise RuntimeError("could not connect %s into custom %s" % (name, desc))
    return e


def sample(mat, name, sampler_type, texture_path, uv, mip, x, y):
    e = MEL.create_material_expression(mat, unreal.MaterialExpressionTextureSampleParameter2D, x, y)
    e.set_editor_property("parameter_name", name)
    e.set_editor_property("texture", unreal.load_asset(texture_path) if isinstance(texture_path, str) else texture_path)
    e.set_editor_property("sampler_type", sampler_type)
    e.set_editor_property("sampler_source", unreal.SamplerSourceMode.SSM_FROM_TEXTURE_ASSET)
    e.set_editor_property("mip_value_mode", unreal.TextureMipValueMode.TMVM_MIP_LEVEL)
    e.set_editor_property("automatic_view_mip_bias", False)
    if not MEL.connect_material_expressions(uv, "", e, "UVs"):
        raise RuntimeError("could not connect UVs into " + name)
    if not MEL.connect_material_expressions(mip, "", e, "Level"):
        raise RuntimeError("could not connect Level into " + name)
    return e


def mask(mat, rgba, x, y):
    e = MEL.create_material_expression(mat, unreal.MaterialExpressionComponentMask, x, y)
    for ch, on in zip("rgba", rgba):
        e.set_editor_property(ch, on)
    return e


def build_uv(noise):
    mat, path = fresh_material(UV_NAME, unreal.BlendMode.BLEND_ALPHA_COMPOSITE)
    s = scalars(mat, UV_SCALARS, -1800, -600)
    tc = MEL.create_material_expression(mat, unreal.MaterialExpressionTextureCoordinate, -1500, -700)
    fn_inputs = [("UV", (tc, ""))] + [(n, (s[n], "")) for n in
                                      ["UvScale", "UvOffsetU", "UvOffsetV", "UvSwap", "ScrambleOn", "ScrambleK",
                                       "ScrambleAInv", "ScrambleB", "DbgTexelShift", "TexelSizeU", "TexelSizeV"]]
    uvfn = custom(mat, "TexCorruptUvFn", UV_FN, fn_inputs, unreal.CustomMaterialOutputType.CMOT_FLOAT2, -1200, -600)
    col = sample(mat, "SrcColor", unreal.MaterialSamplerType.SAMPLERTYPE_COLOR, DEFAULT_COLOR, uvfn, s["SrcMip"], -800, -700)
    dat = sample(mat, "SrcData", unreal.MaterialSamplerType.SAMPLERTYPE_LINEAR_COLOR, DEFAULT_DATA, uvfn, s["SrcMip"], -800, -400)
    nrm = sample(mat, "SrcNormal", unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL, DEFAULT_NORMAL, uvfn, s["SrcMip"], -800, -100)
    enc_inputs = [("Col", (col, "RGBA")), ("Dat", (dat, "RGBA")), ("Nrm", (nrm, "RGBA"))] + [
        (n, (s[n], "")) for n in ["SrcKind", "DbgSkipNormalEncode", "DbgSrgbTwice", "DbgChanSwap", "DbgOpaque"]]
    enc = custom(mat, "TexCorruptUvEncode", UV_ENCODE, enc_inputs, unreal.CustomMaterialOutputType.CMOT_FLOAT4, -400, -400)
    rgb = mask(mat, (True, True, True, False), -150, -450)
    alp = mask(mat, (False, False, False, True), -150, -300)
    MEL.connect_material_expressions(enc, "", rgb, "")
    MEL.connect_material_expressions(enc, "", alp, "")
    if not MEL.connect_material_property(rgb, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR):
        raise RuntimeError("UV corruptor: emissive not connected")
    if not MEL.connect_material_property(alp, "", unreal.MaterialProperty.MP_OPACITY):
        raise RuntimeError("UV corruptor: opacity not connected")
    return mat, path, {"SrcColor": col, "SrcData": dat, "SrcNormal": nrm}


def build_normal(noise):
    mat, path = fresh_material(NORMAL_NAME, unreal.BlendMode.BLEND_OPAQUE)
    s = scalars(mat, NORMAL_SCALARS, -1800, -600)
    tc = MEL.create_material_expression(mat, unreal.MaterialExpressionTextureCoordinate, -1500, -700)
    uvfn = custom(mat, "TexCorruptNormalUvFn", NORMAL_UV_FN,
                  [("UV", (tc, "")), ("DbgTexelShift", (s["DbgTexelShift"], "")), ("TexelSizeU", (s["TexelSizeU"], "")),
                   ("TexelSizeV", (s["TexelSizeV"], ""))],
                  unreal.CustomMaterialOutputType.CMOT_FLOAT2, -1200, -600)
    nrm = sample(mat, "SrcNormal", unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL, DEFAULT_NORMAL, uvfn, s["SrcMip"], -800, -600)
    nse = sample(mat, "NoiseNormal", unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL, noise, tc, s["NoiseMip"], -800, -300)
    enc_inputs = [("Nrm", (nrm, "RGBA")), ("Noise", (nse, "RGBA"))] + [
        (n, (s[n], "")) for n in ["NormalSignX", "NormalSignY", "FlatMix", "NoiseAmp", "DbgSkipNormalEncode", "DbgChanSwap"]]
    enc = custom(mat, "TexCorruptNormalEncode", NORMAL_ENCODE, enc_inputs, unreal.CustomMaterialOutputType.CMOT_FLOAT3,
                 -400, -500)
    if not MEL.connect_material_property(enc, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR):
        raise RuntimeError("normal corruptor: emissive not connected")
    return mat, path, {"SrcNormal": nrm, "NoiseNormal": nse}


def finish(mat, path):
    MEL.recompile_material(mat)
    stats = MEL.get_statistics(mat)
    EAL.save_asset(path, only_if_is_dirty=False)
    return stats


def verify(mat, path, scalar_specs, textures, blend, stats):
    problems = []
    got_s = set(str(n) for n in MEL.get_scalar_parameter_names(mat))
    got_t = set(str(n) for n in MEL.get_texture_parameter_names(mat))
    for name, _ in scalar_specs:
        if name not in got_s:
            problems.append("missing scalar " + name)
    for name in textures:
        if name not in got_t:
            problems.append("missing texture " + name)
    extra = sorted((got_s - set(n for n, _ in scalar_specs)) | (got_t - set(textures)))
    if extra:
        problems.append("unexpected parameters " + ",".join(extra))
    if mat.get_editor_property("material_domain") != unreal.MaterialDomain.MD_SURFACE:
        problems.append("domain is not Surface")
    if mat.get_editor_property("shading_model") != unreal.MaterialShadingModel.MSM_UNLIT:
        problems.append("shading model is not Unlit")
    if mat.get_editor_property("blend_mode") != blend:
        problems.append("blend mode is not %s" % blend)
    for name, default in scalar_specs:
        v = MEL.get_material_default_scalar_parameter_value(mat, name)
        if abs(v - default) > 1e-6:
            problems.append("scalar %s default %s != %s" % (name, v, default))
    if stats.num_pixel_shader_instructions <= 0 or stats.num_samplers < 0:
        problems.append("did not compile (ps=%d samplers=%d)" % (stats.num_pixel_shader_instructions, stats.num_samplers))
    log("%s: blend=%s shading=%s domain=%s scalars=%d textures=%d ps_instructions=%d samplers=%d pixel_texture_samples=%d -> %s"
        % (path, mat.get_editor_property("blend_mode"), mat.get_editor_property("shading_model"),
           mat.get_editor_property("material_domain"), len(got_s), len(got_t), stats.num_pixel_shader_instructions,
           stats.num_samplers, stats.num_pixel_texture_samples, "OK" if not problems else "; ".join(problems)))
    return problems


def verify_samples(mat_path, samples):
    problems = []
    for name, e in sorted(samples.items()):
        mode = e.get_editor_property("mip_value_mode")
        bias = e.get_editor_property("automatic_view_mip_bias")
        src = e.get_editor_property("sampler_source")
        st = e.get_editor_property("sampler_type")
        tex = e.get_editor_property("texture")
        ok = (mode == unreal.TextureMipValueMode.TMVM_MIP_LEVEL and bias is False
              and src == unreal.SamplerSourceMode.SSM_FROM_TEXTURE_ASSET and tex is not None)
        if not ok:
            problems.append("%s sample %s is not an explicit-mip, no-view-bias, asset-sampler read" % (mat_path, name))
        log("%s sample %s: mip_value_mode=%s automatic_view_mip_bias=%s sampler_source=%s sampler_type=%s default=%s -> %s"
            % (mat_path, name, mode, bias, src, st, tex.get_path_name() if tex else None, "OK" if ok else "FAIL"))
    return problems


def main():
    refuse_if_present([UV_PATH, NORMAL_PATH, NOISE_PATH])
    noise = build_noise()
    uv, uv_path, uv_samples = build_uv(noise)
    nm, nm_path, nm_samples = build_normal(noise)
    problems = []
    problems += verify(uv, uv_path, UV_SCALARS, UV_TEXTURES, unreal.BlendMode.BLEND_ALPHA_COMPOSITE, finish(uv, uv_path))
    problems += verify(nm, nm_path, NORMAL_SCALARS, NORMAL_TEXTURES, unreal.BlendMode.BLEND_OPAQUE, finish(nm, nm_path))
    problems += verify_samples(uv_path, uv_samples)
    problems += verify_samples(nm_path, nm_samples)
    for path in (UV_PATH, NORMAL_PATH, NOISE_PATH):
        if not EAL.does_asset_exist(path):
            problems.append("not on disk: " + path)
    tex = unreal.load_asset(NOISE_PATH)
    log("%s compression=%s srgb=%s lod_group=%s never_stream=%s size=%dx%d" % (
        NOISE_PATH, tex.get_editor_property("compression_settings"), tex.get_editor_property("srgb"),
        tex.get_editor_property("lod_group"), tex.get_editor_property("never_stream"), tex.blueprint_get_size_x(),
        tex.blueprint_get_size_y()))
    dirty = [p.get_name() for p in unreal.EditorLoadingAndSavingUtils.get_dirty_content_packages()]
    log("dirty content packages after save: %s" % dirty)
    if dirty:
        problems.append("dirty packages remain: " + ",".join(dirty))
    if problems:
        log("FAILED: " + " | ".join(problems))
        raise RuntimeError("texcorrupt assets failed verification: " + " | ".join(problems))
    log("TEXCORRUPT-ASSETS-AUTHORED OK %s %s %s" % (UV_PATH, NORMAL_PATH, NOISE_PATH))


if __name__ == "__main__":
    main()
