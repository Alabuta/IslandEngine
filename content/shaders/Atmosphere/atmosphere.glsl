/********************************************************************************************************************************
****
****    Source code of Crusoe's Island Engine.
****    Copyright (C) 2009 - 2015 Crusoe's Island LLC.
****
****    Started at 30th June 2010.
****    Description: precomputed atmospheric scattering shader file (Eric Bruneton).
****
********************************************************************************************************************************/
//const float SUN_INTENSITY = 100.0;
const vec3 earthPos = vec3(0.0, 0.0, 6360010.0);

// ----------------------------------------------------------------------------
// PARAMETERIZATION OPTIONS
// ----------------------------------------------------------------------------

const int TRANSMITTANCE_W = 256;
const int TRANSMITTANCE_H = 64;

const int SKY_W = 64;
const int SKY_H = 16;

const int RES_R = 32;
const int RES_MU = 128;
const int RES_MU_S = 32;
const int RES_NU = 8;

#define TRANSMITTANCE_NON_LINEAR 1
#define INSCATTER_NON_LINEAR 1

// ----------------------------------------------------------------------------
// PHYSICAL MODEL PARAMETERS
// ----------------------------------------------------------------------------
const float kPI = 3.14159265358979323846;

const float SUN_INTENSITY = 100.0;

const float SCALE = 1000.0;

const float Rg = 6360.0 * SCALE;
const float Rt = 6420.0 * SCALE;
const float RL = 6421.0 * SCALE;

const float AVERAGE_GROUND_REFLECTANCE = 0.1;

// Rayleigh
const float HR = 8.0 * SCALE;
const vec3 betaR = vec3(5.8e-3, 1.35e-2, 3.31e-2) / SCALE;

// Mie
// DEFAULT
const float HM = 1.2 * SCALE;
const vec3 betaMSca = vec3(4e-3) / SCALE;
const vec3 betaMEx = betaMSca / 0.9;
const float mieG = 0.8;

#if CRUS_FRAGMENT_SHADER
uniform sampler3D inscatterSampler;
uniform sampler2D transmittanceSampler;

vec4 texture4D(sampler3D _table, float _r, float _mu, float _muS, float _nu)
{
    float H = sqrt(Rt * Rt - Rg * Rg);
    float rho = sqrt(_r * _r - Rg * Rg);

#if INSCATTER_NON_LINEAR
    float rmu = _r * _mu;

    float delta = rmu * rmu - _r * _r + Rg * Rg;
    vec4 cst = rmu < 0.0 && delta > 0.0 ? vec4(1.0, 0.0, 0.0, 0.5 - 0.5 / float(RES_MU)) : vec4(-1.0, H * H, H, 0.5 + 0.5 / float(RES_MU));

    float uR = 0.5 / float(RES_R) + rho / H * (1.0 - 1.0 / float(RES_R));
    float uMu = cst.w + (rmu * cst.x + sqrt(delta + cst.y)) / (rho + cst.z) * (0.5 - 1.0 / float(RES_MU));

    // paper formula
    //float uMuS = 0.5 / float(RES_MU_S) + max((1.0 - exp(-3.0 * muS - 0.6)) / (1.0 - exp(-3.6)), 0.0) * (1.0 - 1.0 / float(RES_MU_S));

    // better formula
    float uMuS = 0.5 / float(RES_MU_S) + (atan(max(_muS, -0.1975) * tan(1.26 * 1.1)) / 1.1 + (1.0 - 0.26)) * 0.5 * (1.0 - 1.0 / float(RES_MU_S));
#else
    float uR = 0.5 / float(RES_R) + rho / H * (1.0 - 1.0 / float(RES_R));
    float uMu = 0.5 / float(RES_MU) + (_mu + 1.0) / 2.0 * (1.0 - 1.0 / float(RES_MU));
    float uMuS = 0.5 / float(RES_MU_S) + max(_muS + 0.2, 0.0) / 1.2 * (1.0 - 1.0 / float(RES_MU_S));
#endif
    float lerp = (_nu + 1.0) / 2.0 * (float(RES_NU) - 1.0);
    float uNu = floor(lerp);
    lerp = lerp - uNu;
    return texture(_table, vec3((uNu + uMuS) / float(RES_NU), uMu, uR)) * (1.0 - lerp) +
           texture(_table, vec3((uNu + uMuS + 1.0) / float(RES_NU), uMu, uR)) * lerp;
}

