#version 300 es
precision mediump float;
//precision highp float;
//precision lowp float;
//in vec2 TexCoord;
in vec2 TexCoordCam;
in vec2 TexCoordAugm;
//in vec3 debug;
out vec4 frag_color;
precision mediump sampler2D;
uniform sampler2D IbisImg;
uniform sampler2D cameraImg;
//precision mediump sampler2DRect;
//uniform sampler2DRect cameraImg;

//variables to test edge detection/transparency
uniform int DisplayMode;
//display modes are: 1 -> AugmentationOnly ; 2 -> CameraOnly ; 3 -> AugmentedReality
uniform int AlphaChannelColour;
//alpha channel colors are: 1 -> red ; 2 -> green ; 3 -> blue
uniform bool UseTransparency;
uniform bool UseGradient;
uniform bool ShowMask;
uniform bool TrackerOK;
uniform vec2 TransparencyPosition;
uniform vec2 TransparencyRadius;
uniform vec2 Resolution;
uniform vec2 ImageOffset;
uniform vec2 ImageCenter;
uniform float LensDistortion;
uniform float GlobalOpacity;
uniform float Saturation;
uniform float Brightness;
uniform float GaussianFactor;

void main() {
    
    //hardcoded parameters:
    float GRADIENT_FACTOR = 2.;
    
    //convert positions and radii from screen space to texcoord space:
    vec2 Position_converted = TransparencyPosition / Resolution;
    //vertically flipped again:
    Position_converted.y = 1.-Position_converted.y;
    vec2 Radius_converted = TransparencyRadius / Resolution;
    float aspect_ratio = Resolution.x / Resolution.y;
    //dirty dirty dirty hack!
//    Position_converted.y = Position_converted.y / aspect_ratio;
    //compute the width and height of pixels in TexCoord space:
    float pixel_width = 1. / Resolution.x;
    float pixel_height = 1. / Resolution.y;
    
    vec4 augm = texture( IbisImg, TexCoordAugm );
    vec4 cam = texture( cameraImg, TexCoordCam );
    
    if( DisplayMode == 3 ){
        if( length(augm.rgb) > 0. ){
            float alphaness = 1.;
            if( AlphaChannelColour == 1 ){
                alphaness = max( augm.r - ( ( augm.g + augm.b ) / 2. ), 0. );
                augm.r = augm.r - alphaness;
            }else if( AlphaChannelColour == 2 ){
                alphaness = max( augm.g - ( ( augm.r + augm.b ) / 2. ), 0. );
                augm.g = augm.g - alphaness;
            }else if( AlphaChannelColour == 3 ){
                alphaness = max( augm.b - ( ( augm.r + augm.g ) / 2. ), 0. );
                augm.b = augm.b - alphaness;
            }
            augm.a = 1. - alphaness;
        }else{
            augm.a = 0.;
        }
    }

//    float max_intensity = max( max(augm.r, augm.g), augm.b);
//    augm.a = max_intensity;
//    float intensity = length(augm.rgb);
//    augm.a = intensity;

    //compute transparency factor of the augmentation according to distance to the center of the transparency circle:
    float factor = 0.0;
    if( UseTransparency ){

        //dirty hac to get a circle, but it shifts everything at the same time
        //so the dirty hack at line 33 becomes necessary:
//        float dist = distance( vec2(TexCoord.x,TexCoord.y*(1./aspect_ratio)), Position_converted );
        float dist = distance( vec2( TexCoordAugm.x ,TexCoordAugm.y ), Position_converted );
        //THIS IS NOT A CIRCLE SINCE THE TEXCOORD ARE IN A SQUARE SPACE, AND IT IS TRANSFORMED AFTER TO SCREEN SPACE, SO IT'S SCALED AND THE CIRCLE GETS ELLIPSED
        //SO, I GUESS I WOULD NEED TO COMPENSATE FOR THAT EFFECT HERE, WHEN I SELECT THE DISTANCE...
        //original shader:
        if( dist < Radius_converted.y ){
            if( dist > Radius_converted.x ){
                float ratio = ( dist - Radius_converted.x ) / ( Radius_converted.y - Radius_converted.x );
//                factor = exp(-(ratio*ratio)/0.25);
                factor = exp(-(ratio*ratio)/GaussianFactor);
            }
            else
                factor = 1.0;
        }
        //a square with gaussian decay:
//        if ( abs(TexCoord.x - Position_converted.x) < Radius_converted.x){
//            if ( abs(TexCoord.y - Position_converted.y) < Radius_converted.y){
//                 factor = exp(-(dist*dist)/0.1);
//            }
//        }
        //a circle with gaussian decay:
//        if ( dist < Radius_converted.x ){       //we use only the first coordinate as the radius
//            factor = exp(-(dist*dist)/GaussianFactor);
//        }
    }
    
    //use gradient:
    float alpha = 1.0;
    if( UseGradient ){
        if( factor > 0.01 ){
            vec4 gx = texture( cameraImg, TexCoordCam + vec2(-pixel_width,-pixel_height) );
            gx -= texture( cameraImg, TexCoordCam + vec2(pixel_width,pixel_height) );
            gx += 2.0 * texture( cameraImg, TexCoordCam + vec2(-pixel_width,0.0) );
            gx -= 2.0 * texture( cameraImg, TexCoordCam + vec2(pixel_width,0.0) );
            gx += texture( cameraImg, TexCoordCam + vec2(-pixel_width,pixel_height) );
            gx -= texture( cameraImg, TexCoordCam + vec2(pixel_width,-pixel_height) );
            vec4 gy = texture( cameraImg, TexCoordCam + vec2(-pixel_width,pixel_height) );
            gy -= texture( cameraImg, TexCoordCam + vec2(-pixel_width,-pixel_height) );
            gy += 2.0 * texture( cameraImg, TexCoordCam + vec2(0.0,pixel_height) );
            gy -= 2.0 * texture( cameraImg, TexCoordCam + vec2(0.0,-pixel_height) );
            gy += texture( cameraImg, TexCoordCam + vec2(pixel_width,pixel_height) );
            gy -= texture( cameraImg, TexCoordCam + vec2(pixel_width,-pixel_height) );
            vec2 gradAll = vec2( length( gx ), length( gy ) );
//            alpha = ( 1.0 - factor ) + factor * length( gradAll );
//            alpha = ( 1.0 - factor ) + length(gradAll);
            alpha = (1.-GRADIENT_FACTOR*length(gradAll));
        }
//        else{
//            alpha = 1.0 - factor;
//        }
    }
    
    //Do the mix!:
    if( DisplayMode == 1 ){
        // Bypass camera frames to visualize augm/virtual images only.
        frag_color = augm;
    }else if ( DisplayMode == 2 ){
        frag_color = cam;
    }else{
        // Allow augmentation
        frag_color = mix( cam, augm, alpha * factor * augm.a );
        //turn view red when tracking is lost:
        if( !TrackerOK ){
            frag_color.r = 1.;
        }
    }
    
    //to debug vertex shader:
//    frag_color.r = debug.x;
//    frag_color.g = debug.y;
//    frag_color.b = debug.z;
}
