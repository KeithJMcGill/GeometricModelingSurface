#version 150 


in  vec4 vPosition;
in  vec3 vNormal;
out vec4 color;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform vec4 LightPosition;
uniform float Shininess;
void main()
{
	vec3 L = normalize( LightPosition.xyz - vPosition.xyz );
    vec3 E = normalize( -vPosition.xyz );
    vec3 H = normalize( L + E );

    // Compute terms in the illumination equation
    vec4 ambient = AmbientProduct;

    float Kd = max( dot(L, vNormal), 0.0 );
    vec4  diffuse = Kd*DiffuseProduct;

    float Ks = pow( max(dot(vNormal, H), 0.0), Shininess );
    vec4  specular = Ks * SpecularProduct;
    
    if( dot(L, vNormal) < 0.0 ) {
	specular = vec4(0.0, 0.0, 0.0, 1.0);
    } 

    gl_Position = vPosition;

    color = ambient + diffuse + specular;
    color.a = 1.0;
}