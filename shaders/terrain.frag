#version 330 core

in vec3 WS_position; // world-space position
in vec3 WS_normal;   // world-space normal
in vec3 WS_color;
out vec3 fragColor;
//uniform vec3 color;
void main(){
    vec3 WS_toLight = normalize(vec3(10.0) - WS_position);
   // glm::vec3 col = glm:vec3(0.0f, 1.0f, 0.0f);
    //fragColor = col*vec3(0.3 + 0.7 * max(0.0, dot(normalize(WS_normal), WS_toLight)));
   // fragColor = vec3(colors*max(0.0, dot(normalize(WS_normal), WS_toLight))));
    fragColor = WS_color*vec3(0.3 + 0.7 * max(0.0, dot(normalize(WS_normal), WS_toLight)));
   //fragColor = vec3(0.0f,1.0f,0.0f)*vec3(0.3 + 0.7 * max(0.0, dot(normalize(WS_normal), WS_toLight)));
}