vec2 getTransmittanceUV(float _r, float _mu)
{
    float uR, uMu;
#if TRANSMITTANCE_NON_LINEAR
    uR = sqrt((_r - Rg) / (Rt - Rg));
    uMu = atan((_mu + 0.15) / (1.0 + 0.15) * tan(1.5)) / 1.5;
#else
    uR = (_r - Rg) / (Rt - Rg);
    uMu = (_mu + 0.15) / (1.0 + 0.15);
#endif
    return vec2(uMu, uR);
}

// Rayleigh phase function
float phaseFunctionR(float _mu)
{
    return (3.0 / (16.0 * kPI)) * (1.0 + _mu * _mu);
}

// Mie phase function
float phaseFunctionM(float _mu)
{
    float mieGsq = mieG * mieG;
    return 1.5 * 1.0 / (4.0 * kPI) * (1.0 - mieGsq) * pow(1.0 + (mieGsq) - 2.0 * mieG *_mu, -3.0/2.0) * (1.0 + _mu * _mu) / (2.0 + mieGsq);
}

// approximated single Mie scattering (cf. approximate Cm in paragraph "Angular precision")
vec3 getMie(vec4 _rayMie) // rayMie.rgb=C*, rayMie.w=Cm,r
{
    return _rayMie.rgb * _rayMie.w / max(_rayMie.r, 1e-4) * (betaR.r / betaR);
}

// transmittance(=transparency) of atmosphere for infinite ray (r,mu)
// (mu=cos(view zenith angle)), intersections with ground ignored
vec3 transmittance(float _r, float _mu)
{
    vec2 uv = getTransmittanceUV(_r, _mu);
    return texture2D(transmittanceSampler, uv).rgb;
}

// scattered sunlight between two points
// camera=observer
// viewdir=unit vector towards observed point
// sundir=unit vector towards the sun
// return scattered light and extinction coefficient
vec3 skyRadiance(vec3 _cam, vec3 _view, vec3 _sun, out vec3 _extinction)
{
    vec3 result;

    float r = length(_cam);
    float rMu = dot(_cam, _view);
    float mu = rMu / r;

    float dSq = sqrt(rMu * rMu - r * r + Rt * Rt);
    float din = max(-rMu - dSq, 0.0);

    if(din > 0.0){
        _cam += din * _view;
        rMu += din;

        mu = rMu / Rt;
        r = Rt;
    }

    if(r <= Rt){
        float nu = dot(_view, _sun);
        float muS = dot(_cam, _sun) / r;

        vec4 inScatter = texture4D(inscatterSampler, r, rMu / r, muS, nu);
        _extinction = transmittance(r, mu);

        vec3 inScatterM = getMie(inScatter);

        float phase = phaseFunctionR(nu);
        float phaseM = phaseFunctionM(nu);

        result = inScatter.rgb * phase + inScatterM * phaseM;
    }

    else {
        result = vec3(0.0);
        _extinction = vec3(1.0);
    }

    return result * SUN_INTENSITY;
}

// ----------------------------------------------------------------------------
// SKYMAP AND HDR
// ----------------------------------------------------------------------------
uniform float hdrExposure;

vec3 hdr(vec3 L)
{
    L = L * hdrExposure;
    L.r = L.r < 1.413 ? pow(L.r * 0.38317, 1.0 / 2.2) : 1.0 - exp(-L.r);
    L.g = L.g < 1.413 ? pow(L.g * 0.38317, 1.0 / 2.2) : 1.0 - exp(-L.g);
    L.b = L.b < 1.413 ? pow(L.b * 0.38317, 1.0 / 2.2) : 1.0 - exp(-L.b);
    return L;
}

#endif