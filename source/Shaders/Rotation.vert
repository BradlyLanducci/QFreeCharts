#version 440

layout(location = 0) in vec4 vertex; // Vertex position
layout(location = 1) in float theta;  // Rotation angle
layout(location = 2) in vec2 rotationPoint; // This will be (x1, y1)

layout(std140, binding = 0) uniform buf {
    mat4 modelMatrix;
} ubuf;

void main()
{
    // Create the rotation matrix
    mat4 rotationMatrix = mat4(
        cos(theta), -sin(theta), 0.0, 0.0,
        sin(theta),  cos(theta), 0.0, 0.0,
        0.0,         0.0,        1.0, 0.0,
        0.0,         0.0,        0.0, 1.0
    );

    // Translate to the rotation point, rotate, then translate back
    vec4 translatedVertex = vertex - vec4(rotationPoint.x, rotationPoint.y, 0.0, 0.0); // Translate to origin
    vec4 rotatedVertex = rotationMatrix * translatedVertex; // Apply rotation
    gl_Position = ubuf.modelMatrix * (rotatedVertex + vec4(rotationPoint, 0.0, 0.0)); // Translate back
}
