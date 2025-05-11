#version 330 compatibility
uniform sampler2D	uImageUnit;
uniform float t; //for filter strength
//uniform float   uKa, uKd, uKs;	 // coefficients of each type of lighting -- make sum to 1.0
//uniform vec4    uSpecularColor;	 // light color
//uniform float   uShininess;	 // specular exponent

in vec2			vST;
in vec3			vN;
in vec3			vL;
in vec3			vE;

//in vec4 OriginalColor;
//out vec4 FinalColor;



void
main()
{
	//ivec2 ires = textureSize(uImageUnit, 0);
	//float ResS = float(ires.s);
	//float ResT = float(ires.t);

	vec2 st = vST;

	vec3 irgb = texture2D(uImageUnit, st ).rgb;
	vec3 target = vec3(0., 0., 0.);
	
	gl_FragColor = vec4(mix( target, irgb, t), 1.);

	//gl_FragColor = vec4( mix( blur, irgb, t ), 1. );

	
	//vec3 rgbBefore = texture2D(uTexUnitB, vST).rgb;
	//vec3 rgbAfter = texture2D(uTexUnitA, vST).rgb;
	
	//vec3 target = abs(rgbAfter - rgbBefore);
	//vec4 myColor;
	//vec3 whiteColor = vec3(1.0, 1.0, 1.0);

	//if (t <= 1. )
	//	gl_FragColor = vec4(mix(rgbAfter, rgbBefore, t), 1.0 );
	////	myColor = vec4(mix(rgbAfter, rgbBefore, t), 1.0 );
	//else
	//	gl_FragColor = vec4(mix(rgbBefore, target, t-1. ), 1.0);
	////	myColor = vec4(mix(rgbBefore, target, t-1. ), 1.0);

	//gl_FragColor = vec4(mix(rgbAfter, rgbBefore, t), 1.0);

	//FinalColor = vec4(mix(myColor.rgb, whiteColor, t), 1.0);
	//FinalColor = gl_FragColor;
	//gl_FragColor = vec4(rgbBefore, 1.0);
	//gl_FragColor = vec4(mix(vec3(1.0, 1..0, 1..0), gl_FragColor.rgb, t), 1.0);
	//gl_FragColor = vec4(mix(rgbBefore, rgbAfter, t).rgb, 1.);
	////////////////////////////
	//vec3 myColor = gl_FragColor.rgb;

	//vec3 Normal = normalize(vN);
	//vec3 Light  = normalize(vL);
	//vec3 Eye    = normalize(vE);

	//vec3 ambient = uKa * myColor;

	//float dd = max( dot(Normal,Light), 0. );       // only do diffuse if the light can see the point
	//vec3 diffuse = uKd * dd * myColor;

	//float ss = 0.;
	//if( dot(Normal,Light) > 0. )	      // only do specular if the light can see the point
	//{
	//	vec3 ref = normalize(  reflect( -Light, Normal )  );
	//	ss = pow( max( dot(Eye,ref),0. ), uShininess );
	//}
	//vec3 specular = uKs * ss * uSpecularColor.rgb;
	//gl_FragColor = vec4( ambient + diffuse + specular,  1. );
}