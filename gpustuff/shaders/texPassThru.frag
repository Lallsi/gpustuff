#version 450 core

layout(location = 0) out vec4 color;
uniform sampler2D tex;
void main()
{
    ivec2 screenpos = ivec2(gl_FragCoord.xy);
    color = texelFetch(tex, screenpos, 0);
}