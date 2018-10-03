uniform vec2 resolution;
uniform sampler2DRect tex;
uniform int mono;
uniform int posterization;
uniform int less_resolution;
uniform int laplacian;

void main(){
    vec2 st = gl_TexCoord[0].xy;
    vec2 uv = st / resolution;
 
    vec4 texColor = texture2DRect(tex, st);
    vec4 color = texColor;
    
    if(mono == 1){
        color.rgb = vec3((texColor.r + texColor.g + texColor.b)/3.0);
    }
    
    if(posterization > 1){
        
        for(int i=0; i<posterization; i++){
            float unit = 1.0/float(posterization);
            float range_min = unit*float(i);
            float range_max = unit*float(i+1);
            
            if( range_min <= color.r && color.r <= range_max ){
                if(range_min == 0.0) color.rgb = vec3(0.0);
                else if(range_max == 1.0) color.rgb = vec3(1.0);
                else{
                    float low = max(range_min,0.0);
                    float high = min(range_max,1.0);
                    float middle = (low + high)/2.0;
                    color.rgb = vec3(middle);
                }
                break;
            }
        }        
    }else if(posterization == 1){
        if(color.r < 0.5) color.rgb = vec3(0.0);
        else color.rgb = vec3(1.0);
    }
    
    if(less_resolution == 1){
        
    }
    
    if(laplacian == 1){
        
        float coef[9];
        coef[0] = 1.0;
        coef[1] = 1.0;
        coef[2] = 1.0;
        coef[3] = 1.0;
        coef[4] = -8.0;
        coef[5] = 1.0;
        coef[6] = 1.0;
        coef[7] = 1.0;
        coef[8] = 1.0;
        
        vec2 offset[9];
        offset[0] = vec2(-1.0, -1.0);
        offset[1] = vec2(0.0, -1.0);
        offset[2] = vec2(1.0, -1.0);
        offset[3] = vec2(-1.0, 0.0);
        offset[4] = vec2(0.0, 0.0);
        offset[5] = vec2(1.0, 0.0);
        offset[6] = vec2(-1.0, 1.0);
        offset[7] = vec2(0.0, 1.0);
        offset[8] = vec2(1.0, 1.0);
        
        vec4 destColor = vec4(0.0);
        for(int i=0; i<9; i++){
            destColor += texture2DRect(tex, st+offset[i])*coef[i];
        }
        destColor = max(destColor, 0.0);
        float grayColor = (destColor.r + destColor.g + destColor.b)/3.0;
        
        color.rgb = vec3(grayColor);
    }
    

    gl_FragColor = color;
}





