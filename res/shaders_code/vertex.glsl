#version 450

const uint barsCount = 256;
layout(set=0, binding=0) uniform uniformBuffer{
    vec4 positions[barsCount/4];
};

layout(location = 0) in vec2 inPosition;

layout(location = 0) out flat uint v_index;

void main() {
    gl_Position = vec4(inPosition, 0.0, 1.0);

    float newPos = 0;
    uint index_major = gl_InstanceIndex / 4;
    uint index_minor = gl_InstanceIndex % 4;
    switch(index_minor) {
        case 0:
            newPos = positions[index_major].x;
            break;
        case 1:
            newPos = positions[index_major].y;
            break;
        case 2:
            newPos = positions[index_major].z;
            break;
        case 3:
            newPos = positions[index_major].w;
            break;
    }
    if(inPosition.y < -0.99f) {
        gl_Position.y = max(0, newPos); 
    }
    else {
        gl_Position.y = -max(0, newPos); 
    }
    gl_Position.x += 2*float(gl_InstanceIndex) / (barsCount);
    v_index = gl_InstanceIndex;

    return;
}