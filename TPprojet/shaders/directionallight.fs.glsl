#version 300 es
precision mediump float;

out vec3 fFragColor;

uniform vec3 uKd;
uniform vec3 uKs;
uniform float uShininess;

uniform vec3 uLightDir_vs;
uniform vec3 uLightIntensity;

in vec2 vTexCoords;
in vec3 vNormal_vs;
in vec3 vPosition_vs;

vec3 blinnPhong(){
  vec3 wi = normalize(uLightDir_vs);
  vec3 Li = uLightIntensity;
  vec3 w0 = (normalize(-vPosition_vs));
  vec3 halfVector = (w0+wi)/2.;

  return Li*( uKd*dot(wi, vNormal_vs) + uKs*pow(dot(halfVector,vNormal_vs), uShininess));
}

void main() {
  fFragColor = blinnPhong();
};
