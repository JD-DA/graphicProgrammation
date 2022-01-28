#version 330 core

// Données reçues (interpolées) de la part du vertex shader
in vec3 fragmentColor;

// Résultats du Fragment shader
//out vec3 Color;

vec4 C;
out vec4 color;

void main(){
  //Color = fragmentColor;
  C.rgb = fragmentColor;
  C.a = 1;
  color=C;
  //gl_FragColor = C;

}
