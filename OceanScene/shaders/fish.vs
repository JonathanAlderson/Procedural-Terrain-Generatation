#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 offsets;

out vec2 TexCoords;
out float ID;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


// Code for rotatings
mat4 rotationMatrix(vec3 axis, float angle) {
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

float rand(vec2 c){
	return fract(sin(dot(c.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

/* float noise(vec2 p, float freq ){
	float unit = screenWidth/freq;
	vec2 ij = floor(p/unit);
	vec2 xy = mod(p,unit)/unit;
	//xy = 3.*xy*xy-2.*xy*xy*xy;
	xy = .5*(1.-cos(PI*xy));
	float a = rand((ij+vec2(0.,0.)));
	float b = rand((ij+vec2(1.,0.)));
	float c = rand((ij+vec2(0.,1.)));
	float d = rand((ij+vec2(1.,1.)));
	float x1 = mix(a, b, xy.x);
	float x2 = mix(c, d, xy.x);
	return mix(x1, x2, xy.y);
}

float pNoise(vec2 p, int res){
	float persistance = .5;
	float n = 0.;
	float normK = 0.;
	float f = 4.;
	float amp = 1.;
	int iCount = 0;
	for (int i = 0; i<50; i++){
		n+=amp*noise(p, f);
		f*=2.;
		normK+=amp;
		amp*=persistance;
		if (iCount == res) break;
		iCount++;
	}
	float nf = n/normK;
	return nf*nf*nf*nf;
} */


void main()
{
    // Move the model
    mat4 movedModel = model;
    vec3 pos = offsets;
    movedModel[3][0] = pos.x; //pos.x;
    movedModel[3][1] = pos.y;//pos.y;
    movedModel[3][2] = pos.z; //pos.z;

    // Rotate the fish randomly
    //mat4 r = rotationMatrix(vec3(0., 1., 0.), gl_InstanceID);
    //movedModel = movedModel * r;

    // Scale the fish randomly
    vec3 vert = aPos;
    float scale = .2; //0.2 + .2 * rand(vec2(gl_InstanceID, 36.7));
    vert = vert * scale;


    // Outputs
    ID = gl_InstanceID;
    TexCoords = aTexCoords;
    gl_Position = projection * view * movedModel * vec4(vert, 1.0);
}
