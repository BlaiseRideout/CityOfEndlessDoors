attribute float in_Color;
//attribute vec3 normal;
attribute vec3 v1, v2;
//attribute vec3 eye;

//varying vec3 n, view;
varying vec4 vertColor;

void main(){
  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;

  vec3 normal = normalize(cross(v2, v1));

  vec3 lightDir = normalize(vec3(1.0, -1.0, 0.5) * gl_NormalMatrix);

  float NdotL = max(dot(normal, lightDir), 0.0);

  float shade = NdotL * in_Color * 0.9;
  vertColor = vec4(0.2 + shade, 0.2 + shade, 0.2 + shade, 1.0);
}
