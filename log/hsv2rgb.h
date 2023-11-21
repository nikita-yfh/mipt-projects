#ifndef HSVRGB_H
#define HSVRGB_H

struct HSV {
	double H;
	double S;
	double V;
};

const char *HSVToRGB(char *buffer, struct HSV hsv);

#endif // HSVRGB_H

