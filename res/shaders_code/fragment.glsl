#version 450

layout(location = 0) out vec4 outColor;

layout(location = 0) in flat uint v_index;

const uint barsCount = 256;
void main() {
    outColor = vec4(0.2, 0.2, float(v_index)/barsCount, 1.0);
    
    return;
}