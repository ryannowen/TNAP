#version 330

struct SLightData
{
	// Types
	// 0 = Directional
	// 1 = Point
	// 2 = Spot

	int light_type; 
	vec3 light_position;
	vec3 light_direction;
	float light_fov;
	vec3 light_colour;
	float light_range;
	float light_intensity;
};

struct SMaterial
{
	vec3 ambient_Colour;
	vec3 specular_Colour;
	float specular_Intensity;
};

in vec3 varying_position;
in vec2 varying_uv;
in vec3 varying_normal;

out vec4 fragment_colour;

uniform mat4 model_xform;
uniform sampler2D texSample;
uniform vec3 camera_direction;

uniform SMaterial material;

uniform bool hasLighting;
uniform int numOfLights;
uniform SLightData lights[20];

void main(void)
{
	vec3 ambient_colour = max(vec3(0.03), material.ambient_Colour);
	vec3 specular_Colour = material.specular_Colour;
	float specular_intensity = material.specular_Intensity;
	vec4 diffuse_colour = max(vec4(0.1), texture(texSample, varying_uv));

	vec3 normal = normalize(varying_normal);
	vec3 fragmentPos = varying_position;


	fragment_colour += diffuse_colour;

	if (hasLighting)
	{
		fragment_colour *= vec4(ambient_colour, 1);

		// Loops through all lights
		for (int i = 0; i < numOfLights; i++)
		{
			vec3 L;
			float attenuation = 1.0;

			if (lights[i].light_type == 0) // Directional Light
			{
				L = normalize(-lights[i].light_direction);
			}
			else // Not a Directional Light
			{
				L = normalize(lights[i].light_position - fragmentPos);

				// Attenuation
				float light_distance = distance(lights[i].light_position, fragmentPos);
				attenuation = 1.0 - smoothstep(0, lights[i].light_range, light_distance);
			}

			// Diffuse
			vec3 diffuse_intensity = max(vec3(0.0), dot(L, normal));
			diffuse_intensity *= max(vec3(0.1), lights[i].light_colour) * max(0.01, lights[i].light_intensity);


			// Specular
			vec3 specular = vec3(0);

			if (specular_intensity > 0)
			{
				vec3 rV = reflect(L, normal);
				float LR = max(0, dot(camera_direction, rV));
				specular = specular_Colour * pow(LR, specular_intensity);
			}


			// Spot light
			if (lights[i].light_type == 2) 
			{
				attenuation *= smoothstep(cos(0.5 * lights[i].light_fov), 1, dot(-L, lights[i].light_direction));
			}

			// Final Calculation
			fragment_colour += vec4(((diffuse_colour.rgb + specular) * diffuse_intensity * attenuation), 0.0);
		}
	}

	//fragment_colour = vec4(varying_normal, 1.0);
	//fragment_colour = diffuse_colour;
}