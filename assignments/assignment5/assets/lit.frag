#version 450
out vec4 FragColor; // The color of this fragment
in Surface{
	vec3 WorldPos; //Vertex position in world space
	vec3 WorldNormal; // vertex normal in world space
	vec2 TexCoord;
}fs_in;
in vec4 fragPosLight;
uniform sampler2D _MainTex; // 2D texture sampler
uniform sampler2D shadowMap;


// light pointing straight down
uniform vec3 _EyePos;
uniform vec3 _LightDirection = vec3(0.0,-1.0,0.0);
uniform vec3 _LightColor = vec3(1.0); // white light
uniform vec3 _AmbientColor = vec3(0.3,0.4,0.46);
struct Material{
	float Ka; //ambient coefficient (0-1)
	float Kd; //diffuse coefficient (0-1)
	float Ks; //specular coefficient (0-1)
	float Shininess; //Affects size of specular highlight
};
uniform Material _Material;
void main(){
	float shadow = 0.0f;
	vec3 lightCoords = fragPosLight.xyz / fragPosLight.w;
	if(lightCoords.z <= 1.0f){
		lightCoords = (lightCoords + 1.0f) / 2.0f;
		float closestDepth = texture(shadowMap, lightCoords.xy).r;
		float currentDepth = lightCoords.z;

		if (currentDepth > closestDepth){
			shadow = 1.0f;
		}
	}
	// make sure fragment normal is still length 1 after interpolation
	vec3 normal = normalize(fs_in.WorldNormal);
	// light pointing straight down
	vec3 toLight = -_LightDirection;
	float diffuseFactor = max(dot(normal,toLight),0.0);
	//direction towards eye
	vec3 toEye = normalize(_EyePos +- fs_in.WorldPos);
	// blinn-phong uses half angle
	vec3 h = normalize(toLight + toEye);
	float specularFactor = pow(max(dot(normal,h),0.0),_Material.Shininess);
	// amount of light diffusely reflecting off surface
	vec3 lightColor = (_Material.Kd * diffuseFactor * (1.0f - shadow) + _Material.Ks * specularFactor * (1.0f - shadow)) * _LightColor;
	lightColor += _AmbientColor * _Material.Ka;
	vec3 objectColor = texture(_MainTex,fs_in.TexCoord).rgb;
	FragColor = vec4(objectColor * lightColor,1.0);
}