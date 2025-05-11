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
	ivec2 ires = textureSize(uImageUnit, 0);
	float ResS = float(ires.s);
	float ResT = float(ires.t);

	vec2 st = vST;

	vec3 irgb = texture2D(uImageUnit, st ).rgb;

	vec2 stp0 = vec2(1./ResS, 0.);
	vec2 st0p = vec2(0., 1./ResT);
	vec2 stpp = vec2(1./ResS, 1./ResT);
	vec2 stpm = vec2(1./ResS, -1./ResT);

	vec3 i00 = texture( uImageUnit, vST ).rgb;
	vec3 im1m1 = texture( uImageUnit, vST-stpp ).rgb;
	vec3 ip1p1 = texture( uImageUnit, vST+stpp ).rgb;
	vec3 im1p1 = texture( uImageUnit, vST-stpm ).rgb;
	vec3 ip1m1 = texture( uImageUnit, vST+stpm ).rgb;
	vec3 im10 = texture( uImageUnit, vST-stp0 ).rgb;
	vec3 ip10 = texture( uImageUnit, vST+stp0 ).rgb;
	vec3 i0m1 = texture( uImageUnit, vST-st0p ).rgb;
	vec3 i0p1 = texture( uImageUnit, vST+st0p ).rgb;

	vec3 blur = vec3(0.,0.,0.);
	blur += 1.*(im1m1+ip1m1+ip1p1+im1p1);
	blur += 2.*(im10+ip10+i0m1+i0p1);
	blur += 4.*(i00);
	blur /= 16.;

	gl_FragColor = vec4( mix( blur, irgb, t ), 1. );

	
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