//precision highp float;

varying vec2 texture_coordinate;
uniform sampler2D my_color_texture;

void main()
{
    // Sampling The Texture And Passing It To The Frame Buffer
    gl_FragColor = texture2D(my_color_texture, texture_coordinate);
    //gl_FragColor = vec4(1.0,1.0,0.0,1.0);
}

