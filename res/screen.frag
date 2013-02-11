varying vec4 vertColor;
//varying vec3 n, view;

void main(){

//  vec3 lightDir = normalize(vec3(1.0f, -0.5f, 0.5f));

//  float ambi = 0.1;
//  float dotp = max(dot(n, lightDir), 0);
//  float diff = 0.5 * dotp;
//  vec3 a = dotp * n;
//  vec3 b = normalize(2.0 * a - lightDir);
//  float spec = 0.5 * max(pow(max(dot(b, view), 0), 128), 0);

//  float shade = ambi + diff + spec;
//  gl_FragColor = vec4(shade, shade, shade, 1.0f);
  gl_FragColor = vertColor;
}
