#version 460

in vec2 textCoords;
out vec4 fragColor;
uniform sampler2D texture3;

void main()
{
    fragColor = texture(texture3, textCoords);
}