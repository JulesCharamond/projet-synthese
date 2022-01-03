#version 300 es
precision mediump float;

out vec3 fFragColor;

uniform vec3 uKd;
uniform vec3 uKs;
uniform float uShininess;

uniform vec3 uLightPos_vs;
uniform vec3 uLightIntensity;

in vec2 vTexCoords;
in vec3 vNormal_vs;
in vec3 vPosition_vs;



vec3 blinnPhong(){
  vec3 wi = normalize(uLightPos_vs-vPosition_vs);
  float dist = distance(vPosition_vs,uLightPos_vs);
  vec3 Li = uLightIntensity/(dist*dist);
  vec3 w0 = (normalize(-vPosition_vs));
  vec3 halfVector = (w0+wi)/2.;

  return Li*( uKd*max(0.,dot(wi, normalize(vNormal_vs))) + uKs*pow(max(0.,dot(halfVector,normalize(vNormal_vs))), uShininess));
}

void main() {
  fFragColor = blinnPhong();
};
