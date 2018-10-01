uniform vec2 resolution;
uniform sampler2DRect tex;
uniform int mono;
uniform int posterization;
uniform int less_resolution;

void main(){
    vec2 st = gl_TexCoord[0].xy;
    vec2 uv = st / resolution;
 
    vec4 texColor = texture2DRect(tex, st);
    vec4 color = texColor;
    
    if(mono == 1){
        color.rgb = vec3((texColor.r + texColor.g + texColor.b)/3.0);
    }
    
    if(posterization > 1){
        
        float unit = 1.0/float(posterization);
        for(int i=0; i<posterization; i++){
            float low = max(unit*float(i),0.0);
            float high = min(unit*float(i+1),1.0);
            float middle = (low + high)/2.0;
            
            if( color.r == 0.0) color.rgb = vec3(0.0);
            else if( color.r == 1.0) color.rgb = vec3(1.0);
            else if( low <= color.r && color.r <= high) color.rgb = vec3(middle);
            
        }
    }else if(posterization == 1){
        if(color.r < 0.5) color.rgb = vec3(0.0);
        else color.rgb = vec3(1.0);
    }
    
    if(less_resolution == 1){
        
    }

    gl_FragColor = color;
}