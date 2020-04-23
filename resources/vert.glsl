#version 300 es
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 textureCoordinate;
out vec2 TexCoordCam;
out vec2 TexCoordAugm;
//out vec3 debug;
uniform float aspect_ratio_orig_cam;
uniform float aspect_ratio_screen;
uniform vec2 Screen_resolution;
uniform vec2 Camera_resolution;
void main() {
    gl_Position = vec4( vertexPosition, 1.0 );
    //THE Y AXIS MUST BE FLIPPED
    //IT HAS TO DO WITH THE CONVENTION ON THE ORIGIN OF COORDINATES
    //OPENGL HAS BOTTOM AS 0; QIMAGE IS THE OPPOSITE
//    TexCoordAugm = vec2( textureCoordinate.x , 1. - textureCoordinate.y );
    TexCoordAugm = vec2( textureCoordinate.x , textureCoordinate.y );
    //HACK TO MAKE NEW CAMERA IMAGE DIMENSIONS FIT ON SCREEN, UNTIL THE BUG IS FIXED BY QT
    float ratio = aspect_ratio_screen / aspect_ratio_orig_cam;
    
//    if( textureCoordinate.x < 0.5 ){
//        TexCoordCam = vec2( textureCoordinate.x / ratio , 1. - textureCoordinate.y );
//    }else{
//        TexCoordCam = vec2( textureCoordinate.x * ratio , 1. - textureCoordinate.y );
//    }
    TexCoordCam = vec2( textureCoordinate.x , 1. - textureCoordinate.y );
//    debug = vec3( aspect_ratio_orig_cam-1., aspect_ratio_screen-1., ratio-1. );
}
