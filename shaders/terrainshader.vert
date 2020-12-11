#version 410 core

layout(location = 0) in vec3 OS_position;
layout(location = 1) in vec3 OS_normal;
layout(location = 2) in vec3 OS_color;
layout(location = 5) in vec2 OS_texture;


uniform mat4 model, view, projection;

out vec3 WS_position; // world-space position
out vec3 WS_normal;   // world-space normal
out vec3 WS_color;
out vec2 WS_texture;
out float isColor;

void main() {
    isColor=1;
    if(OS_position.y<.9&&OS_position.y>.25){
        isColor = 0;
    }
    WS_position = (model * vec4(OS_position, 1.0)).xyz;
    WS_normal = (model * vec4(OS_normal, 0.0)).xyz;
    WS_color = OS_color;
    WS_texture = OS_texture;
    gl_Position = projection * view * vec4(WS_position, 1.0);
}


