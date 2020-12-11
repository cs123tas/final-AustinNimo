#version 410 core

in vec3 WS_position; // world-space position
in vec3 WS_normal;   // world-space normal
in vec2 WS_texture;
in vec3 WS_color;
in float isColor;
out vec3 fragColor;

uniform sampler2D sampler;
void main(){
    vec3 WS_toLight = normalize(vec3(10.0) - WS_position);
    //fragColor = vec4(1.0,1.0,1.0,1.0);
    //fragColor = vec3(0.3 + 0.7 * max(0.0, dot(normalize(WS_normal), WS_toLight)));
    if(isColor==1){
          fragColor=WS_color*vec3(0.3 + 0.7 * max(0.0, dot(normalize(WS_normal), WS_toLight)));
      }
      else{
          fragColor = texture(sampler,WS_texture).xyz;
      }
}


