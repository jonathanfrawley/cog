//attribute vec4 position;

void main()
{
//    gl_Position=position;
    // Transforming The Vertex
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
