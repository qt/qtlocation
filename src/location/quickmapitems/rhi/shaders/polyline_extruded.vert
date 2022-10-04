#version 440

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec4 previous;
layout(location = 2) in vec4 next;
layout(location = 3) in float direction;
layout(location = 4) in float triangletype;
layout(location = 5) in float vertextype;  // -1.0 if it is the "left" end of the segment, 1.0 if it is the "right" end.
layout(location = 0) out vec4 primitivecolor;

layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    mat4 mapProjection;
    vec4 center;
    vec4 center_lowpart;
    float lineWidth;
    float aspect;
    int miter; // currently unused
    vec4 color;
    float wrapOffset;
};


vec4 wrapped(in vec4 v) { return vec4(v.x + wrapOffset, v.y, 0.0, 1.0); }
void main() { // ln 22
  primitivecolor = color;
  vec2 aspectVec = vec2(aspect, 1.0);
  mat4 projViewModel = qt_Matrix * mapProjection;
  vec4 cur = wrapped(vertex) - center;
  cur = cur - center_lowpart;
  vec4 prev = wrapped(previous) - center;
  prev = prev - center_lowpart;
  vec4 nex = wrapped(next) - center;
  nex = nex - center_lowpart;

  vec4 centerProjected = projViewModel * (center + vec4(0.0, 0.0, 0.0, 1.0));
  vec4 previousProjected = projViewModel * prev;
  vec4 currentProjected = projViewModel * cur;
  vec4 nextProjected = projViewModel * nex;

  //get 2D screen space with W divide and aspect correction
  vec2 currentScreen = (currentProjected.xy / currentProjected.w) * aspectVec;
  vec2 previousScreen = (previousProjected.xy / previousProjected.w) * aspectVec;
  vec2 nextScreen = (nextProjected.xy / nextProjected.w) * aspectVec;
  float len = (lineWidth);
  float orientation = direction;
  bool clipped = false;
  bool otherEndBelowFrustum = false;
  //starting point uses (next - current)
  vec2 dir = vec2(0.0);
  if (vertextype < 0.0) {
    dir = normalize(nextScreen - currentScreen);
    if (nextProjected.z < 0.0) dir = -dir;
  } else {
    dir = normalize(currentScreen - previousScreen);
    if (previousProjected.z < 0.0) dir = -dir;
  }
// first, clip current, and make sure currentProjected.z is > 0
  if (currentProjected.z < 0.0) {
    if ((nextProjected.z > 0.0 && vertextype < 0.0) || (vertextype > 0.0 && previousProjected.z > 0.0)) {
      dir = -dir;
      clipped = true;
      if (vertextype < 0.0 && nextProjected.y / nextProjected.w < -1.0) otherEndBelowFrustum = true;
      else if (vertextype > 0.0 && previousProjected.y / previousProjected.w < -1.0) otherEndBelowFrustum = true;
    } else {
        primitivecolor = vec4(0.0,0.0,0.0,0.0);
        gl_Position = vec4(-10000000.0, -1000000000.0, -1000000000.0, 1); // get the vertex out of the way if the segment is fully invisible
        return;
    }
  } else if (triangletype < 2.0) { // vertex in the view, try to miter
    //get directions from (C - B) and (B - A)
    vec2 dirA = normalize((currentScreen - previousScreen));
    if (previousProjected.z < 0.0) dirA = -dirA;
    vec2 dirB = normalize((nextScreen - currentScreen));
    //now compute the miter join normal and length
    if (nextProjected.z < 0.0) dirB = -dirB;
    vec2 tangent = normalize(dirA + dirB);
    vec2 perp = vec2(-dirA.y, dirA.x);
    vec2 vmiter = vec2(-tangent.y, tangent.x);
    len = lineWidth / dot(vmiter, perp);
// The following is an attempt to have a segment-length based miter threshold.
// A mediocre workaround until better mitering will be added.
    float lenTreshold = clamp( min(length((currentProjected.xy - previousProjected.xy) / aspectVec),
                               length((nextProjected.xy - currentProjected.xy) / aspectVec)), 3.0, 6.0 ) * 0.5;
    if (len < lineWidth * lenTreshold && len > -lineWidth * lenTreshold) {
       dir = tangent;
    } else {
       len = lineWidth;
    }
  }
  vec4 offset;
  if (!clipped) {
    vec2 normal = normalize(vec2(-dir.y, dir.x));
    normal *= len; // fracZL apparently was needed before the (-2.0 / qt_Matrix[1][1]) factor was introduced
    normal /= aspectVec;  // straighten the normal up again
    float scaleFactor =  currentProjected.w / centerProjected.w;
    offset = vec4(normal * orientation * scaleFactor * (centerProjected.w / (-2.0 / qt_Matrix[1][1])), 0.0, 0.0); // ToDo: figure out why (-2.0 / qt_Matrix[1][1]), that is empirically what works
    gl_Position = currentProjected + offset;
  } else {
     if (otherEndBelowFrustum) offset = vec4((dir * 1.0) / aspectVec, 0.0, 0.0);  // the if is necessary otherwise it seems the direction vector still flips in some obscure cases.
     else offset = vec4((dir * 500000000000.0) / aspectVec, 0.0, 0.0); // Hack alert: just 1 triangle, long enough to look like a rectangle.
     if (vertextype < 0.0) gl_Position = nextProjected - offset; else gl_Position = previousProjected + offset;
  }
}
