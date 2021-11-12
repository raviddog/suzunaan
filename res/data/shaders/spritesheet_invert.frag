#version 330 core

in vec2 txCoord;

uniform sampler2D txUnit;

void main()
{
	bool invert = false;
	if(distance(vec2(600.0, 440.0), gl_FragCoord.xy) < 320.0) invert = !invert;
	if(distance(vec2(600.0, 520.0), gl_FragCoord.xy) < 320.0) invert = !invert;
	if(distance(vec2(680.0, 520.0), gl_FragCoord.xy) < 320.0) invert = !invert;
	if(distance(vec2(680.0, 440.0), gl_FragCoord.xy) < 320.0) invert = !invert;

	if(invert) {
		vec4 color = texture(txUnit, txCoord);
		gl_FragColor = vec4(1.0 - color.r, 1.0 - color.g, 1.0 - color.b, color.a);
	} else {
		gl_FragColor = texture(txUnit, txCoord);
	}
}