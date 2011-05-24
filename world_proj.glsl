void main()
{
  vec4 tmp = gl_ModelViewMatrix * gl_Vertex;
  tmp.xyz = tmp.xyz / (1.0 - tmp.w);
  tmp.w = 1.0;
  gl_Position = gl_ProjectionMatrix * tmp;
  gl_FrontColor = gl_Color;
  gl_FogFragCoord = length(gl_Position);
}