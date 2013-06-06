uniform sampler2D colorMap;
uniform sampler2D normalMap;
varying vec4 texCoord;
uniform vec2 light;
uniform vec2 screen;
uniform float dist;

void main() {
    vec3 normal = texture2D(normalMap, texCoord.st).rgb;
    normal = 2.0*normal-1.0;
    vec3 n = normalize(normal);
    vec3 l = normalize(vec3((gl_FragCoord.xy-light.xy)/screen, dist));
    float a = dot(n, l);
    gl_FragColor = a*texture2D(colorMap, texCoord.st);
}
