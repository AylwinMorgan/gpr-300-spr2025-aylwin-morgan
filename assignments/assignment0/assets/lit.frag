#version 450
out vec4 FragColor; // The color of this fragment

in Surface{
	vec3 WorldPos; //Vertex position in world space
	vec3 WorldNormal; // vertex normal in world space
	vec2 TexCoord;
	mat3 TBN;
}fs_in;

uniform sampler2D _MainTex; // 2D texture sampler
// light pointing straight down
uniform vec3 _EyePos;
uniform vec3 _LightDirection = vec3(0.0,-1.0,0.0);
uniform vec3 _LightColor = vec3(1.0); // white light
uniform vec3 _AmbientColor = vec3(0.3,0.4,0.46);
uniform sampler2D normalMap;

struct Material{
	float Ka; //ambient coefficient (0-1)
	float Kd; //diffuse coefficient (0-1)
	float Ks; //specular coefficient (0-1)
	float Shininess; //Affects size of specular highlight
};
uniform Material _Material;

void main(){
	// make sure fragment normal is still length 1 after interpolation
	vec3 normal = texture(normalMap, fs_in.TexCoord).rgb;
	normal = normal * 2.0 - 1.0;
	normal = normalize(fs_in.TBN * fs_in.WorldNormal);

	// light pointing straight down
	vec3 toLight = fs_in.TBN * normalize(_LightDirection - fs_in.WorldPos);
	float diffuseFactor = max(dot(normal,toLight),0.0);

	//direction towards eye
	vec3 toEye = fs_in.TBN * normalize(_EyePos - fs_in.WorldPos);

	// blinn-phong uses half angle
	vec3 h = normalize(toLight + toEye);
	float specularFactor = pow(max(dot(normal,h),0.0),_Material.Shininess);

	// amount of light diffusely reflecting off surface
	vec3 lightColor = (_Material.Kd * diffuseFactor + _Material.Ks * specularFactor) * _LightColor;
	lightColor += _AmbientColor * _Material.Ka;
	vec3 objectColor = texture(_MainTex,fs_in.TexCoord).rgb;
	FragColor = vec4(objectColor * lightColor,1.0);
}