attribute float in_Color;
attribute vec3 normal;
//attribute vec3 eye;

//varying vec3 n, view;
varying vec4 vertColor;

void main(){
//  view =  eye - vec3(gl_Position);
  gl_Position = ftransform();

  vec3 normal = normalize(normal);

  vec3 lightDir = normalize(vec3(1.0f, -1.0f, 0.5f));
//  pnormal = normalize(normal);
//  n = normalize(normal);
//  HV =  - lightDir;

  float NdotL = max(dot(normal, lightDir), 0.0);
//  float specular = 1.0f;
//  if(NdotL > 0.0) {
//    float NdotHV = max(dot(normal, HV), 0.0);
//    float specular = 0.5f * pow(NdotHV, 8);
//  }

  float shade = NdotL * in_Color * 0.9;
//  shade = specular * shade;
  vertColor = vec4(0.1 + shade, 0.1 + shade, 0.1 + shade, 1.0f);
}
