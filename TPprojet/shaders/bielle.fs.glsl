#version 300 es

precision mediump float;

in vec3 vPosition_vs;
in vec3 vNormal_vs;
in vec2 vTexCoords;

out vec3 fColor;

uniform vec3 uKd;
uniform vec3 uKs;
uniform float uShininess;

const int MAX_SPOTLIGHTS = 2;

uniform vec3 uLightPos;
uniform vec3 uLightIntensity;

uniform vec3 uSpotLightsPos[MAX_SPOTLIGHTS];
uniform vec3 uSpotLightsIntensity[MAX_SPOTLIGHTS];
uniform vec3 uSpotLightsDirection[MAX_SPOTLIGHTS];
uniform float uSpotLightsAngle[MAX_SPOTLIGHTS];

float dot_pos(vec3 v1, vec3 v2){
    return max(0., dot(v1,v2));
}

vec3 blinnPhong(vec3 uLightPos_vs, vec3 uLightIntensity){
    vec3 wi = normalize(uLightPos_vs - vPosition_vs);
    vec3 N = normalize(vNormal_vs);
    float d = distance(vPosition_vs, uLightPos_vs);
    vec3 Li = uLightIntensity/(d);
    vec3 halfvector = normalize((normalize(-vPosition_vs) + wi)/2.);

    return Li * (uKd * dot_pos(wi,N) + uKs*(pow(dot_pos(halfvector,N),uShininess)));
}

bool isInSpotLight(vec3 LightPos, vec3 Intensity, vec3 direction, float angle) {
    if (Intensity == vec3(0,0,0)){
        return false;
    }
    float d = distance(vPosition_vs, LightPos);
    vec3 dir = normalize(vPosition_vs-LightPos);
    
    if (dot(normalize(direction), dir) > angle) {
        return true;
    }

    return false;
}

void main() {
    fColor = vec3(0,0,0);
    fColor += blinnPhong(uLightPos, uLightIntensity);
    
    for (int i = 0; i < MAX_SPOTLIGHTS; i++){
        vec3 dir = normalize(vPosition_vs-uSpotLightsPos[i]);
        float cosine = dot(normalize(uSpotLightsDirection[i]), dir);
        if (cosine > uSpotLightsAngle[i]){
            float dif = 1. - uSpotLightsAngle[i];
            float factor = clamp((cosine-uSpotLightsAngle[i])/dif, 0., 1.);
            fColor += factor*blinnPhong(uSpotLightsPos[i], uSpotLightsIntensity[i]);
        }
    }
};