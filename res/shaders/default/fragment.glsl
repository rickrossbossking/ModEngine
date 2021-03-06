#version 330 core

in vec3 FragPos;
in vec2 TexCoord;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D coolzero;
uniform sampler2D coolsomevalue;
uniform sampler2D maintexture;

uniform vec3 tint;
uniform vec3 cameraPosition;

uniform bool enableDiffuse;
uniform bool enableSpecular;

#define MAX_LIGHTS 32
uniform int numlights;
uniform vec3 lights[MAX_LIGHTS];

const float constant = 0.4;
const float linear = 0.2;
const float quadratic = 0.0;

void main(){
  if (tint.r < 0.1){
    FragColor = vec4(tint.r, tint.g, tint.b, 1.0);
  }else{
    //vec4 texColor = (texture(coolzero, TexCoord) + texture(coolsomevalue, TexCoord)) / 2;
    //vec4 texColor = 
    vec4 texColor = texture(maintexture, TexCoord);
    
    if (texColor.a < 0.1){
      discard;
    }

    vec3 ambient = vec3(0.2, 0.2, 0.2);     
    vec3 totalSpecular = vec3(0, 0, 0);
    vec3 totalDiffuse  = vec3(0, 0, 0);
    
    for (int i = 0; i < min(numlights, MAX_LIGHTS); i++){
        vec3 lightPos = lights[i];
        vec3 lightDir = normalize(lightPos - FragPos);
        vec3 normal = normalize(Normal);

        vec3 diffuse = max(dot(normal, lightDir), 0.0) * vec3(1.0, 1.0, 1.0);

        vec3 viewDir = normalize(cameraPosition - FragPos);
        vec3 reflectDir = reflect(-lightDir, normal);  
        vec3 specular = pow(max(dot(viewDir, reflectDir), 0.0), 32) * vec3(1.0, 1.0, 1.0);  
 
        float distanceToLight = length(lightPos - FragPos);
        float attenuation = 1.0 / (constant + linear * distanceToLight + quadratic * (distanceToLight * distanceToLight));  

        totalDiffuse = totalDiffuse + (attenuation * diffuse);
        totalSpecular = totalSpecular + (attenuation * specular);
    }

    vec3 diffuseValue = enableDiffuse ? totalDiffuse : vec3(0, 0, 0);
    vec3 specularValue = enableSpecular ? totalSpecular : vec3(0, 0, 0);
    vec4 color = vec4(ambient + diffuseValue + specularValue, 1.0) * texColor;
    FragColor = color * vec4(tint.x, tint.y, tint.z, 1.0);
  }
}
