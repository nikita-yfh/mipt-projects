#include "hsv2rgb.h"

#include <math.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <float.h>

const char *HSVToRGB(char *buffer, struct HSV hsv) {
	assert(buffer);

	double r = 0, g = 0, b = 0;

	if (fabs(hsv.S) < DBL_EPSILON) {
		r = hsv.V;
		g = hsv.V;
		b = hsv.V;
	} else {
		int i;
		double f, p, q, t;

		hsv.H = fmod(hsv.H, 360.0) / 60.0;

		i = (int) trunc(hsv.H);
		f = (double) hsv.H - i;

		p = hsv.V * (1.0 - hsv.S);
		q = hsv.V * (1.0 - (hsv.S * f));
		t = hsv.V * (1.0 - (hsv.S * (1.0 - f)));

		switch (i) {
		case 0:
			r = hsv.V;
			g = t;
			b = p;
			break;

		case 1:
			r = q;
			g = hsv.V;
			b = p;
			break;

		case 2:
			r = p;
			g = hsv.V;
			b = t;
			break;

		case 3:
			r = p;
			g = q;
			b = hsv.V;
			break;

		case 4:
			r = t;
			g = p;
			b = hsv.V;
			break;

		default:
			r = hsv.V;
			g = p;
			b = q;
			break;
		}
	}

	snprintf(buffer, 8, "#%02X%02X%02X", (uint8_t) (r * 255),
	                                     (uint8_t) (g * 255),
	                                     (uint8_t) (b * 255));

	return buffer;
}

