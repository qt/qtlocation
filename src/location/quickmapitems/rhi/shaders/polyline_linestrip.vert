#version 440

layout(location = 0) in vec4 qt_Vertex;

layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    mat4 mapProjection;
    vec4 center;
    vec4 center_lowpart;
    float opacity;
    float wrapOffset;
    vec4 color;
};

vec4 wrapped(in vec4 v) { return vec4(v.x + wrapOffset, v.y, 0.0, 1.0); }

void main() {
    vec4 vtx = wrapped(qt_Vertex) - center;
    vtx = vtx - center_lowpart;
    gl_Position = qt_Matrix * mapProjection * vtx;
}
