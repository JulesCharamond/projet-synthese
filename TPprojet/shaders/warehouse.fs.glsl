#version 300 es
precision mediump float;

out vec3 fColor;

uniform float uShininess;

const int SPOTS = 2;

uniform vec3 uLightDir_vs;
uniform vec3 uLightIntensity;

uniform vec3 uSpotLightsPos[SPOTS];
uniform vec3 uSpotLightsIntensity[SPOTS];
uniform vec3 uSpotLightsDirection[SPOTS];
uniform float uSpotLightsAngle[SPOTS];

uniform sampler2D uKdTexture;
uniform sampler2D uKsTexture;

in vec2 vTexCoords;
in vec3 vNormal_vs;
in vec3 vPosition_vs;

vec3 blinnPhong(vec3 LightDir_vs, vec3 LightIntensity){
  vec3 wi = normalize(LightDir_vs);
  vec3 Li = LightIntensity;
  vec3 N = normalize(vNormal_vs);
  vec3 w0 = (normalize(-vPosition_vs));
  vec3 halfVector = (w0+wi)/2.;

  return Li * (texture(uKdTexture,vTexCoords).xyz * max(0.,dot(wi,N)) 
    + texture(uKsTexture,vTexCoords).xyz*(pow(max(0.,dot(halfVector,N)),uShininess)));
}

void main() {
  fColor = vec3(0,0,0);
    fColor += blinnPhong(uLightDir_vs, uLightIntensity);
    
    for (int i = 0; i < SPOTS; i++){
        vec3 dir = normalize(vPosition_vs-uSpotLightsPos[i]);
        float cosine = dot(normalize(uSpotLightsDirection[i]), dir);
        if (cosine > uSpotLightsAngle[i]){
            float dif = 1. - uSpotLightsAngle[i];
            float factor = clamp((cosine-uSpotLightsAngle[i])/dif, 0., 1.);
            fColor += factor*blinnPhong(uSpotLightsPos[i], uSpotLightsIntensity[i]);
        }
    }
};
