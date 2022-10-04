#version 440

layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    mat4 mapProjection;
    vec4 center;
    vec4 center_lowpart;
    float opacity;
    float wrapOffset;
    vec4 color;
};

void main() {
    fragColor = color;
}
