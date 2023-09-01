#include <stdio.h>

#include "mystring.h"

int main() {
	// strlen test
	const char *string1 = "Dumb test of strlen";
	printf("mystrlen(\"%s\") = %lu\n", string1, mystrlen(string1));
	putchar('\n');

	// strcat test
	char catstr[26] = "Hello ";
	const char *string2 = "phystech! ";
	const char *string3 = "We botaem every day!";
	printf("mystrcat(\"%s\", \"%s\", %lu) = \"%s\"\n", catstr, string2, sizeof(catstr), mystrncat(catstr, string2, sizeof(catstr)));
	printf("mystrcat(\"%s\", \"%s\", %lu) = \"%s\"\n", catstr, string3, sizeof(catstr), mystrncat(catstr, string3, sizeof(catstr)));
	putchar('\n');

	const char *string4 = "tetettestxtstsgdffwetegdfgd";
	const char *string5 = "jasdghksjdfgnsdfgndsfkjgnds";
	const char *string6 = "test";
	printf("mystrstr1(\"%s\", \"%s\") = \"%s\"\n", string4, string6, mystrstr1(string4, string6));
	printf("mystrstr1(\"%s\", \"%s\") = \"%s\"\n", string5, string6, mystrstr1(string5, string6));
	putchar('\n');
	printf("mystrstr2(\"%s\", \"%s\") = \"%s\"\n", string4, string6, mystrstr2(string4, string6));
	printf("mystrstr2(\"%s\", \"%s\") = \"%s\"\n", string5, string6, mystrstr2(string5, string6));
	putchar('\n');
	printf("mystrstr3(\"%s\", \"%s\") = \"%s\"\n", string4, string6, mystrstr3(string4, string6));
	printf("mystrstr3(\"%s\", \"%s\") = \"%s\"\n", string5, string6, mystrstr3(string5, string6));
	putchar('\n');


	return 0;
}

