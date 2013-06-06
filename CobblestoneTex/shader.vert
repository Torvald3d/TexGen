varying vec4 texCoord;

void main(){
    gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;
    texCoord = gl_MultiTexCoord0;
}
